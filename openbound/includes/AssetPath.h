#ifndef SBURB_ASSET_PATH_H
#define SBURB_ASSET_PATH_H

#include <Common.h>
#include "Asset.h"

namespace SBURB {
    class AssetPath : public Asset {
    public:
        AssetPath(std::wstring name, std::vector<Vector2> points);

        void Push(Vector2 point);
        void QueryBatchPos(std::map<std::wstring, sf::Vector2f> queries, std::map<std::wstring, bool>* results);
        void QueryBatchNeg(std::map<std::wstring, sf::Vector2f> queries, std::map<std::wstring, bool>* results);
        bool Query(sf::Vector2f point);

        std::vector<Vector2> GetPoints() { return this->points; };

    private:
        std::vector<Vector2> points;

    };
}

#endif