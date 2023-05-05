#include <platform/d3d11/audio/audio_manager_d3d11.h>

namespace gef
{
	AudioManager* AudioManager::Create()
	{
		return new AudioManagerD3D11();
	}

	PlayingSoundD3D11::PlayingSoundD3D11(std::unique_ptr<sf::SoundSource> sound, bool del_auto, bool is_music) : 
		sound_{ std::move(sound) }, 
		delete_automatically_{ del_auto }, 
		is_music_{is_music},
		should_delete{false}
	{
		
	}

	void PlayingSoundD3D11::Remove() {
		should_delete = true;
	}

	bool PlayingSoundD3D11::GetDeleteAutomatically() const {
		return delete_automatically_;
	}

	void PlayingSoundD3D11::SetDeleteAutomatically(bool val) {
		delete_automatically_ = val;
	}

	bool PlayingSoundD3D11::GetLooping() const {
		if (is_music_) return ((sf::Music*)sound_.get())->getLoop();
		else return ((sf::Sound*)sound_.get())->getLoop();
	}

	void PlayingSoundD3D11::SetLooping(bool val) {
		if(is_music_) ((sf::Music*)sound_.get())->setLoop(val);
		else ((sf::Sound*)sound_.get())->setLoop(val);
	}

	bool PlayingSoundD3D11::GetPlaying() const {
		return sound_->getStatus() == sf::SoundSource::Playing;
	}

	void PlayingSoundD3D11::SetPlaying(bool val) {
		if (val) sound_->play();
		else sound_->pause();
	}

	float PlayingSoundD3D11::GetPitch() const {
		return sound_->getPitch();
	}

	void PlayingSoundD3D11::SetPitch(float val) {
		sound_->setPitch(val);
	}

	float PlayingSoundD3D11::GetVolume() const {
		return sound_->getVolume();
	}

	void PlayingSoundD3D11::SetVolume(float val) {
		sound_->setVolume(val);
	}
	
	AudioManagerD3D11::AudioManagerD3D11() :
	sample_key_counter{0}
	{
	}

	SoundBufferID AudioManagerD3D11::LoadSample(const std::string& file, const Platform& platform)
	{
		// Create file and open the target sound file
		File* sampleFile = File::Create();
		if (sampleFile->Open(file.c_str())) {
			// Get size of the file so we can read it properly
			Int32 fileSize;
			if (sampleFile->GetSize(fileSize)) {
				// Create a buffer thats the correct size and read the data into it
				std::vector<UInt8> rawSample((size_t)fileSize);
				Int32 bytesRead = 0;
				if (sampleFile->Read(rawSample.data(), fileSize, bytesRead)) {
					// Create new sound buffer and load in the data
					std::unique_ptr<sf::SoundBuffer> newBuffer{ new sf::SoundBuffer() };
					if (newBuffer->loadFromMemory(rawSample.data(), bytesRead)) {
						// Add the new buffer to the vector
						sampleBuffers_.push_back(std::move(newBuffer));

						// Close the file and delete it
						sampleFile->Close();
						delete sampleFile;

						// Return the index of the new sounds
						return { sampleBuffers_.size() - 1 };
					}
				}
			}
			sampleFile->Close();
		}

		delete sampleFile;
		return { (size_t) - 1};
	}

	bool AudioManagerD3D11::LoadMusic(const std::string& file, const Platform & platform)
	{
		// If the music already has been loaded, unload it so it can be replaced
		musicFile.reset(nullptr);
		musicBuffer.clear();
		music.reset(nullptr);

		// Create a file and open the target music file
		musicFile.reset(gef::File::Create());
		if (musicFile->Open(file.c_str()))
		{
			// Get the file size so we can read the correct amount of data
			Int32 fileSize;
			if (musicFile->GetSize(fileSize))
			{
				// Create a buffer to store the music data
				musicBuffer.resize(fileSize);
				Int32 bytesRead = 0;
				if (musicFile->Read(musicBuffer.data(), fileSize, bytesRead))
				{
					// Pass data buffer to the music object so it can play
					std::unique_ptr<sf::Music> m{ new sf::Music() };
					if (m->openFromMemory(musicBuffer.data(), bytesRead)) {
						m->setLoop(true);
						music.reset(new PlayingSoundD3D11(std::move(m), false, true));
						return true;
					}
				}
			}
		}
		musicFile.reset(nullptr);
		musicBuffer.clear();
		music.reset(nullptr);
		return false;
	}

	PlayingSoundID AudioManagerD3D11::CreateSound(const SoundBufferID sound_buffer_index, const bool delete_automatically, const bool looping )
	{
		if (sound_buffer_index.val_ == -1) return {(size_t)-1};
		sf::Sound* s = new sf::Sound();
		s->setBuffer(*sampleBuffers_.at(sound_buffer_index.val_).get());
		s->setLoop(looping);
		s->play();
		size_t key = ++sample_key_counter;
		samples_.insert({ key, std::unique_ptr<PlayingSoundD3D11>(new PlayingSoundD3D11(std::unique_ptr<sf::SoundSource>(s), delete_automatically, false)) });
		return { key };
	}

	void AudioManagerD3D11::SetMasterVolume(float volume)
	{
		// Set the master volume control of the listener
		sf::Listener::setGlobalVolume(volume);
	}

	PlayingSound* AudioManagerD3D11::GetSound(const PlayingSoundID key) {
		auto it = samples_.find(key.val_);
		if (it == samples_.end()) return nullptr;
		return it->second.get();
	}

	PlayingSound* AudioManagerD3D11::GetMusic() {
		return music.get();
	}

	void AudioManagerD3D11::Update() {
		auto pred = [&](std::unique_ptr<PlayingSoundD3D11>& sound) {
			if (sound->should_delete) return true;
			if (sound->delete_automatically_ && !sound->GetPlaying()) return true;
			return false;
		};
		for (auto i = samples_.begin(), last = samples_.end(); i != last; ) {
			if (pred(i->second)) {
				i = samples_.erase(i);
			}
			else {
				++i;
			}
		}
	}
}