#ifndef SBURB_RICH_TEXT_H
#define SBURB_RICH_TEXT_H

#include <string>
#include "Font.h"

namespace SBURB
{
    class RichText : public sf::Drawable, sf::Transformable
    {
    public:
        RichText();

        void Update(float delta);
        void AddColor(std::string name, int32_t color);

        Font *font;
        std::string rawText;
        sf::Vector2f renderPosition;
        short monospacing;
        float scale;

        char textTypeFlags;
        bool ignoreTags;

    private:
        sf::Vector2f renderOffset;

        std::unordered_map<std::string, int32_t> colorPresets;

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    };
}
#endif