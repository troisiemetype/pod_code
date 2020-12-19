// #include "audio.h"
#include "esPod.h"

AudioFileSourceID3 *audioTags;
AudioFileSourceFS *audioFile;
AudioGenerator *player;
AudioOutputI2S *audioOutput;

//int8_t audioBuffer[AUDIO_BUFFER_SIZE];

const uint8_t I2S_CLK = 16;
const uint8_t I2S_DATA = 21;
const uint8_t I2S_WS = 22;

AudioTrackData *current = NULL;
MenuItem *currentItem = NULL;

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
//	player = new AudioGeneratorMP3(audioBuffer, AUDIO_BUFFER_SIZE);
	player = new AudioGeneratorMP3();

	audioTimer = timerBegin(0, 80, true);
	timerAttachInterrupt(audioTimer, &audio_int, true);
	timerAlarmWrite(audioTimer, 1000, true);
//	timerAlarmEnable(audioTimer);

}

bool audio_update(){
	if(!playing) return false;
	bool running = player->loop();
	uint16_t counter = audioCounter / 1000;
	if(counter != prevAudioCounter){
		prevAudioCounter = counter;
		display_pushPlayerProgress(counter, 300);
	}

	return running;
}

void audio_int(){
	if(playing) audioCounter++;
}

void audio_playTrack(AudioTrackData *track, MenuItem *item){
	// set track infos
	currentItem = item;
	if(current != track){
		current = track;

		playing = 0;
		audioFile->close();
		audioFile->open(current->getFilename());
		// todo : stopping and changing file make the player reboot.
//		player->stop();
//		player = new AudioGeneratorMP3();
		*audioTags = AudioFileSourceID3(audioFile, true);
		playing = player->begin(audioTags, audioOutput);
		Serial.printf("playing %s : %i\n", current->getName(), playing);
		log_d("Free heap: %d", ESP.getFreeHeap());
		if(playing){
			audioCounter = 0;
			timerAlarmEnable(audioTimer);
		}
	}

	audio_updateDisplay();

}

void audio_updateDisplay(){
	if(!current) return;

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
	display_pushPlayerProgress(audioCounter / 1000, 134);	
}

void audio_nextTrack(){
	MenuItem *item = currentItem->getNext();
	if(!item) return;

	AudioTrackData *newTrack = reinterpret_cast<AudioTrackData*>(item->getData());
	playing = false;
	audio_playTrack(newTrack, item);
}

// TODO : make the player call the menu from which the song was taken, when reaching first.
void audio_prevTrack(){
	MenuItem *item = currentItem->getPrevious();
	AudioTrackData *newTrack = current;
	if((audioCounter / 1000) >= 1){
		current = NULL;
		item = currentItem;
	} else {
		if(!item) return;

		newTrack = reinterpret_cast<AudioTrackData*>(item->getData());
	}
	playing = false;
	audio_playTrack(newTrack, item);
}

void audio_pause(){
	if(playing){
		playing = false;
//		audioOutput->SetGain(0);
//		timerAlarmDisable(audioTimer);
		audioOutput->stop();
	} else {
		playing = true;
//		audioOutput->SetGain(1);
		audioOutput->begin();
//		timerAlarmEnable(audioTimer);
	}
}

void audio_mute(){

}

void audio_unmute(){

}

bool audio_getTag(fs::File* file){
	String name = file->name();
	if(!(name.endsWith(".mp3"))){
	audioLogger->println("unsupported file");
		return 0;
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
	log_d("beginning player ; Free heap: %d", ESP.getFreeHeap());
	bool ret = player->begin(audioTags, audioOutput);
	log_d("player beginned ;  Free heap: %d", ESP.getFreeHeap());
	return ret;

//	audioFile->close();
}
