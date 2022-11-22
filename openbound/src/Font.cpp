#include "Font.h"

// TODO: Modify this slightly to use some sort of BaseSprite, which will be the original Sprite from UTEngine, instead of the current Sprite.
namespace SBURB
{
    Font::Font()
    {
        this->texId = -1;
        this->glyphs = {};
    }

    Font::Font(int texId, std::unordered_map<char, Glyph> glyphs)
    {
        this->texId = texId;
        this->glyphs = glyphs;
    }

    RenderSprite Font::GetGlyphSprite(char character)
    {
        return RenderSprite(this->texId, glyphs[character].texture);
    }

    RenderSprite Font::GetGlyphAsColor(char character, sf::Color color)
    {
        RenderSprite sprite{this->texId, glyphs[character].texture};
        sprite.color = color;
        return sprite;
    }
}