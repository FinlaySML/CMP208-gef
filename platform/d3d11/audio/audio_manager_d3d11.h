#ifndef _ABFW_AUDIO_MANAGER_D3D11_H
#define _ABFW_AUDIO_MANAGER_D3D11_H

#include <audio/audio_manager.h>
#include <vector>
#include <gef.h>
#include <external/SFML/Audio.hpp>
#include <system/file.h>
#include <unordered_map>
#include <memory>

namespace gef
{
	class PlayingSoundD3D11 : public PlayingSound {
	public:
		std::unique_ptr<sf::SoundSource> sound_;
		bool delete_when_finished_;
		bool is_music_;
		bool should_delete;
		PlayingSoundD3D11(std::unique_ptr<sf::SoundSource> sound, bool delete_when_finished, bool is_music_);
		void Remove();
		bool GetDeleteWhenFinished() const override;
		void SetDeleteWhenFinished(bool) override;
		bool GetLooping() const override;
		void SetLooping(bool) override;
		bool GetPlaying() const override;
		void SetPlaying(bool) override;
		float GetPitch() const override;
		void SetPitch(float) override;
		float GetVolume() const override;
		void SetVolume(float) override;
		gef::Vector4 GetPosition() const override;
		void SetPosition(gef::Vector4) override;
		float GetAttenuation() const override;
		void SetAttenuation(float) override;
		float GetMinDistance() const override;
		void SetMinDistance(float) override;
	};


class AudioManagerD3D11 : public AudioManager
{
public:
	AudioManagerD3D11();

	SoundBufferID LoadSample(const std::string& file, const Platform& platform) override;
	PlayingSoundID CreateSound(const SoundBufferID sound_buffer_index, const bool delete_when_finished, const bool spatial) override;
	bool LoadMusic(const std::string& file, const Platform& platform) override;
	PlayingSound* GetSound(const PlayingSoundID key) override;
	PlayingSound* GetMusic() override;
	void SetMasterVolume(float volume) override;
	float GetMasterVolume() const override;
	void Update() override;
	void UpdateSpatialAudio(gef::Vector4 position, gef::Vector4 direction, gef::Vector4 up) override;

private:
	std::vector<std::unique_ptr<sf::SoundBuffer>> sampleBuffers_;
	std::unordered_map<size_t, std::unique_ptr<PlayingSoundD3D11>> samples_;
	size_t sample_key_counter;
	std::vector<UInt8> musicBuffer;
	std::unique_ptr<File> musicFile;
	std::unique_ptr<PlayingSoundD3D11> music;
};

}

#endif // _ABFW_AUDIO_MANAGER_VITA_H

