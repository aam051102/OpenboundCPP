#ifndef SBURB_ANIMATION_H
#define SBURB_ANIMATION_H

#include "Common.h"
#include "Asset.h"

namespace SBURB
{
    class Animation : public sf::Drawable, public sf::Transformable
    {
    public:
		Animation(std::string name, std::string sheetName, int x = 0, int y = 0, int colSize = 0, int rowSize = 0, int startPos = 0, int length = 1, std::string frameInterval = "", int loopNum = -1, std::string followUp = "", bool flipX = false, bool flipY = false, bool sliced = false, int numCols = 0, int numRows = 0);
        ~Animation();

        void NextFrame();
        void Update();

        void Reset();
        bool HasPlayed();
        bool IsVisuallyUnder(int x, int y);

        void SetColSize(int newSize);
        void SetRowSize(int newSize);

		void SetX(int newX);
		void SetY(int newY);
		void SetFlipX(bool newFlipX);
		void SetFlipY(bool newFlipY);

		std::string GetFollowUp() { return this->followUp; };

        Animation Clone(int x = 0, int y = 0);

        std::string Serialize(std::string output);

		std::string GetName() { return this->name; };

    protected:
		std::string sheetName;
		std::shared_ptr<sf::Texture> sheet;
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
		std::map<int, std::map<int, std::shared_ptr<sf::Texture>>> sheets;
		std::map<int, int> frameIntervals;
		int frameInterval;

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    };
}

#endif