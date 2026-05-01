#pragma once

#include <raudio.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <glm/vec3.hpp>

/** Example usage:
    * gl::AudioEngine::initialize(); // Call once at application start
    *
    * gl::AudioEngine::loadSound("path/to/sound.wav", "explosion");
    * gl::AudioEngine::playSound("explosion", 0.8f, 1.0f, 0.5f);
    *
    * gl::AudioEngine::loadMusic("path/to/music.mp3", "background", true);
    * gl::AudioEngine::playMusic("background");
    *
    * // In the main update loop
    * gl::AudioEngine::update();
    *
    * // On application shutdown
    * gl::AudioEngine::shutdown();
    */

namespace gl {

    class AudioEngine {
    public:
        static void initialize();
        static void shutdown();
        static bool isReady();

        // Sound loading
        static void loadSound(const std::string& path, const std::string& name);
        static void unloadSound(const std::string& name);

        // Music loading
        static void loadMusic(const std::string& path, const std::string& name, bool looping = true);
        static void unloadMusic(const std::string& name);

        // Sound playback
        static void playSound(const std::string& name, float volume = 1.0f, float pitch = 1.0f, float pan = 0.5f);
        static void playSound3D(const std::string& name, const glm::vec3& worldPos, float pitch = 1.0f, float volume = 1.0f);

        // Music playback
        static void playMusic(const std::string& name);
        static void pauseMusic(const std::string& name);
        static void resumeMusic(const std::string& name);
        static void stopMusic(const std::string& name);
        static void seekMusic(const std::string& name, float seconds);

        // Sound controls
        static void setSoundVolume(const std::string& name, float volume);
        static void setSoundPitch(const std::string& name, float pitch);
        static void setSoundPan(const std::string& name, float pan);

        // Music controls
        static void setMusicVolume(const std::string& name, float volume);
        static void setMusicPitch(const std::string& name, float pitch);
        static void setMusicPan(const std::string& name, float pan);

        // Global controls
        static void setMasterVolume(float volume);
        static float getMasterVolume();

        // 3D audio
        static void setListener(const glm::vec3& worldPos);
        static void setMaxDistance(float meters);

        // Update (call once per frame)
        static void update();

    private:
        struct SoundEntry {
            Sound sound;
        };

        struct MusicEntry {
            Music music;
        };

        struct ActiveAlias {
            Sound alias;
            glm::vec3 position;
            bool is_spatial;
            float base_volume = 1.0f;
        };

        static void updateSpatialAudio(ActiveAlias& alias);
        static bool isMusicPlaying(const MusicEntry& entry);

        static std::unordered_map<std::string, SoundEntry> sounds_;
        static std::unordered_map<std::string, MusicEntry> musics_;
        static std::vector<ActiveAlias> active_sounds_;

        static glm::vec3 listener_position_;
        static float max_distance_;
        static bool initialized_;
    };

} // namespace gl

