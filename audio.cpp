#include "audio.h"

extern AudioFileSourceID3 *audioTags;
extern AudioFileSourceFS *audioFile;
extern AudioGenerator *player;
extern AudioOutputI2S *audioOutput;

void audio_tagCB(void *cbData, const char *type, bool isUnicode, const char *string)
{
	(void)cbData;
	audioLogger->printf("ID3 callback for: %s = '", type);

	if (isUnicode) {
 	   string += 2;
	}
  
	while (*string) {
		char a = *(string++);
		if (isUnicode) {
			string++;
	    }
	    audioLogger->printf("%c", a);
	}
	audioLogger->printf("'\n");
}

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
