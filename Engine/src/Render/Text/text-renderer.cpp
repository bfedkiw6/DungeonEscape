#include "Engine/Graphics/Text/text-renderer.h"

#include <ranges>

#include "Engine/debug.h"
#include "Engine/Graphics/graphics.h"
#include "Engine/Graphics/shaders.h"

namespace gl {

    // Initialize static members
    std::unordered_map<std::string, Font> TextRenderer::fonts_;
    unsigned int TextRenderer::vao_ = 0;
    unsigned int TextRenderer::vbo_ = 0;
    bool TextRenderer::initialized_ = false;

    void TextRenderer::initialize() {
        if (initialized_) {
            return;
        }

        // Create and configure VAO/VBO for rendering quads
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);

        // Allocate memory for a quad (6 vertices * 4 floats per vertex)
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

        // Configure vertex attributes (position + texCoords)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

        // Unbind VAO first, THEN unbind buffer
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        initialized_ = true;
    }

    void TextRenderer::cleanup() {
        if (initialized_) {
            glDeleteVertexArrays(1, &vao_);
            glDeleteBuffers(1, &vbo_);
            initialized_ = false;
        }
        for (auto& font : fonts_ | std::views::values) {
            font.unloadFont();
        }
    }

    void TextRenderer::ensureInitialized() {
        if (!initialized_) {
            initialize();
        }
    }

    void TextRenderer::loadFont(const std::string& name, const char* filepath) {
        if (fonts_.contains(name)) {
            debug::warn("Font already exists: {}", name);
        } else {
            // Use try_emplace to construct Font in-place without moves
            fonts_.try_emplace(name, filepath);
            debug::print("Font loaded: {}", name);
        }
    }

    void TextRenderer::drawText(const std::string& text, glm::vec2 pos, float fontSize,
    TextAlign align, const std::string& font_name) {


        auto it = fonts_.find(font_name);
        if (it == fonts_.end()) {
            debug::error("Font not found: {}", font_name);
            return;
        }
        Font& font = it->second;
        float scale = fontSize / Font::BASE_FONT_SIZE;

        // Adjust starting position based on alignment
        float startX = pos.x;
        if (align != TextAlign::LEFT) {
            glm::vec2 textSize = font.getTextSize(text, fontSize);
            if (align == TextAlign::CENTER) {
                startX -= textSize.x / 2.0f;
            } else if (align == TextAlign::RIGHT) {
                startX -= textSize.x;
            }
        }



        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);  // Bind VBO AFTER VAO

        float x = startX;
        float y = pos.y;

        int charIndex = 0;

        // Render each character
        for (char c : text) {
            Character& ch = font.getCharacter(c);

            // Skip characters with no texture (like spaces)
            if (ch.textureID == 0) {
                x += (ch.advance >> 6) * scale;
                charIndex++;
                continue;
            }


            // In FreeType, bearing.y is distance from baseline to top of glyph
            // With Y increasing downward, we need to subtract bearing.y from the baseline position
            float xpos = x + ch.bearing.x * scale;
            float ypos = y - ch.bearing.y * scale;  // y is the baseline, subtract bearing to get top of glyph

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            // Update VBO for each character
            // Vertices form a quad from top-left to bottom-right
            float vertices[6][4] = {
                { xpos,     ypos,       0.0f, 0.0f },  // Top-left
                { xpos,     ypos + h,   0.0f, 1.0f },  // Bottom-left
                { xpos + w, ypos + h,   1.0f, 1.0f },  // Bottom-right

                { xpos,     ypos,       0.0f, 0.0f },  // Top-left
                { xpos + w, ypos + h,   1.0f, 1.0f },  // Bottom-right
                { xpos + w, ypos,       1.0f, 0.0f }   // Top-right
            };

            // Bind texture for this character
            glBindTexture(GL_TEXTURE_2D, ch.textureID);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Advance to next character (advance is in 1/64 pixels)
            x += (ch.advance >> 6) * scale;
            charIndex++;
        }

        // Restore state
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

} // namespace gl

