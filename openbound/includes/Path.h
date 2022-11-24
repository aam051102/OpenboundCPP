#ifndef SBURB_PATH_H
#define SBURB_PATH_H

#include <Common.h>

namespace SBURB {
    class Path {
    public:
        Path();
        ~Path();

        void Push(Vector2 point);
        void QueryBatchPos(std::map<std::string, Vector2> queries, std::map<std::string, bool>* results);
        void QueryBatchNeg(std::map<std::string, Vector2> queries, std::map<std::string, bool>* results);
        bool Query(Vector2 point);

    private:
        std::vector<Vector2> points;

    };
}

#endif