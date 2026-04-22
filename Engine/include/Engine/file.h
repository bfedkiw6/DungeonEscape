#pragma once

#include <filesystem>
#include <string>

/**
 * Utility functions for handling file paths within the project for loading resources.
 * PROJECT_ROOT is defined in CMakeLists.txt as the absolute path to the project root directory.
 */
namespace file {
    namespace fs = std::filesystem;

    /**
     * Gets the absolute path to a file given its path from the project root directory.
     * Automatically handles path separators correctly for the current OS.
     * @param path_from_root Path from project root, e.g., "Resources/Shaders/phong_vert.glsl"
     * @return Absolute filesystem path as a string (for C API compatibility)
     */
    inline std::string getPath(const std::string& path_from_root) {
        fs::path project_root(PROJECT_ROOT);
        fs::path result = (project_root / path_from_root).lexically_normal();
        return result.string();
    }

    /**
     * Convenience overload for const char*
     */
    inline std::string getPath(const char* path_from_root) {
        return getPath(std::string(path_from_root));
    }

    /**
     * Gets the directory of a file from the project root.
     * @param path_from_root Path from project root, e.g., "Resources/Models/character.fbx"
     * @return Directory path from project root, e.g., "Resources/Models"
     */
    inline std::string getDirectory(const std::string& path_from_root) {
        return fs::path(path_from_root).parent_path().string();
    }

    /**
     * Normalizes a path by removing leading "./" or ".\" symbols.
     * This is useful for handling OBJ files and other formats that prefix relative paths.
     * @param path The path to normalize, e.g., ".\\textures\\diffuse.png"
     * @return Normalized path, e.g., "textures/diffuse.png"
     */
    inline std::string normalizePath(const std::string& path) {
        if (path.empty()) {
            return path;
        }

        fs::path path_obj(path);
        std::string path_str = path_obj.generic_string();

        if (path_str.starts_with("./") || path_str.starts_with(".\\")) {  // Remove leading "./" or ".\"
            return path_str.substr(2);
        }

        return path_str;
    }
}
