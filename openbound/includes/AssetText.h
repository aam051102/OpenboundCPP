#ifndef SBURB_ASSET_TEXT_H
#define SBURB_ASSET_TEXT_H

#include "Common.h"
#include "Asset.h"

namespace SBURB
{
    class AssetText : public Asset
    {
    public:
        AssetText(std::string name, std::string text) {
            this->type = "text";
            this->name = name;
            this->text = unescape(trim(text));
        };

        const char* GetText() { return this->text; };

    private:
        const char* text;

    };
}

#endif