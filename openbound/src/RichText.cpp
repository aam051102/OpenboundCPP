#include "RichText.h"
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <cmath>
#include <iostream>

namespace SBURB
{
    RichText::RichText()
    {
        this->font = NULL;
        this->rawText = "";
        this->renderPosition = {0, 0};
        this->monospacing = -1;
        this->renderOffset = {0, 0};
        this->ignoreTags = false;
        this->scale = 1.0;

        this->colorPresets = std::unordered_map<std::string, int32_t>();

        AddColor("aa", 0xa10000);
        AddColor("aradia", 0xa10000);
        AddColor("ac", 0x416600);
        AddColor("nepeta", 0x416600);
        AddColor("ag", 0x005682);
        AddColor("vriska", 0x005682);
        AddColor("at", 0xa15000);
        AddColor("tavros", 0xa15000);
        AddColor("ca", 0x6a006a);
        AddColor("eridan", 0x6a006a);
        AddColor("cc", 0x77003c);
        AddColor("feferi", 0x77003c);
        AddColor("cg", 0x626262);
        AddColor("karkat", 0x626262);
        AddColor("ct", 0x000056);
        AddColor("equius", 0x000056);
        AddColor("ga", 0x008141);
        AddColor("kanaya", 0x008141);
        AddColor("gc", 0x008282);
        AddColor("terezi", 0x008282);
        AddColor("ta", 0xa1a100);
        AddColor("sollux", 0xa1a100);
        AddColor("dave", 0xe00707);
        AddColor("meenah", 0x77003c);
        AddColor("rose", 0xb536da);
        AddColor("aranea", 0x005682);
        AddColor("kankri", 0xff0000);
        AddColor("porrum", 0x008141);
        AddColor("latula", 0x008282);
    }

    void RichText::AddColor(std::string name, int32_t color)
    {
        colorPresets.insert(std::pair<std::string, int32_t>(name, color));
    }

    void RichText::Update(float delta)
    {
        
    }

    void RichText::draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        states.transform *= getTransform();

        int x = renderPosition.x, y = renderPosition.y;

        std::vector<int32_t> colorStack;
        sf::Color formatColor = sf::Color(255, 255, 255);

        bool cancelNext = false;

        for (int i = 0; i < rawText.size(); i++)
        {
            bool verifiedTag = false;

            switch (rawText.at(i))
            {
            case '\n':
                y += (font->GetGlyph('A').texture.height + font->GetGlyph('A').offset + (monospacing == -1 ? 5 : monospacing * 2)) * scale;
                x = renderPosition.x;
                verifiedTag = true;
                break;

            case '\\':
                if (!ignoreTags)
                {
                    if ((size_t)i + 1 < rawText.size())
                    {
                        if (rawText[(size_t)i + 1] == 'i')
                        {
                            y += (font->GetGlyph('A').texture.height + font->GetGlyph('A').offset) * scale;

                            if (monospacing == -1)
                            {
                                x = renderPosition.x + (font->GetGlyph('*').shift + font->GetGlyph(' ').shift) * scale;
                            }
                            else
                            {
                                x = renderPosition.x + (font->GetGlyph('*').texture.width + font->GetGlyph(' ').texture.width + monospacing * 2) * scale;
                            }
                            i += 1;
                            verifiedTag = true;
                            break;
                        }
                    }

                    cancelNext = true;
                    verifiedTag = true;
                }

                break;

            default:
                break;
            }

            if (verifiedTag)
            {
                continue;
            }

            if (i < rawText.length() && font->HasGlyph(rawText.at(i)))
            {
                sf::Vector2f localRenderOffset = {0, 0};

                auto glyph = font->GetGlyph(rawText.at(i));
                auto sprite = font->GetGlyphSprite(rawText.at(i));
                sprite.setPosition(x + localRenderOffset.x, y + localRenderOffset.y);
                sprite.setScale({scale, scale});
                sprite.color = formatColor;

                if (monospacing == -1)
                {
                    x += (glyph.shift) * scale;
                }
                else
                {
                    x += (glyph.texture.width + monospacing) * scale;
                }

                target.draw(sprite, states);
            }
        }
    }
}