#include "AssetGraphic.h"
#include "Sburb.h"

namespace SBURB
{
    AssetGraphic::AssetGraphic(std::string name, std::string path)
    {
        this->type = "graphic";
        this->name = name;
        this->path = path;
        this->count = 0;
    }

    std::shared_ptr<sf::Texture> AssetGraphic::Load()
    {
        this->count++;

        if (!this->asset)
        {
            this->asset = std::make_shared<sf::Texture>();
            this->asset->loadFromFile(Sburb::ResolvePath(path));
        }

        return this->asset;
    }

    void AssetGraphic::Unload()
    {
        this->count--;

        if (this->count == 0)
        {
            this->asset.reset();
            this->asset = nullptr;
        }
    }
}