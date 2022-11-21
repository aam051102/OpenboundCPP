#ifndef SBURB_ANIMATION_H
#define SBURB_ANIMATION_H

#include "Common.h"
#include "Asset.h"

namespace SBURB
{
    class Animation
    {
    public:
        Animation(std::string name, std::string sheet, int x, int y, int colSize, int rowSize, int startPos, int length, int frameInterval, int loopNum, std::string followUp, bool flipX = false, bool flipY = false, bool sliced = false, int numCols = 0, int numRows = 0);
        ~Animation();

        void NextFrame();
        void Update();

        void DrawNormal(int x, int y);
        void DrawSliced(int x, int y);
		void Draw(int x, int y);

        void Reset();
        bool HasPlayed();
        bool IsVisuallyUnder(int x, int y);

        void SetColSize(int newSize);
        void SetRowSize(int newSize);
        void SetSheet(std::string newSheet);

        Animation Clone(int x, int y);

        std::string Serialize(std::string output);


    protected:
		std::string sheet;
		bool sliced;
		int x;
		int y;
		int rowSize;
		int colSize;
		int startPos;
		int length;
		int curInterval;
		int curFrame;
		std::string name;
		int loopNum;
		std::string followUp;
		bool flipX;
		bool flipY;
		int numRows;
		int numCols;
		std::vector<std::vector<std::shared_ptr<Asset>>> sheets;
		std::vector<int> frameIntervals;
		int frameInterval;

    };
}

#endif