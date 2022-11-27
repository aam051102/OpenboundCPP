#include "AssetManager.h"

namespace SBURB {
    AssetManager::AssetManager() {
        
    }

    AssetManager::~AssetManager() {

    }

    bool AssetManager::CheckIsLoaded(std::string name) {
        return false;
    }

    bool AssetManager::FinishedLoading() {
        return false;
    }

    std::string AssetManager::GetAsset(std::string name) {
        return "";
    }

    void AssetManager::LoadAsset(AssetTexture asset) {
        
    }
}