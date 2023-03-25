#include "AssetGraphic.h"
#include "Sburb.h"

namespace SBURB {
    AssetGraphic::AssetGraphic(std::wstring name, std::wstring path) {
        this->type = L"graphic";
        this->name = name;
        this->path = path;
        this->asset = std::make_shared<sf::Texture>();
        const auto resolvedPath = Sburb::ResolvePath(path);
        this->asset->loadFromFile(std::string(resolvedPath.begin(), resolvedPath.end()));
    }
}