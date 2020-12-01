// #include "audio.h"
#include "esPod.h"

AudioFileSourceID3 *audioTags;
AudioFileSourceFS *audioFile;
AudioGenerator *player;
AudioOutputI2S *audioOutput;

const uint8_t I2S_CLK = 16;
const uint8_t I2S_DATA = 21;
const uint8_t I2S_WS = 22;

void audio_init(){
	audioOutput = new AudioOutputI2S();
	audioOutput->SetPinout(I2S_CLK, I2S_WS, I2S_DATA);
	audioFile = new AudioFileSourceFS(SD_MMC);
	audioTags = new AudioFileSourceID3(audioFile);
//	player = new AudioGeneratorMP3();
}

void audio_playTrack(MenuSong *track){
	// Attach buttons callbacks
	io_deattachAllCB();
	io_attachCBRight(audio_nextTrack);
	io_attachCBLeft(audio_prevTrack);
	io_attachCBDown(audio_pause);
	io_attachCBUp(menu_update);
	// Display track infos
	display_makePlayer(track->getArtist(), track->getAlbum(), track->getName(), track->getTrack());

	// Play track
	// Update time every seconds
	// 

}

void audio_nextTrack(){

}

void audio_prevTrack(){

}

void audio_pause(){

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
