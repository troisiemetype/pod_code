// #include "audio.h"
#include "esPod.h"

Audio audio;

//int8_t audioBuffer[AUDIO_BUFFER_SIZE];

const uint8_t I2S_CLK = 5;
const uint8_t I2S_DATA = 17;
const uint8_t I2S_WS = 16;

audioState_t audioState = AUDIO_IDLE;

AudioTrackData *current = NULL;
MenuItem *currentItem = NULL;

bool playing = false;
SemaphoreHandle_t audioUpdateMutex;

hw_timer_t *audioTimer = NULL;
// portMUX_TYPE audioTimerMux= portMUX_INITIALIZER_UNLOCKED;

// notes
// I2S has a 64 bytes buffer and uses DMA.
// player->loop() takes around 650us to run. On begin it's 11650us, and can take 2500us from time to time

volatile uint32_t audioCounter = 0;
uint32_t prevAudioCounter = 0;
volatile bool audioCounterFlag = false;
uint8_t audioGain = 63;
const uint8_t MAX_AUDIO_GAIN = 63;
uint16_t muteDelay = 50;						// The time wanted for mute / unmute, in milliseconds

void audio_init(){

	audio.setPinout(I2S_CLK, I2S_WS, I2S_DATA);
	audio.setVolumeSteps(MAX_AUDIO_GAIN);
	audio.setVolume(MAX_AUDIO_GAIN);

	audioUpdateMutex = xSemaphoreCreateMutex();

	audioState = AUDIO_IDLE;

//	Todo : change the audio update to 1 second.
//	or, better : change it to some value that will give a smooth progress bar whatever the size of the track.
//	But 1ms is clearlry a waste of processor ressource !
	audioTimer = timerBegin(0, 80, true);
	timerAttachInterrupt(audioTimer, &audio_int, true);
	timerAlarmWrite(audioTimer, 1000, true);

//	timerAlarmEnable(audioTimer);

}

bool audio_update(){
//	if(!playing) return false;
	if(audioState == AUDIO_PLAY || audioState == AUDIO_MUTING || audioState == AUDIO_UNMUTING){
		xSemaphoreTake(audioUpdateMutex, portMAX_DELAY);
		audio.loop();
		xSemaphoreGive(audioUpdateMutex);

		if(audioCounterFlag){
			audioCounterFlag = false;
			if(audioState == AUDIO_MUTING){
//				audio_mute();
			} else if(audioState == AUDIO_UNMUTING){
//				audio_unmute();
			}
			if(((audioCounter - prevAudioCounter) / 1000) >= 1){
				display_pushPlayerProgress(audioCounter / 1000, current->getDuration() / 1000);
				prevAudioCounter = audioCounter;
			}
		}
/*
		if(!running){
			audio_nextTrack();
		}
*/
		return true;
	}

	return false;
}

// Todo : add semaphore so audio_update() doesn't access audioCounter when it's being modified.
void audio_int(){
	if(audioState == AUDIO_PLAY || audioState == AUDIO_MUTING || audioState == AUDIO_UNMUTING){
		audioCounter++;
		audioCounterFlag = true;
	}
}

void audio_playTrack(MenuItem *item){
	// set track infos
	if(item != currentItem){
		currentItem = item;

		current = reinterpret_cast<AudioTrackData*>(item->getData());

		audio_stop();

		log_d("open %s for playing", current->getFilename());

		audio.connecttoFS(SD_MMC, current->getFilename());

		playing = audio.isRunning();

		if(playing){
			audioState = AUDIO_PLAY;
			audioCounter = 0;
			timerAlarmEnable(audioTimer);
		}
//		Serial.printf("update display %s : %i\n", current->getName(), playing);
		audio_updateDisplay();
	}

}

void audio_stop(){
	if(!playing) return;
	xSemaphoreTake(audioUpdateMutex, portMAX_DELAY);
	audioState = AUDIO_IDLE;
	playing = 0;
	audio.stopSong();
	xSemaphoreGive(audioUpdateMutex);
}

void audio_updateDisplay(){
	if(!current) return;
//	log_d("Free heap: %d", ESP.getFreeHeap());
	// Attach buttons callbacks
	io_deattachAllCB();
	io_attachCBRight(audio_nextTrack);
	io_attachCBLeft(audio_prevTrack);
	io_attachCBDown(audio_pause);
	io_attachCBUp(menu_update);

	display_setState(PLAYER);
	display_pushHeader("playing");
	display_pushClearDisplay();
	display_pushPlayer(current->getArtist(), current->getAlbum(), current->getName(), current->getTrack());
	display_pushPlayerProgress(audioCounter / 1000, current->getDuration() / 1000);	
}

void audio_nextTrack(){
	MenuItem *item = currentItem->getNext();
	if(!item){
//		menu_update();
//		MenuList *parent = currentItem->getParent();
//		parent->getFirst();
//		menu_cbList(parent);
		return;
	}

//	AudioTrackData *newTrack = reinterpret_cast<AudioTrackData*>(item->getData());

//	audio_stop();
	audio_playTrack(item);
}

// TODO : make the player call the menu from which the song was taken, when reaching first.
void audio_prevTrack(){
	MenuItem *item = currentItem->getPrevious();
	if((audioCounter / 1000) >= 2){
//		current = NULL;
		// Hack forcing audio_play to enter
		MenuItem *tmp = item;
		item = currentItem;
		currentItem = tmp;
	} else {
		if(!item) return;
	}
//	audio_stop();
	audio_playTrack(item);
}

void audio_pause(){
	if(audioState == AUDIO_PLAY){
		audioState = AUDIO_MUTING;
	} else {
		audioState = AUDIO_UNMUTING;
	}
	xTaskCreate(audio_muteThread, "audio mute", 1000, NULL, 1, NULL);
}

// Todo : the muteThread could use the soft mute function of PCM5102.
void audio_muteThread(void *params){
	TickType_t last;
	last = xTaskGetTickCount();
	// We want to update the volume every millisecond when (un)muting
	const TickType_t freq = 1 / portTICK_PERIOD_MS;
	for(;;){
		vTaskDelayUntil(&last, freq);
//		Serial.println(last);
		if(audioState == AUDIO_MUTING){
			audioGain--;
			audio.setVolume(audioGain);

			if(audioGain == 0){
				audioState = AUDIO_MUTE;
				audio.pauseResume();
				vTaskDelete(NULL);
			}
		} else if(audioState == AUDIO_UNMUTING){
			audioGain++;
			audio.setVolume(audioGain);

			if(audioGain >= MAX_AUDIO_GAIN){
				audioState = AUDIO_PLAY;
				audio.pauseResume();
				vTaskDelete(NULL);
			}
		}
	}
}

/*
// replaced by mute thread
void audio_mute(){
	audioGain -= (MAX_AUDIO_GAIN / muteDelay);
	audioOutput->SetGain(audioGain);
	if(audioGain <= 0){
		audioState = AUDIO_MUTE;
		audioOutput->stop();
	}
}

// Replaced by mute thread
void audio_unmute(){
	audioGain += (MAX_AUDIO_GAIN / muteDelay);
	audioOutput->SetGain(audioGain);
	if(audioGain >= 1.0){
		audioState = AUDIO_PLAY;
		audioOutput->begin();
	}
}
*/
audioState_t audio_getState(){
	return audioState;
}

bool audio_getTag(fs::File* file){
	// Here we have to implement our functions.
	return false;
}
