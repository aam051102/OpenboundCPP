#ifndef SBURB_ASSET_H
#define SBURB_ASSET_H

#include <Common.h>

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


    private:
        std::string name;
        AssetType type;
        bool loaded;

    };
}

#endif