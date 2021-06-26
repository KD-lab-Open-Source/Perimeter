#ifndef __SOUND_TRACK_H__
#define __SOUND_TRACK_H__

class SoundTrack
{
public:
	SoundTrack();

	bool empty() const { return fileNames.empty(); }

	const char* fileName() const;

#include "SoundTrack-1251.inl"

private:
	std::vector<std::string> fileNames;
	std::string fileName_;
	bool randomChoice;
	mutable int index;
};

typedef SoundTrack SoundTracks[3];

#endif //__SOUND_TRACK_H__