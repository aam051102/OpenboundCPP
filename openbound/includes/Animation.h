#ifndef SBURB_ANIMATION_H
#define SBURB_ANIMATION_H

#include "Common.h"
#include "AssetGraphic.h"

namespace SBURB
{
    class Animation : public sf::Drawable, public sf::Transformable
    {
    public:
		Animation(std::string name, std::string sheetName, int x = 0, int y = 0, int colSize = 0, int rowSize = 0, int startPos = 0, int length = 1, std::string frameInterval = "", int loopNum = -1, std::string followUp = "", bool flipX = false, bool flipY = false, bool sliced = false, int numCols = 1, int numRows = 1);
        ~Animation();

        void NextFrame();
        void Update();

        void Reset();
        bool HasPlayed();
        bool IsVisuallyUnder(int x, int y);

        void SetColSize(int newSize);
		int GetColSize() { return this->colSize; };

        void SetRowSize(int newSize);
		int GetRowSize() { return this->rowSize; };

		void SetX(int newX);
		int GetX() { return this->x; };

		void SetY(int newY);
		int GetY() { return this->y; };

		void SetFlipX(bool newFlipX);
		void SetFlipY(bool newFlipY);

		void SetFrameInterval(int frameInterval) { this->frameInterval = frameInterval; };
		int GetFrameInterval() { return this->frameInterval; };

		std::shared_ptr<AssetGraphic> GetSheet() { return this->sheet; };

		std::string GetFollowUp() { return this->followUp; };

        std::shared_ptr<Animation> Clone(int x = 0, int y = 0);

        std::string Serialize(std::string output);

		std::string GetName() { return this->name; };

    protected:
		std::string sheetName;
		std::shared_ptr<AssetGraphic> sheet;
		bool sliced;
		int x;
		int y;
		int rowSize;
		int colSize;
		int startPos;
		int length;
		int curInterval;
		int curFrame;
		int curLoop;
		std::string name;
		int loopNum;
		std::string followUp;
		bool flipX;
		bool flipY;
		int numRows;
		int numCols;
		std::map<int, std::map<int, std::shared_ptr<AssetGraphic>>> sheets;
		std::map<int, int> frameIntervals;
		int frameInterval;

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    };
}

#endif