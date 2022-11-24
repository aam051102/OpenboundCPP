#include "Path.h"

namespace SBURB {
	Path::Path() {
		
	}

	Path::~Path() {

	}

	void Path::Push(Vector2 point) {
		this->points.push_back(point);
	}

	void Path::QueryBatchPos(std::map<std::string, Vector2> queries, std::map<std::string, bool>* results) {
		for (auto query : queries) {
			(*results)[query.first] = this->Query(query.second);
		}
	}

	void Path::QueryBatchNeg(std::map<std::string, Vector2> queries, std::map<std::string, bool>* results) {
		for (auto query : queries) {
			(*results)[query.first] = !this->Query(query.second);
		}
	}

    bool Path::Query(Vector2 point) {
		bool isOnPath = false;
		
		for (int i = -1, l = this->points.size(), j = l - 1; ++i < l; j = i) {
			Vector2 pointA = this->points[i];
			Vector2 pointB = this->points[j];

			if ((pointA.y <= point.y && point.y < pointB.y) || (pointB.y <= point.y && point.y < pointA.y)) {
				if (point.x < (pointB.x - pointA.x) * (point.y - pointA.y) / (pointB.y - pointA.y) + pointA.x) {
					isOnPath = !isOnPath;
				}
			}
		}

		return isOnPath;
    }
}