#ifndef SBURB_ASSET_TEXT_H
#define SBURB_ASSET_TEXT_H

#include "Common.h"
#include "Asset.h"

namespace SBURB
{
    class AssetText : public Asset
    {
    public:
        AssetText(std::wstring name, std::wstring text) {
            this->type = L"text";
            this->name = name;
            this->text = unescape(trim(text));
        };

        const wchar_t* GetText() { return this->text; };

    private:
        const wchar_t* text;

    };
}

#endif