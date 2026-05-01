#include "Engine/Audio/audio-engine.h"

#include <algorithm>
#include <stdexcept>
#include <glm/glm.hpp>

#include "Engine/debug.h"

namespace gl {

    // Static member initialization
    std::unordered_map<std::string, AudioEngine::SoundEntry> AudioEngine::sounds_;
    std::unordered_map<std::string, AudioEngine::MusicEntry> AudioEngine::musics_;
    std::vector<AudioEngine::ActiveAlias> AudioEngine::active_sounds_;
    glm::vec3 AudioEngine::listener_position_ = glm::vec3(0.0f);
    float AudioEngine::max_distance_ = 25.0f;
    bool AudioEngine::initialized_ = false;

    void AudioEngine::initialize() {
        if (initialized_) {
            debug::warn("AudioEngine already initialized");
            return;
        }

        InitAudioDevice();
        std::cout << std::endl;
        if (!IsAudioDeviceReady()) {
            debug::error("Failed to initialize audio device");
            throw std::runtime_error("Audio device initialization failed");
        }

        initialized_ = true;
        debug::info("AudioEngine initialized");
    }

    void AudioEngine::shutdown() {
        if (!initialized_) {
            return;
        }

        // Unload all active sound aliases
        for (auto& alias : active_sounds_) {
            UnloadSoundAlias(alias.alias);
        }

        // Unload all sounds
        for (auto& [name, entry] : sounds_) {
            UnloadSound(entry.sound);
        }

        // Unload all music
        for (auto& [name, entry] : musics_) {
            UnloadMusicStream(entry.music);
        }

        CloseAudioDevice();

        active_sounds_.clear();
        sounds_.clear();
        musics_.clear();
        initialized_ = false;

        debug::info("AudioEngine shutdown");
    }

    bool AudioEngine::isReady() {
        return initialized_;
    }

    void AudioEngine::loadSound(const std::string& path, const std::string& name) {
        unloadSound(name);

        Sound sound = LoadSound(path.c_str());
        if (!IsSoundReady(sound)) {
            throw std::runtime_error("Failed to load sound: " + path);
        }

        sounds_[name] = {sound};
        debug::print("Loaded sound: %s", name.c_str());
    }

    void AudioEngine::unloadSound(const std::string& name) {
        auto it = sounds_.find(name);
        if (it != sounds_.end()) {
            UnloadSound(it->second.sound);
            sounds_.erase(it);
            debug::print("Unloaded sound: %s", name.c_str());
        }
    }

    void AudioEngine::loadMusic(const std::string& path, const std::string& name, bool looping) {
        unloadMusic(name);

        Music music = LoadMusicStream(path.c_str());
        if (!IsMusicReady(music)) {
            debug::error("Failed to load music: %s", path.c_str());
            throw std::runtime_error("Failed to load music: " + path);
        }

        music.looping = looping;
        musics_[name] = {music};
        debug::print("Loaded music: %s (looping: %s)", name.c_str(), looping ? "true" : "false");
    }

    void AudioEngine::unloadMusic(const std::string& name) {
        auto it = musics_.find(name);
        if (it != musics_.end()) {
            UnloadMusicStream(it->second.music);
            musics_.erase(it);
            debug::print("Unloaded music: %s", name.c_str());
        }
    }

    void AudioEngine::playSound(const std::string& name, float volume, float pitch, float pan) {
        auto it = sounds_.find(name);
        if (it == sounds_.end()) {
            debug::warn("Sound not found: %s", name.c_str());
            return;
        }

        Sound alias = LoadSoundAlias(it->second.sound);
        SetSoundVolume(alias, volume);
        SetSoundPitch(alias, pitch);
        SetSoundPan(alias, pan);
        PlaySound(alias);

        active_sounds_.push_back({alias, glm::vec3(0.0f), false});
    }

    void AudioEngine::playSound3D(const std::string& name, const glm::vec3& worldPos, float pitch, float volume) {
        auto it = sounds_.find(name);
        if (it == sounds_.end()) {
            debug::warn("Sound not found: %s", name.c_str());
            return;
        }

        Sound alias = LoadSoundAlias(it->second.sound);
        SetSoundPitch(alias, pitch);

        active_sounds_.push_back({alias, worldPos, true, volume});
        updateSpatialAudio(active_sounds_.back());

        PlaySound(alias);
    }

    void AudioEngine::playMusic(const std::string& name) {
        auto it = musics_.find(name);
        if (it != musics_.end()) {
            PlayMusicStream(it->second.music);
        } else {
            debug::warn("Music not found: %s", name.c_str());
        }
    }

