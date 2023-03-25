#ifndef SBURB_FONT_ENGINE_H
#define SBURB_FONT_ENGINE_H

#include "Common.h"
#include <SFML/Graphics/Drawable.hpp>

namespace SBURB {
    struct FormatRange {
        int minIndex;
        int maxIndex;
        std::wstring type;
        sf::Color extra;

        FormatRange(int minIndex, int maxIndex, std::wstring type, sf::Color extra = sf::Color::Black) {
            this->minIndex = minIndex;
            this->maxIndex = maxIndex;
            this->type = type;
            this->extra = extra;
        };
    };

    class FontEngine : public sf::Drawable {
    public:
        FontEngine(std::wstring text = L"");
        ~FontEngine();

        void ParseEverything();
        void ParseText();
        void ParseFormatting();
        void ParseEscapes();
        void ParsePrefixes();
        void ParseUnderlines();
        void ParseColors();
        void ParsePrefix(std::wstring prefix);

        sf::Color PrefixColouration(std::wstring prefix);

        void AddToFormatQueue(FormatRange format);
        void RealignFormatQueue(int startPos, int shiftSize);

        int GetStart() { return this->start; };
        void SetStart(int start) { this->start = start; };

        int GetEnd() { return this->end; };

        void SetColor(sf::Color color) { this->color = color; };

        void SetText(std::wstring text);
        void SetAlign(std::wstring align) { this->align = align; };

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

        std::wstring GetFontName() const { return this->fontName; };
        int GetFontSize() const { return this->fontSize; };

        const std::wstring GetLine(int index) const { return this->lines[index]; };

    private:
        std::map<std::wstring, int32_t> prefixColours;

        std::map<char, std::map<int, bool>> escaped;
        std::wstring fontName;
        int fontSize;
        sf::Color color;
        int start;
        int end;
        std::wstring text;
        bool formatted;
        int width;
        int height;
        int x;
        int y;
        int lineHeight;
        int charWidth;
        std::wstring align;
        std::vector<std::wstring> lines;
        std::vector<FormatRange> formatQueue;

    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    };
}

#endif