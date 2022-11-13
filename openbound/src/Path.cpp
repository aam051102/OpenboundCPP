#include "Path.h"

namespace SBURB {
	Path::Path() {
		
	}

	Path::~Path() {

	}

	void Path::Push(Point point) {
		this->points.push_back(point);
	}

	void Path::QueryBatchPos(std::vector<Point> queries, std::vector<bool>* results) {
		for (int i = 0; i < queries.size(); i++) {
			(*results)[i] = this->Query(queries[i]);
		}
	}

	void Path::QueryBatchNeg(std::vector<Point> queries, std::vector<bool>* results) {
		for (int i = 0; i < queries.size(); i++) {
			(*results)[i] = !this->Query(queries[i]);
		}
	}

    bool Path::Query(Point point) {
		bool isOnPath = false;
		
		for (int i = -1, l = this->points.size(), j = l - 1; ++i < l; j = i) {
			Point pointA = this->points[i];
			Point pointB = this->points[j];

			if ((pointA.y <= point.y && point.y < pointB.y) || (pointB.y <= point.y && point.y < pointA.y)) {
				if (point.x < (pointB.x - pointA.x) * (point.y - pointA.y) / (pointB.y - pointA.y) + pointA.x) {
					isOnPath = !isOnPath;
				}
			}
		}

		return isOnPath;
    }
}