#ifndef SBURB_ASSET_H
#define SBURB_ASSET_H

#include <Common.h>

// Is Asset even necessary? Can't it all simply be kept in the asset handler/manager?
namespace SBURB {
    enum class AssetType {
        Graphic,
        Audio,
        Path,
        Movie,
        Font,
        Text
    };

    class Asset {
    public:
        Asset();
        ~Asset();

        std::string name;
        int width;
        int height;

    private:
        AssetType type;
        bool loaded;

    };
}

#endif