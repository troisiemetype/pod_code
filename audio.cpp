// #include "audio.h"
#include "esPod.h"

AudioFileSourceID3 *audioTags;
AudioFileSourceFS *audioFile;
AudioGenerator *player;
AudioOutputI2S *audioOutput;

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
float audioGain = 1.0;
const float MAX_AUDIO_GAIN = 0.9;
uint16_t muteDelay = 50;						// The time wanted for mute / unmute, in milliseconds

void audio_init(){
	audioOutput = new AudioOutputI2S();
	audioOutput->SetPinout(I2S_CLK, I2S_WS, I2S_DATA);
	audioFile = new AudioFileSourceFS(SD_MMC);
	audioTags = new AudioFileSourceID3(audioFile);
//	player = new AudioGeneratorMP3(audioBuffer, AUDIO_BUFFER_SIZE);
	player = new AudioGeneratorMP3();

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
		bool running = player->loop();
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

		if(!running){
			audio_nextTrack();
		}

		return running;
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

//		playing = 0;
//		audioFile->close();
		log_d("open %s for playing", current->getFilename());
		audioFile->open(current->getFilename());
//		log_d("file opened");
		// todo : stopping and changing file make the player reboot.
//		player->stop();
//		player = new AudioGeneratorMP3();
		// Modified constructor whit checked member set to true, to skip ID3 parsing.
		*audioTags = AudioFileSourceID3(audioFile, true);
//		log_d("add file");
//		*audioTags = AudioFileSourceID3(audioFile);
//		log_d("bit per channel : %i", audioOutput->SetBitsPerSample(16));
//		log_d("channels : %i", audioOutput->SetChannels(2));
//		log_d("begin : %i", audioOutput->begin());

		playing = player->begin(audioTags, audioOutput);
//		log_d("now playing");
		audioOutput->SetGain(MAX_AUDIO_GAIN);
//		Serial.printf("playing %s : %i\n", current->getName(), playing);
//		log_d("Free heap: %d", ESP.getFreeHeap());
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
	player->stop();
	audioFile->close();
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
		audioOutput->begin();
	}
	xTaskCreate(audio_muteThread, "audio mute", 1000, NULL, 1, NULL);
}

void audio_muteThread(void *params){
	TickType_t last;
	last = xTaskGetTickCount();
	// We want to update the volume every millisecond when (un)muting
	const TickType_t freq = 1 / portTICK_PERIOD_MS;
	for(;;){
		vTaskDelayUntil(&last, freq);
//		Serial.println(last);
		if(audioState == AUDIO_MUTING){
			audioGain -= MAX_AUDIO_GAIN / (float)muteDelay;
			audioOutput->SetGain(audioGain);

			if(audioGain <= 0.0){
				audioState = AUDIO_MUTE;
				audioOutput->stop();
				vTaskDelete(NULL);
			}
		} else if(audioState == AUDIO_UNMUTING){
			audioGain += MAX_AUDIO_GAIN / (float)muteDelay;
			audioOutput->SetGain(audioGain);

			if(audioGain >= 1.0){
				audioState = AUDIO_PLAY;
//				audioOutput->begin();
				vTaskDelete(NULL);
			}
		}

	}
}

void audio_mute(){
	audioGain -= (MAX_AUDIO_GAIN / muteDelay);
	audioOutput->SetGain(audioGain);
	if(audioGain <= 0){
		audioState = AUDIO_MUTE;
		audioOutput->stop();
	}
}

void audio_unmute(){
	audioGain += (MAX_AUDIO_GAIN / muteDelay);
	audioOutput->SetGain(audioGain);
	if(audioGain >= 1.0){
		audioState = AUDIO_PLAY;
		audioOutput->begin();
	}
}

audioState_t audio_getState(){
	return audioState;
}

bool audio_getTag(fs::File* file){
/*
	char *name = (char*)calloc(128, sizeof(char));
	*name = '/';
	strcat(name, file->name());

	audioFile->open(name);
*/

	audioFile->open(file->path());
//	audioFile->open(file->name());
	/*	if(audioTags == NULL){
		audioTags = new AudioFileSourceID3(audioFile);
	} else {
		*audioTags = AudioFileSourceID3(audioFile);
	}
	*/
	*audioTags = AudioFileSourceID3(audioFile);
	audioTags->RegisterMetadataCB(data_getFileTags, (void*)audioFile);
//	log_d("beginning player ; Free heap: %d", ESP.getFreeHeap());
	playing = player->begin(audioTags, audioOutput);
//	log_d("player beginned ;  Free heap: %d", ESP.getFreeHeap());
	if(playing){
		audioState = AUDIO_PARSING_TAGS;
	} else {
		audioState = AUDIO_IDLE;
	}
	return playing;

//	audioFile->close();
}
