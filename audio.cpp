// #include "audio.h"
#include "esPod.h"

AudioFileSourceID3 *audioTags;
AudioFileSourceFS *audioFile;
AudioGenerator *player;
AudioOutputI2S *audioOutput;

const uint8_t I2S_CLK = 16;
const uint8_t I2S_DATA = 21;
const uint8_t I2S_WS = 22;

MenuSong *current = NULL;

bool playing = false;

hw_timer_t *audioTimer = NULL;
// portMUX_TYPE audioTimerMux= portMUX_INITIALIZER_UNLOCKED;

// notes
// I2S has a 64 bytes buffer and uses DMA.
// player->loop() takes around 650us to run. On begin it's 11650us, and can take 2500us from time to time

volatile uint32_t audioCounter = 0;
uint16_t prevAudioCounter = 0;

void audio_init(){
	audioOutput = new AudioOutputI2S();
	audioOutput->SetPinout(I2S_CLK, I2S_WS, I2S_DATA);
	audioFile = new AudioFileSourceFS(SD_MMC);
	audioTags = new AudioFileSourceID3(audioFile);
	player = new AudioGeneratorMP3();

	audioTimer = timerBegin(0, 80, true);
//	timerAttachInterrupt(audioTimer, &audio_update, true);
	timerAttachInterrupt(audioTimer, &audio_int, true);
	timerAlarmWrite(audioTimer, 1000, true);
//	timerAlarmEnable(audioTimer);

}

void /*IRAM_ATTR*/ audio_update(){
//	portENTER_CRITICAL_ISR(&audioTimerMux);
	if(!playing) return;
//	uint32_t counter = audioCounter;
	player->loop();
	uint16_t counter = audioCounter / 1000;
	if(counter != prevAudioCounter){
		prevAudioCounter = counter;
//		display_playerProgress(counter, 300);
	}
//	counter = audioCounter - counter;
//	Serial.printf("%i\n", counter);
//	Serial.println("tick");
//	portEXIT_CRITICAL_ISR(&audioTimerMux);
}

void audio_int(){
	audioCounter++;
}

void audio_playTrack(MenuSong *track){
	// Attach buttons callbacks
	io_deattachAllCB();
	io_attachCBRight(audio_nextTrack);
	io_attachCBLeft(audio_prevTrack);
	io_attachCBDown(audio_pause);
	io_attachCBUp(menu_update);
	// set track infos
	if(current != track){
		current = track;

		audioFile->close();
		audioFile->open(current->getFilename());
		// todo : stopping and changing file make the player reboot.
//		player->stop();
//		player = new AudioGeneratorMP3();
		*audioTags = AudioFileSourceID3(audioFile, true);
		player->begin(audioTags, audioOutput);
		playing = true;
		audioCounter = 0;
		timerAlarmEnable(audioTimer);

	}

	// Display track infos
//	display_makePlayer(current->getArtist(), current->getAlbum(), current->getName(), current->getTrack());
//	display_playerProgress(0, 134);
	// Play track
	// Update time every seconds
	// 

}

void audio_nextTrack(){

}

void audio_prevTrack(){

}

void audio_pause(){
	if(playing){
		playing = false;
//		audioOutput->SetGain(0);
		timerAlarmDisable(audioTimer);
		audioOutput->stop();
	} else {
		playing = true;
//		audioOutput->SetGain(1);
		audioOutput->begin();
		timerAlarmEnable(audioTimer);
	}
}


bool audio_getTag(fs::File* file){
	String name = file->name();
	if(!(name.endsWith(".mp3"))){
	audioLogger->println("unsupported file");
		return 1;
	}
	audioFile->open(file->name());
	/*	if(audioTags == NULL){
		audioTags = new AudioFileSourceID3(audioFile);
	} else {
		*audioTags = AudioFileSourceID3(audioFile);
	}
	*/
	*audioTags = AudioFileSourceID3(audioFile);
	audioTags->RegisterMetadataCB(data_getFileTags, (void*)audioFile);
	player->begin(audioTags, audioOutput);
	return 0;

//	audioFile->close();
}
