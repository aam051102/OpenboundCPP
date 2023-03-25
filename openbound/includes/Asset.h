#ifndef SBURB_ASSET_H
#define SBURB_ASSET_H

#include <string>

namespace SBURB {
    class Asset {
    public:
        std::wstring GetType() { return this->type; };
        std::wstring GetName() { return this->name; };

    protected:
        std::wstring type;
        std::wstring name;
    };
}
#endif