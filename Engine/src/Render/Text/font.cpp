#include "Engine/Graphics/Text/font.h"

#include "Engine/debug.h"
#include "Engine/file.h"


Font::Font(const std::string& filepath) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        debug::error("FREETYPE: Could not init FreeType Library");
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, file::getPath(filepath).c_str(), 0, &face)) {
        debug::error("FREETYPE: Failed to load font: {}", file::getPath(filepath));
        FT_Done_FreeType(ft);
        return;
    }

    // Set pixel size (48 is the base size, will be scaled during rendering)
    FT_Set_Pixel_Sizes(face, 0, BASE_FONT_SIZE);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 ASCII characters
    for (unsigned char c = 0; c < 128; c++) {
        createGlyphTexture(c, face);
    }

    // Restore default alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}


void Font::createGlyphTexture(unsigned char c, FT_Face& face) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        debug::error("FREETYPE: Failed to load glyph for character: {}", c);
        return;
    }

    // Check if the glyph has bitmap data
    if (face->glyph->bitmap.width == 0 || face->glyph->bitmap.rows == 0) {
        // Some characters like space have no visual representation
        if (c == ' ') {
            // Store a character with no texture but with proper advance
            Character character = {
                0,  // No texture
                glm::ivec2(0, 0),
                glm::ivec2(0, 0),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            characters_.insert(std::pair<char, Character>(c, character));
        }
        return;
    }

    // Generate texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R8,  // Use GL_R8 for internal format (more explicit)
        face->glyph->bitmap.width,
        face->glyph->bitmap.rows,
        0,
        GL_RED,  // Source format
        GL_UNSIGNED_BYTE,
        face->glyph->bitmap.buffer
    );

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Ensure texture is complete (no mipmaps needed)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // Check for GL errors during texture creation
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        debug::error("GL error creating texture for: {}", c);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    // Store character
    Character character = {
        texture, 
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        static_cast<unsigned int>(face->glyph->advance.x)
    };
    characters_.insert(std::pair<char, Character>(c, character));
}

void Font::unloadFont() {
    for (auto& pair : characters_) {
        glDeleteTextures(1, &pair.second.textureID);
    }
}

Character& Font::getCharacter(char c) {
    return characters_[c];
}

glm::vec2 Font::getTextSize(const std::string& text, float fontSize) const {
    // Convert font size to scale factor (base size is 48)
    float scale = fontSize / BASE_FONT_SIZE;

    float width = 0.0f;
    float maxHeight = 0.0f;

    // Iterate through all characters to calculate total width and max height
    for (char c : text) {
        auto it = characters_.find(c);
        if (it == characters_.end()) {
            continue;  // Skip characters that aren't loaded
        }

        const Character& ch = it->second;

        // Add the advance to the width (advance is in 1/64 pixels, so divide by 64)
        width += (ch.advance >> 6) * scale;

        // Track the maximum height (considering bearing and size)
        float charHeight = ch.size.y * scale;
        if (charHeight > maxHeight) {
            maxHeight = charHeight;
        }
    }

    return glm::vec2(width, maxHeight);
}

