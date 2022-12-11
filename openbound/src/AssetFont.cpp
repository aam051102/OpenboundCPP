#include "AssetFont.h"
#include "Sburb.h"
#include "Logger.h"

namespace SBURB {
    AssetFont::AssetFont(std::string name, std::vector<std::string> sources) {
        this->type = "font";
        this->name = name;
        this->sources = sources;
        this->asset = std::make_shared<sf::Font>();
        this->style = sf::Text::Style::Regular;

        for (int i = 0; i < sources.size(); i++) {
            auto values = split(sources[i], ":", 1);
            auto type = trim(values[0]);
            auto path = trim(values[1]);

            if (type == "url") {
                auto extension = path.substr(path.find_last_of(".") + 1, path.size() - (path.find_last_of(".") + 1));
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
                        GlobalLogger->Log(Logger::Error, "Font does not exist.");
                        return;
                    }
                }
            }
            else if (type == "local") {
                // NOTE: Probably not possible anymore. Unknown.
                //ret.sources.push("local('" + path + "')");
            }
            else if (type == "weight") {
                if (path == "bold") {
                    this->style = sf::Text::Style::Bold;
                }
            }
        }
    }
}