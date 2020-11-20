#include "audio.h"

extern AudioFileSourceID3 *audioTags;
extern AudioFileSourceFS *audioFile;
extern AudioGenerator *player;
extern AudioOutputI2S *audioOutput;

bool audio_getTag(fs::File* file){
	String name = file->name();
	if(!(name.endsWith(".mp3"))){
	audioLogger->println("unsuported file");
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
