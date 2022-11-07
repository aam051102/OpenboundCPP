#ifndef SBURB_TEXTURE_H
#define SBURB_TEXTURE_H

#include <stdint.h>
#include <vector>

namespace SBURB
{
    struct TextureEntry
    {
        sf::Vector2f pos;
        sf::Vector2f size;
        Vector4 padding;
        uint16_t pageId;
    };
}

#endif