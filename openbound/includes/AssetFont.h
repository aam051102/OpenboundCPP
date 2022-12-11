#ifndef SBURB_ASSET_FONT_H
#define SBURB_ASSET_FONT_H

#include "Common.h"
#include "Asset.h"

namespace SBURB
{
    class AssetFont : public Asset
    {
    public:
        AssetFont(std::string name, std::vector<std::string> sources);

        std::shared_ptr<sf::Font> GetAsset() { return this->asset; };

        std::vector<std::string> GetSources() { return this->sources; };

        sf::Text::Style GetStyle() { return this->style; };

    private:
        std::vector<std::string> sources;
        std::shared_ptr<sf::Font> asset;
        sf::Text::Style style;

    };
}

#endif