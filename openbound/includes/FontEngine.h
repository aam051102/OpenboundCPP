#ifndef SBURB_FONT_ENGINE_H
#define SBURB_FONT_ENGINE_H

#include "Common.h"
#include <SFML/Graphics/Drawable.hpp>

namespace SBURB {
    struct FormatRange {
        int minIndex;
        int maxIndex;
        std::string type;
        sf::Color extra;

        FormatRange(int minIndex, int maxIndex, std::string type, sf::Color extra = sf::Color::Black) {
            this->minIndex = minIndex;
            this->maxIndex = maxIndex;
            this->type = type;
            this->extra = extra;
        };
    };

    class FontEngine : public sf::Drawable {
    public:
        FontEngine(std::string text = "");
        ~FontEngine();

        void ParseEverything();
        void ParseText();
        void ParseFormatting();
        void ParseEscapes();
        void ParsePrefixes();
        void ParseUnderlines();
        void ParseColors();
        void ParsePrefix(std::string prefix);

        sf::Color PrefixColouration(std::string prefix);

        void AddToFormatQueue(FormatRange format);
        void RealignFormatQueue(int startPos, int shiftSize);

        int GetStart() { return this->start; };
        void SetStart(int start) { this->start = start; };

        int GetEnd() { return this->end; };

        void SetColor(sf::Color color) { this->color = color; };

        void SetText(std::string text);
        void SetAlign(std::string align) { this->align = align; };

        bool NextBatch();
        bool OnLastBatch();
        int BatchLength();

        bool IsShowingAll();
        void ShowSubText(int start, int end);
        void ShowAll();

        void SetFormatted(bool formatted) { this->formatted = formatted;  };

        int GetWidth() { return this->width; };
        int GetHeight() { return this->height; };
        void SetDimensions(int x, int y, int width, int height);

        int GetX() const { return this->x; };
        void SetX(int x) { this->x = x; };

        int GetY() const { return this->y; };
        void SetY(int y) { this->y = y; };

        int GetLineHeight() const { return this->lineHeight; };
        int GetCharWidth() const { return this->charWidth; };

        const std::string GetLine(int index) const { return this->lines[index]; };

    private:
        std::map<std::string, int32_t> prefixColours;

        std::map<char, std::map<int, bool>> escaped;
        std::string fontName;
        sf::Uint32 fontStyle;
        int fontSize;
        sf::Color color;
        int start;
        int end;
        std::string text;
        bool formatted;
        int width;
        int height;
        int x;
        int y;
        int lineHeight;
        int charWidth;
        std::string align;
        std::vector<std::string> lines;
        std::vector<FormatRange> formatQueue;

    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    };
}

#endif