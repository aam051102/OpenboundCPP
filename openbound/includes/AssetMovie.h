#ifndef SBURB_ASSET_MOVIE_H
#define SBURB_ASSET_MOVIE_H

#include "Common.h"
#include "Asset.h"

namespace SBURB
{
    // UNSUPPORTED
    class AssetMovie : public Asset
    {
    public:
        AssetMovie(std::string name, std::string path) {
            this->type = "movie";
            this->name = name;
            this->path = path;
            this->asset = nullptr;
        };

        int* GetAsset() { return this->asset; };

        std::string GetPath() { return this->path; };

    private:
        std::string path;
        int* asset;

    };
}

#endif