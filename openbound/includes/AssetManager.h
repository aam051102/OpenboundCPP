#ifndef SBURB_ASSET_MANAGER_H
#define SBURB_ASSET_MANAGER_H

#include <pugixml.hpp>
#include "Common.h"
#include "Sprite.h"

namespace SBURB
{
    class AssetManager
    {
    public:
        AssetManager();
        ~AssetManager();

        std::string GetAsset(std::string name);
        void LoadAsset(AssetTexture name);
        bool CheckIsLoaded(std::string name);
        bool FinishedLoading();
        
        void SetDescription(std::string description) { this->description = description;  };

    protected:
        std::string description;

    };
}
#endif
