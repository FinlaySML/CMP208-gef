#ifndef _GEF_AUDIO_MANAGER_H
#define _GEF_AUDIO_MANAGER_H

#include <gef.h>
#include <string>
#include <maths/vector4.h>

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
		virtual bool GetDeleteWhenFinished() const = 0;
		virtual void SetDeleteWhenFinished(bool) = 0;
		virtual bool GetLooping() const = 0;
		virtual void SetLooping(bool) = 0;
		virtual bool GetPlaying() const = 0;
		virtual void SetPlaying(bool) = 0;
		virtual float GetPitch() const = 0;
		virtual void SetPitch(float) = 0;
		virtual float GetVolume() const = 0;
		virtual void SetVolume(float) = 0;
		virtual gef::Vector4 GetPosition() const = 0;
		virtual void SetPosition(gef::Vector4) = 0;
		virtual float GetAttenuation() const = 0;
		virtual void SetAttenuation(float) = 0;
		virtual float GetMinDistance() const = 0;
		virtual void SetMinDistance(float) = 0;
		virtual ~PlayingSound() {};
	};

	class Platform;

	class AudioManager
	{
	public:
		virtual ~AudioManager();

		virtual SoundBufferID LoadSample(const std::string& file, const Platform& platform) = 0;
		virtual PlayingSoundID CreateSound(const SoundBufferID sound_buffer_index, const bool delete_when_finished = true, const bool spatial = false) = 0;
		virtual bool LoadMusic(const std::string& file, const Platform& platform) = 0;
		virtual PlayingSound* GetSound(const PlayingSoundID key) = 0;
		virtual PlayingSound* GetMusic() = 0;
		virtual void SetMasterVolume(float volume) = 0;
		virtual float GetMasterVolume() const = 0;
		virtual void Update() = 0;
		virtual void UpdateSpatialAudio(gef::Vector4 position, gef::Vector4 direction, gef::Vector4 up) = 0;
		static AudioManager* Create();
	protected:
		AudioManager();
	};
}

#endif // _GEF_AUDIO_MANAGER_H
