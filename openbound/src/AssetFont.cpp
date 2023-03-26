#include "AssetFont.h"
#include "Sburb.h"
#include "Logger.h"

namespace SBURB {
    AssetFont::AssetFont(std::wstring name, std::vector<std::wstring> sources) {
        this->type = L"font";
        this->name = name;
        this->sources = sources;
        this->asset = std::make_shared<sf::Font>();
        this->style = sf::Text::Style::Regular;

        for (int i = 0; i < sources.size(); i++) {
            auto values = split(sources[i], L":", 1);
            auto type = trim(values[0]);
            auto path = trim(values[1]);

            if (type == L"url") {
                auto extension = path.substr(path.find_last_of(L".") + 1, path.size() - (path.find_last_of(L".") + 1));
                auto format = "";

                if (extension == L"ttf" || extension == L"otf" || extension == L"ttc") {
                    format = "truetype";
                }
                else if (extension == L"woff") {
                    format = "woff";
                }
                else if (extension == L"svg") {
                    format = "svg";
                }

                if (format == "truetype" || format == "woff") {
                    // NOTE: Unsure if WOFF is supported
                    const auto resolvedPath = Sburb::ResolvePath(path);
                    if (!this->asset->loadFromFile(std::string(resolvedPath.begin(), resolvedPath.end()))) {
                        GlobalLogger->Log(Logger::Error, "Font does not exist.");
                        return;
                    }
                }
            }
            else if (type == L"local") {
                // NOTE: Is possible. Consider implementation.
                //ret.sources.push(L"local('" + path + "')");
            }
            else if (type == L"weight") {
                if (path == L"bold") {
                    this->style = sf::Text::Style::Bold;
                }
            }
        }
    }
}