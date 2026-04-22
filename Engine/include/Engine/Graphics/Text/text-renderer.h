#pragma once

#include "Engine/Graphics/Text/font.h"

namespace gl {

    enum class TextAlign {
        LEFT,
        CENTER,
        RIGHT
    };
    class TextRenderer {
    public:
        static void drawText(const std::string& text, glm::vec2 pos, float fontSize,
            TextAlign align = TextAlign::LEFT, const std::string& font_name = "default");
        static void loadFont(const std::string& name, const char* filepath);
        static void initialize();
        static void cleanup();

    private:
        static void ensureInitialized();

        static std::unordered_map<std::string, Font> fonts_;
        static unsigned int vao_;
        static unsigned int vbo_;
        static bool initialized_;
    };
}
