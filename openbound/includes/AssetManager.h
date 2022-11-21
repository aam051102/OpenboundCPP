#ifndef SBURB_ASSET_MANAGER_H
#define SBURB_ASSET_MANAGER_H

#include <pugixml.hpp>
#include "Common.h"
#include "Asset.h"

namespace SBURB
{
    class AssetManager
    {
    public:
        AssetManager();
        ~AssetManager();

        std::string GetAsset(std::string name);
        void LoadAsset(Asset name);
        bool CheckIsLoaded(std::string name);
        
        void SetDescription(std::string description) { this->description = description;  };

    protected:
        std::string description;

    };
}
#endif
