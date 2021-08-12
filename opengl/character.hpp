#include <glm/vec2.hpp>

namespace rjs {
    /**
     * From https://learnopengl.com/In-Practice/Text-Rendering
     */
    struct character {
        unsigned int textureID;  // ID handle of the glyph texture
        glm::ivec2   size;       // Size of glyph
        glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
        unsigned int advance;    // Offset to advance to next glyph
    };
}