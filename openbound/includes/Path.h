#ifndef SBURB_PATH_H
#define SBURB_PATH_H

#include <Common.h>

namespace SBURB {
    struct Point {
        int x;
        int y;

        Point(int x, int y) { this->x = x; this->y = y; };
    };

    class Path {
    public:
        Path();
        ~Path();

        void Push(Point point);
        void QueryBatchPos(std::vector<Point> queries, std::vector<bool>* results);
        void QueryBatchNeg(std::vector<Point> queries, std::vector<bool>* results);
        bool Query(Point point);

    private:
        std::vector<Point> points;

    };
}

#endif