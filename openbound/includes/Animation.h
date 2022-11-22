#ifndef SBURB_ANIMATION_H
#define SBURB_ANIMATION_H

#include "Common.h"
#include "Asset.h"

namespace SBURB
{
    class Animation : public sf::Drawable, public sf::Transformable
    {
    public:
		// TODO: Figure out what to do for "sheet", since it can be either a string (for sliced) or an Asset for unsliced. Is sliced necessary? I suppose it's best to keep it.
        Animation(std::string name, Asset sheet, int x = 0, int y = 0, int colSize = 0, int rowSize = 0, int startPos = 0, int length = 0, int frameInterval = 0, int loopNum = 0, std::string followUp = "", bool flipX = false, bool flipY = false, bool sliced = false, int numCols = 0, int numRows = 0);
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

		std::string GetName() { return this->name; };


    protected:
		Asset sheet;
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

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    };
}

#endif