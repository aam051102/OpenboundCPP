#ifndef SBURB_ASSET_H
#define SBURB_ASSET_H

#include <string>

namespace SBURB {
    class Asset {
    public:
        std::string GetType() { return this->type; };
        std::string GetName() { return this->name; };

    protected:
        std::string type;
        std::string name;
    };
}
#endif