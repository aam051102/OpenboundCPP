#include "AssetFont.h"
#include "Sburb.h"
#include "Logger.h"

namespace SBURB {
    AssetFont::AssetFont(std::string name, std::vector<std::string> sources) {
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

                    if (!this->asset->loadFromFile(Sburb::ResolvePath(path))) {
                        GlobalLogger->Log(Logger::Error, "Failed to create main game window.");
                        return;
                    }
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
    }
}