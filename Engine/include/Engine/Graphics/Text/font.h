#pragma once

#include <string>
#include <unordered_map>
#include <freetype/freetype.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

/**
 * Represents a single character glyph with its texture and metrics
 */
struct Character {
    GLuint textureID;      // OpenGL texture ID for this glyph
    glm::ivec2 size;       // Size of glyph in pixels
    glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
    unsigned int advance;  // Horizontal advance to next glyph
};

/**
 * Font class loads and manages font glyphs using FreeType
 */


class Font {
public:
    Font(const std::string& filepath);

    void unloadFont();

    static constexpr int BASE_FONT_SIZE = 48;

    /**
     * Get the character glyph data for a specific character
     * @param c The character to retrieve
     * @return Reference to the Character struct containing glyph data
     */
    Character& getCharacter(char c);

    /**
     * Calculate the size (width and height) of a text string when rendered
     * @param text The text string to measure
     * @param fontSize Font size in points (e.g., 11 for small, 48 for medium, 72 for large)
     * @return A vec2 containing the width and height in pixels
     */
    glm::vec2 getTextSize(const std::string& text, float fontSize) const;

private:
    void createGlyphTexture(unsigned char c, FT_Face& face);
    std::unordered_map<char, Character> characters_;
};