#include "AssetPath.h"

namespace SBURB {
    AssetPath::AssetPath(std::wstring name, std::vector<Vector2> points) {
        this->type = L"path";
        this->name = name;
        this->points = points;
    }

    void AssetPath::Push(Vector2 point) {
        this->points.push_back(point);
    }

    void AssetPath::QueryBatchPos(std::map<std::wstring, sf::Vector2f> queries, std::map<std::wstring, bool>* results) {
        for (auto query : queries) {
            (*results)[query.first] = (*results)[query.first] || this->Query(query.second);
        }
    }

    void AssetPath::QueryBatchNeg(std::map<std::wstring, sf::Vector2f> queries, std::map<std::wstring, bool>* results) {
        for (auto query : queries) {
            (*results)[query.first] = (*results)[query.first] && !this->Query(query.second);
        }
    }

    bool AssetPath::Query(sf::Vector2f point) {
        bool isOnPath = false;

        for (int i = -1, l = this->points.size(), j = l - 1; ++i < l; j = i) {
            Vector2 pointA = this->points[i];
            Vector2 pointB = this->points[j];

            if ((pointA.y <= point.y && point.y < pointB.y) || (pointB.y <= point.y && point.y < pointA.y)) {
                if (point.x < (pointB.x - pointA.x) * (point.y - pointA.y) / (float)(pointB.y - pointA.y) + pointA.x) {
                    isOnPath = !isOnPath;
                }
            }
        }

        return isOnPath;
    }
}