    void AudioEngine::pauseMusic(const std::string& name) {
        auto it = musics_.find(name);
        if (it != musics_.end()) {
            PauseMusicStream(it->second.music);
        } else {
            debug::warn("Music not found: %s", name.c_str());
        }
    }

    void AudioEngine::resumeMusic(const std::string& name) {
        auto it = musics_.find(name);
        if (it != musics_.end()) {
            ResumeMusicStream(it->second.music);
        } else {
            debug::warn("Music not found: %s", name.c_str());
        }
    }

    void AudioEngine::stopMusic(const std::string& name) {
        auto it = musics_.find(name);
        if (it != musics_.end()) {
            StopMusicStream(it->second.music);
        } else {
            debug::warn("Music not found: %s", name.c_str());
        }
    }

    void AudioEngine::seekMusic(const std::string& name, float seconds) {
        auto it = musics_.find(name);
        if (it != musics_.end()) {
            SeekMusicStream(it->second.music, seconds);
        } else {
            debug::warn("Music not found: %s", name.c_str());
        }
    }

    void AudioEngine::setSoundVolume(const std::string& name, float volume) {
        auto it = sounds_.find(name);
        if (it != sounds_.end()) {
            SetSoundVolume(it->second.sound, volume);
        }
    }

    void AudioEngine::setSoundPitch(const std::string& name, float pitch) {
        auto it = sounds_.find(name);
        if (it != sounds_.end()) {
            SetSoundPitch(it->second.sound, pitch);
        }
    }

    void AudioEngine::setSoundPan(const std::string& name, float pan) {
        auto it = sounds_.find(name);
        if (it != sounds_.end()) {
            SetSoundPan(it->second.sound, pan);
        }
    }

    void AudioEngine::setMusicVolume(const std::string& name, float volume) {
        auto it = musics_.find(name);
        if (it != musics_.end()) {
            SetMusicVolume(it->second.music, volume);
        }
    }

    void AudioEngine::setMusicPitch(const std::string& name, float pitch) {
        auto it = musics_.find(name);
        if (it != musics_.end()) {
            SetMusicPitch(it->second.music, pitch);
        }
    }

    void AudioEngine::setMusicPan(const std::string& name, float pan) {
        auto it = musics_.find(name);
        if (it != musics_.end()) {
            SetMusicPan(it->second.music, pan);
        }
    }

    void AudioEngine::setMasterVolume(float volume) {
        SetMasterVolume(volume);
    }

    float AudioEngine::getMasterVolume() {
        return GetMasterVolume();
    }

    void AudioEngine::setListener(const glm::vec3& worldPos) {
        listener_position_ = worldPos;
    }

    void AudioEngine::setMaxDistance(float meters) {
        max_distance_ = std::max(0.1f, meters);
    }

    void AudioEngine::update() {
        // Clean up finished sound aliases
        for (size_t i = 0; i < active_sounds_.size();) {
            auto& alias = active_sounds_[i];

            if (!IsSoundPlaying(alias.alias)) {
                // Sound finished, unload and remove
                UnloadSoundAlias(alias.alias);
                active_sounds_[i] = active_sounds_.back();
                active_sounds_.pop_back();
                continue;
            }

            // Update spatial audio if needed
            if (alias.is_spatial) {
                updateSpatialAudio(alias);
            }

            ++i;
        }

        // Update all music streams
        for (auto& [name, entry] : musics_) {
            UpdateMusicStream(entry.music);

            // Restart looping music if it finished
            if (!isMusicPlaying(entry) && entry.music.looping) {
                SeekMusicStream(entry.music, 0);
                PlayMusicStream(entry.music);
            }
        }
    }

    void AudioEngine::updateSpatialAudio(ActiveAlias& alias) {
        glm::vec3 to_source = alias.position - listener_position_;
        float distance = glm::length(to_source);

        // Calculate volume based on distance
        float distance_volume = std::clamp(1.0f - distance / max_distance_, 0.0f, 1.0f);
        float volume = alias.base_volume * distance_volume;

        // Calculate pan based on left-right position
        float pan = 0.5f; // Center
        if (distance > 0.0001f) {
            float x = to_source.x / distance; // -1 to +1
            pan = std::clamp(0.5f + 0.5f * x, 0.0f, 1.0f);
        }

        SetSoundVolume(alias.alias, volume);
        SetSoundPan(alias.alias, pan);
    }

    bool AudioEngine::isMusicPlaying(const MusicEntry& entry) {
        return IsMusicStreamPlaying(entry.music);
    }

} // namespace gl
