#ifndef SBURB_ASSET_FONT_H
#define SBURB_ASSET_FONT_H

#include "Common.h"
#include "Asset.h"

namespace SBURB
{
    class AssetFont : public Asset
    {
    public:
        AssetFont(std::string name, std::vector<std::string> sources) {
            this->type = "font";
            this->name = name;
            this->sources = sources;
            this->asset = std::make_shared<sf::Font>();

            for (int i = 0; i < sources.size(); i++) {
                auto values = split(sources[i], ":");
                auto type = trim(values[0]);
                auto path = trim(values[1]);

                if (type == "url") {
                    auto extension = path.substr(path.find(".") + 1, path.size() - (path.find(".") + 1));
                    auto format = "";

                    if (extension == "ttf") {
                        format = "truetype";
                    }
                    else if (extension == "woff") {
                        format = "woff";
                    }
                    else if (extension == "svg") {
                        format = "svg";
                    }

                    if (format == "truetype" || format == "woff") {
                        // NOTE: UNSURE IF WOFF IS SUPPORTED?????
                        this->asset->loadFromFile(Sburb::ResolvePath(path));
                    }
                }
                else if (type == "local") {
                    // NOTE: Probably not possible anymore. Unknown.
                    //ret.sources.push("local('" + path + "')");
                }
                else if (type == "weight") {
                    // Probably find some way to transfer this?
                    //ret.extra += "font-weight:" + path + "; "
                }
            }
        };

        std::shared_ptr<sf::Font> GetAsset() { return this->asset; };

        std::vector<std::string> GetSources() { return this->sources; };

    private:
        std::vector<std::string> sources;
        std::shared_ptr<sf::Font> asset;

    };
}

#endif