#include "Animation.h"

namespace SBURB {
	// Problem: Sheet can be either string or asset.
	// Solution: ???
	// Thoughts:
	// Is it even necessary to parse it on like that? If we keep all assets in the asset handler, couldn't we simply always pass along the ID/sheet name? Check original code.
    Animation::Animation(std::string name, Asset sheet, int x, int y, int colSize, int rowSize, int startPos, int length, std::string frameInterval, int loopNum, std::string followUp, bool flipX, bool flipY, bool sliced, int numCols, int numRows) {
		this->sheet = sheet;
		this->sliced = sliced;
		this->x = x;
		this->y = y;
		this->rowSize = rowSize ? rowSize : sheet.height;
		this->colSize = colSize ? colSize : sheet.width;
		this->startPos = startPos;
		this->length = length;
		this->curInterval = 0;
		this->curFrame = 0;
		this->name = name;
		this->loopNum = loopNum;
		this->curLoop = 0;
		this->followUp = followUp;
		this->flipX = flipX;
		this->flipY = flipY;

		if (sliced) {
			this->numRows = numRows;
			this->numCols = numCols;
			this->sheets = {};

			for (int colNum = 0; colNum < this->numCols; colNum++) {
				for (int rowNum = 0; rowNum < this->numRows; rowNum++) {
					std::shared_ptr<Asset> asset = assets[this->sheet + "_" + colNum + "_" + rowNum];

					if (asset) {
						if (this->sheets.find(colNum) == this->sheets.end()) {
							this->sheets[colNum] = {};
						}
						this->sheets[colNum][rowNum] = std::make_shared<Asset>(asset);
					}
				}
			}
		}
		else {
			this->numRows = sheet.height / this->rowSize;
			this->numCols = sheet.width / this->colSize;
		}

		if (frameInterval == "") {
			this->frameInterval = 1;
		} else {
			if (frameInterval.find(":") == -1) {
				this->frameInterval = stoi(frameInterval);
			}
			else {
				std::vector<std::string> intervals = split(frameInterval, ",");
				this->frameIntervals = {};

				for (int i = 0; i < intervals.size(); i++) {
					std::vector<std::string> pair = split(intervals[i], ":");
					this->frameIntervals[stoi(pair[0])] = stoi(pair[1]);
				}
				if (!this->frameIntervals[0]) {
					this->frameIntervals[0] = 1;
				}
				this->frameInterval = this->frameIntervals[this->curFrame];
			}
		}
    }

    Animation::~Animation() {

    }

	void Animation::NextFrame() {

	}

	void Animation::Update() {

	}

	void Animation::DrawNormal(int x, int y) {

	}

	void Animation::DrawSliced(int x, int y) {

	}

	void Animation::Draw(int x, int y) {
		if (this->sliced) {
			DrawSliced(x, y);
		}
		else {
			DrawNormal(x, y);
		}
	}

	void Animation::Reset() {
		this->curFrame = 0;
		this->curInterval = 0;
		this->curLoop = 0;
	}

	bool Animation::HasPlayed() {
		return this->curLoop == this->loopNum && this->curFrame == this->length - 1;
	}

	bool Animation::IsVisuallyUnder(int x, int y) {
		if (x >= this->x && x <= this->x + this->colSize) {
			if (y >= this->y && y <= this->y + this->rowSize) {
				return true;
			}
		}

		return false;
	}

	void Animation::SetColSize(int newSize) {
		this->colSize = newSize;
		this->numCols = this->sheet.width / this->colSize;
		Reset();
	}

	void Animation::SetRowSize(int newSize) {
		this->rowSize = newSize;
		this->numRows = this->sheet.height / this->rowSize;
		Reset();
	}

	void Animation::SetSheet(Asset newSheet) {
		this->sheet = newSheet;
		this->numRows = this->sheet.height / this->rowSize;
		this->numCols = this->sheet.width / this->colSize;
		Reset();
	}

	Animation Animation::Clone(int x, int y) {
		return Animation(this->name, this->sheet, x + this->x, y + this->y, this->colSize, this->rowSize, this->startPos, this->length, std::to_string(this->frameInterval), this->loopNum, this->followUp, this->flipX, this->flipY, this->sliced, this->numCols, this->numRows);
	}

	std::string Animation::Serialize(std::string output) {
		std::string frameInterval = "";
		bool firstInterval = true;

		if (!this->frameIntervals.empty())
		{
			for (std::pair<int, int> interval : this->frameIntervals)
			{
				frameInterval = frameInterval + (firstInterval ? "" : ",") + std::to_string(interval.first )+ ":" + std::to_string(interval.second);
				firstInterval = false;
			}
		}
		else if (this->frameInterval != 1)
		{
			frameInterval = this->frameInterval;
		}

		output = output + "\n<animation " +
			("sheet='" + this->sheet.name + "' ") +
			((this->name != "image") ? "name='" + this->name + "' " : "") +
			SerializeAttributes(this, "x", "y") +
			((this->rowSize != this->sheet.height) ? "rowSize='" + std::to_string(this->rowSize) + "' " : "") +
			((this->colSize != this->sheet.width) ? "colSize='" + std::to_string(this->colSize) + "' " : "") +
			SerializeAttribute(this, "startPos") +
			((this->length != 1) ? "length='" + std::to_string(this->length) + "' " : "") +
			((frameInterval != "") ? "frameInterval='" + frameInterval + "' " : "") +
			((this->loopNum != -1) ? "loopNum='" + std::to_string(this->loopNum) + "' " : "") +
			SerializeAttributes(this, "folowUp", "flipX", "flipY") +
			(this->sliced ? ("sliced='true' numCols='" + std::to_string(this->numCols) + "' numRows='" + std::to_string(this->numRows) + "' ") : ("")) +
			" />";

		return output;
	}
}