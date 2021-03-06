#ifndef __SOUND_TRACK_H__
#define __SOUND_TRACK_H__

class SoundTrack
{
public:
	SoundTrack();

    bool empty() const { return fileNames.empty(); }

    void clear() {
        randomChoice = false;
        index = 0;
        fileName_.clear();
        fileNames.clear();
    }

	const char* fileName() const;

    SERIALIZE(ar) {
        ar & WRAP_NAME(fileName_, "fileName");
        ar & TRANSLATE_OBJECT(fileNames, "Имена файлов");
        ar & TRANSLATE_OBJECT(randomChoice, "Случайный выбор");
        if (ar.isInput() && fileNames.empty() && !fileName_.empty()) {
            fileNames.push_back(fileName_);
        }
    }

private:
	std::vector<std::string> fileNames;
	std::string fileName_;
	bool randomChoice;
	mutable int index;
};

typedef SoundTrack SoundTracks[3];

#endif //__SOUND_TRACK_H__