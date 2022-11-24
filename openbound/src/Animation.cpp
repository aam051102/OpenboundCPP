#include "Animation.h"
#include "Serializer.h"
#include "AssetHandler.h"
#include "BatchHandler.h"

namespace SBURB {
    Animation::Animation(std::string name, std::string sheetName, int x, int y, int colSize, int rowSize, int startPos, int length, std::string frameInterval, int loopNum, std::string followUp, bool flipX, bool flipY, bool sliced, int numCols, int numRows) {
		this->sheetName = sheetName;
		this->sliced = sliced;
		this->x = x;
		this->y = y;
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
					std::shared_ptr<sf::Texture> texture = AssetHandler::GetTextureByName(sheetName + "_" + std::to_string(colNum) + "_" + std::to_string(rowNum));

					if (texture) {
						if (this->sheets.find(colNum) == this->sheets.end()) {
							this->sheets[colNum] = {};
						}

						this->sheets[colNum][rowNum] = texture;
					}
				}
			}
		}
		else {
			this->sheet = AssetHandler::GetTextureByName(sheetName);
			this->numRows = this->sheet->getSize().y / this->rowSize;
			this->numCols = this->sheet->getSize().x / this->colSize;
		}

		this->rowSize = rowSize ? rowSize : this->sheet->getSize().y;
		this->colSize = colSize ? colSize : this->sheet->getSize().x;

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
		this->curFrame++;

		if (this->curFrame >= this->length) {
			if (this->curLoop == this->loopNum) {
				this->curFrame = this->length - 1;
			}
			else {
				this->curFrame = 0;
				if (this->loopNum >= 0) {
					this->curLoop++;
				}
			}
		}

		if (this->frameIntervals[this->curFrame]) {
			this->frameInterval = this->frameIntervals[this->curFrame];
		}
	}

	void Animation::Update() {
		this->curInterval++;

		while (this->curInterval > this->frameInterval) {
			this->curInterval -= this->frameInterval;
			NextFrame();
		}
	}

	void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		// Instead of doing transformations here, do them directly on Animation.
		states.transform *= getTransform();

		if (this->sliced) {
			// TODO: Keep inside of camera view for optimization??? May not be necessary.
			for (int colNum = 0; colNum <= this->numCols; colNum++) {
				for (int rowNum = 0; rowNum <= this->numRows; rowNum++) {
					if (this->sheets.at(colNum).at(rowNum)) {
						std::shared_ptr<sf::Texture> sheet = this->sheets.at(colNum).at(rowNum);
						int frameX = 0;
						int frameY = 0;
						int drawWidth = sheet->getSize().x;
						int drawHeight = sheet->getSize().y;
						int offsetX = colNum * this->colSize;
						int offsetY = rowNum * this->rowSize;

						sf::FloatRect transformRect({ offsetX, offsetY }, sf::Vector2f(drawWidth, drawHeight));
						transformRect = states.transform.transformRect(transformRect);
						sf::VertexArray arr(sf::Quads, 4);
						arr[0].position = sf::Vector2f(transformRect.left, transformRect.top);
						arr[1].position = sf::Vector2f(transformRect.left + transformRect.width, transformRect.top);
						arr[2].position = sf::Vector2f(transformRect.left + transformRect.width, transformRect.top + transformRect.height);
						arr[3].position = sf::Vector2f(transformRect.left, transformRect.top + transformRect.height);

						arr[0].texCoords = sf::Vector2f(frameX, frameY);
						arr[1].texCoords = sf::Vector2f(frameX + drawWidth, frameY);
						arr[2].texCoords = sf::Vector2f(frameX + drawWidth, frameY + drawHeight);
						arr[3].texCoords = sf::Vector2f(frameX, frameY + drawHeight);

						arr[0].color = sf::Color::White;
						arr[1].color = sf::Color::White;
						arr[2].color = sf::Color::White;
						arr[3].color = sf::Color::White;

						BatchHandler::getInstance().DrawSpriteRect(this->sheetName, arr, target);
					}
				}
			}
		}
		else {
			int colNum = ((this->startPos + this->curFrame) % this->numCols);
			int rowNum = (floor((this->startPos + this->curFrame - colNum) / this->numCols));
			int frameX = colNum * this->colSize;
			int frameY = rowNum * this->rowSize;
			int drawWidth = this->colSize;
			int drawHeight = this->rowSize;

			sf::FloatRect transformRect({ 0, 0 }, sf::Vector2f(drawWidth, drawHeight));
			transformRect = states.transform.transformRect(transformRect);
			sf::VertexArray arr(sf::Quads, 4);
			arr[0].position = sf::Vector2f(transformRect.left, transformRect.top);
			arr[1].position = sf::Vector2f(transformRect.left + transformRect.width, transformRect.top);
			arr[2].position = sf::Vector2f(transformRect.left + transformRect.width, transformRect.top + transformRect.height);
			arr[3].position = sf::Vector2f(transformRect.left, transformRect.top + transformRect.height);

			arr[0].texCoords = sf::Vector2f(frameX, frameY);
			arr[1].texCoords = sf::Vector2f(frameX + drawWidth, frameY);
			arr[2].texCoords = sf::Vector2f(frameX + drawWidth, frameY + drawHeight);
			arr[3].texCoords = sf::Vector2f(frameX, frameY + drawHeight);

			arr[0].color = sf::Color::White;
			arr[1].color = sf::Color::White;
			arr[2].color = sf::Color::White;
			arr[3].color = sf::Color::White;

			BatchHandler::getInstance().DrawSpriteRect(this->sheetName, arr, target);
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
		this->numCols = this->sheet->getSize().x / this->colSize;
		Reset();
	}

	void Animation::SetRowSize(int newSize) {
		this->rowSize = newSize;
		this->numRows = this->sheet->getSize().y / this->rowSize;
		Reset();
	}

	Animation Animation::Clone(int x, int y) {
		return Animation(this->name, this->sheetName, x + this->x, y + this->y, this->colSize, this->rowSize, this->startPos, this->length, std::to_string(this->frameInterval), this->loopNum, this->followUp, this->flipX, this->flipY, this->sliced, this->numCols, this->numRows);
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
			("sheet='" + this->sheetName + "' ") +
			((this->name != "image") ? "name='" + this->name + "' " : "") +
			Serializer::SerializeAttribute("x", this->x) +
			Serializer::SerializeAttribute("y", this->y) +
			((this->rowSize != this->sheet->getSize().y) ? "rowSize='" + std::to_string(this->rowSize) + "' " : "") +
			((this->colSize != this->sheet->getSize().x) ? "colSize='" + std::to_string(this->colSize) + "' " : "") +
			Serializer::SerializeAttribute("startPos", this->startPos) +
			((this->length != 1) ? "length='" + std::to_string(this->length) + "' " : "") +
			((frameInterval != "") ? "frameInterval='" + frameInterval + "' " : "") +
			((this->loopNum != -1) ? "loopNum='" + std::to_string(this->loopNum) + "' " : "") +
			Serializer::SerializeAttribute("followUp", this->followUp) +
			Serializer::SerializeAttribute("flipX", this->flipX) +
			Serializer::SerializeAttribute("flipY", this->flipY) +
			(this->sliced ? ("sliced='true' numCols='" + std::to_string(this->numCols) + "' numRows='" + std::to_string(this->numRows) + "' ") : ("")) +
			" />";

		return output;
	}
}