#ifndef _GEF_AUDIO_MANAGER_H
#define _GEF_AUDIO_MANAGER_H

#include <gef.h>
#include <string>

namespace gef
{
	struct SoundBufferID {
		size_t val_ = 0;
	};

	struct PlayingSoundID {
		size_t val_ = 0;
	};

	class PlayingSound {
	public:
		virtual void Remove() = 0;
		virtual bool GetDeleteAutomatically() const = 0;
		virtual void SetDeleteAutomatically(bool) = 0;
		virtual bool GetLooping() const = 0;
		virtual void SetLooping(bool) = 0;
		virtual bool GetPlaying() const = 0;
		virtual void SetPlaying(bool) = 0;
		virtual float GetPitch() const = 0;
		virtual void SetPitch(float) = 0;
		virtual float GetVolume() const = 0;
		virtual void SetVolume(float) = 0;
		virtual ~PlayingSound() {};
	};

	class Platform;

	class AudioManager
	{
	public:
		virtual ~AudioManager();

		virtual SoundBufferID LoadSample(const std::string& file, const Platform& platform) = 0;
		virtual PlayingSoundID CreateSound(const SoundBufferID sound_buffer_index, const bool delete_automatically = true, const bool looping = false) = 0;
		virtual bool LoadMusic(const std::string& file, const Platform& platform) = 0;
		virtual PlayingSound* GetSound(const PlayingSoundID key) = 0;
		virtual PlayingSound* GetMusic() = 0;
		virtual void SetMasterVolume(float volume) = 0;
		virtual void Update() = 0;
		static AudioManager* Create();
	protected:
		AudioManager();
	};
}

#endif // _GEF_AUDIO_MANAGER_H
