#ifndef SBURB_FONT_ENGINE_H
#define SBURB_FONT_ENGINE_H

#include "Common.h"
#include <SFML/Graphics/Drawable.hpp>

namespace SBURB {
    class FontEngine : public sf::Drawable {
    public:
        FontEngine();

        int GetStart() { return this->start; };
        int GetEnd() { return this->end; };

        void SetText(std::string text) { this->text = text; };

        bool NextBatch();
        bool OnLastBatch();

        bool IsShowingAll();
        void ShowSubText(int x, int y);
        void ShowAll();

        void SetFormatted(bool formatted) { this->formatted = formatted;  };

        int GetWidth() { return this->width; };
        int GetHeight() { return this->height; };
        void SetDimensions(int x, int y, int width, int height) { this->x = x; this->y = y; this->width = width; this->height = height; };

        int GetX() { return this->x; };
        void SetX(int x) { this->x = x; };

        int GetY() { return this->y; };
        void SetY(int y) { this->y = y; };

    private:
        int start;
        int end;
        std::string text;
        bool formatted;
        int width;
        int height;
        int x;
        int y;

    };
}

#endif