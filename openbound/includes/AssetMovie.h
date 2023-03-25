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
        AssetMovie(std::wstring name, std::wstring path) {
            this->type = L"movie";
            this->name = name;
            this->path = path;
            this->asset = nullptr;
        };

        int* GetAsset() { return this->asset; };

        std::wstring GetPath() { return this->path; };

    private:
        std::wstring path;
        int* asset;

    };
}

#endif