#include "FontEngine.h"
#include "Sburb.h"
#include "AssetManager.h"

namespace SBURB {
    FontEngine::FontEngine(std::string text) {
		this->fontName = "SburbFont";
		this->fontSize = 14;
		this->fontStyle = sf::Text::Bold;
		this->color = sf::Color::Black;
		this->text = text;
		this->x = 0;
		this->y = 0;
		this->width = 999999;
		this->height = 999999;
		this->start = 0;
		this->end = 999999;
		this->lines = {};
		this->lineHeight = 17;
		this->charWidth = 8;
		this->align = "left";
		this->escaped = {};

		this->formatted = true;

		this->formatQueue = {};

        this->prefixColours = {
            { "aa", 0xa10000ff }, { "aradia", 0xa10000ff },
            { "ac", 0x416600ff }, { "nepeta", 0x416600ff },
            { "ag", 0x005682ff }, { "vriska", 0x005682ff },
            { "at", 0xa15000ff }, { "tavros", 0xa15000ff },
            { "ca", 0x6a006aff }, { "eridan", 0x6a006aff },
            { "cc", 0x77003cff }, { "feferi", 0x77003cff },
            { "cg", 0x626262ff }, { "karkat", 0x626262ff },
            { "ct", 0x000056ff }, { "equius", 0x000056ff },
            { "ga", 0x008141ff }, { "kanaya", 0x008141ff },
            { "gc", 0x008282ff }, { "terezi", 0x008282ff },
            { "ta", 0xa1a100ff }, { "sollux", 0xa1a100ff },
            { "tc", 0x2b0057ff }, { "gamzee", 0x2b0057ff },
            { "dave", 0xe00707ff },
            { "meenah", 0x77003cff },
            { "rose", 0xb536daff },
            { "aranea", 0x005682ff },
            { "kankri", 0xff0000ff },
            { "porrum", 0x008141ff },
            { "latula", 0x008282ff },
        };
    }

	FontEngine::~FontEngine() {

	}

    void FontEngine::SetText(std::string text) {
        this->text = text;
        this->ParseEverything();
    }

    void FontEngine::ShowSubText(int start, int end) {
        this->start = start;
        this->end = end;
    }

    void FontEngine::SetDimensions(int x, int y, int width, int height) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->ParseText();
    }

    void FontEngine::ParseEverything() {
        this->ParseFormatting();
        this->ParseText();
    }

    void FontEngine::ParseText() {
		this->lines = {};
		int i = 0;
		int lastSpace = 0;
		int lineStart = 0;

		sf::Text textMeasurer;
		textMeasurer.setFont(*AssetManager::GetFontByName(this->fontName)->GetAsset());
		textMeasurer.setCharacterSize(this->fontSize);
		textMeasurer.setStyle(this->fontStyle);

		for (i = 0; i < this->text.size(); i++) {
			if (this->text[i] == ' ') {
				lastSpace = i;
			}
			else if (this->text[i] == '\n') {
				this->lines.push_back(this->text.substr(lineStart, i - lineStart));
				lineStart = i + 1;
				lastSpace = lineStart;
				continue;
			}

			textMeasurer.setString(this->text.substr(lineStart, i + 1 - lineStart));
			if (textMeasurer.getGlobalBounds().width > this->width) {
				if (lineStart == lastSpace) {
					this->lines.push_back(this->text.substr(lineStart, i - lineStart));
					lineStart = i;
					lastSpace = i;
				}
				else {
					this->lines.push_back(this->text.substr(lineStart, lastSpace - lineStart));
					lineStart = lastSpace + 1;
					lastSpace = lineStart;
				}
			}
		}

		this->lines.push_back(this->text.substr(lineStart, i - lineStart));
    }
	
	void FontEngine::ParseFormatting() {
		this->formatQueue = {};

		if (this->formatted) {
			this->escaped = {};

			this->ParsePrefixes();
			this->ParseEscapes();
			this->ParseUnderlines();
			this->ParseColors();
		}
	}

	void FontEngine::ParseEscapes() {
		size_t index;
		int escapeLocation = 0;

		do {
			index = this->text.find("/", escapeLocation);

			if (index < this->text.size() - 1 && index != std::string::npos) {
				char character = this->text[index + 1];

				if (character == '/') {
					escapeLocation = index + 1;
				}
				else {
					auto characterListing = &this->escaped[character];
					if (!characterListing) {
						this->escaped[character] = {};
						characterListing = &this->escaped[character];
					}

					int count = 0;
					for (int i = 0; i < index; i++) {
						if (this->text[i] == character) {
							count++;
						}
					}

					(*characterListing)[count + 1] = true;
				}
			}

			if (index == std::string::npos) {
				break;
			}

			this->text = this->text.substr(0, index) + this->text.substr(index + 1);
		} while (index >= 0);
	}

	void FontEngine::ParsePrefixes() {
		std::string prefix = this->text.substr(0, this->text.find(" "));
		std::string actor;
		if (prefix != "!") {
			if (prefix.find("_") != std::string::npos) {
				actor = prefix.substr(0, this->text.find("_"));
			}
			else {
				actor = prefix.substr(0, 2);
			}

			this->ParsePrefix(actor);
		}

		this->text = trim(this->text.substr(prefix.size(), this->text.size() - prefix.size()));
	}

	void FontEngine::ParseUnderlines() {
		int escapePoint = 0;
		size_t index = 0;
		int count = 0;
		while (true) {
			while (true) {
				count++;
				index = this->text.find("_", escapePoint);
				if (this->escaped.find('_') != this->escaped.end() && this->escaped['_'][count]) {
					escapePoint = index + 1;
				}
				else {
					break;
				}
			}
			
			if (index == std::string::npos) {
				break;
			}

			bool closing = false;
			for (int i = this->formatQueue.size() - 1; i >= 0; i--) {
				if (this->formatQueue[i].type == "underline" && this->formatQueue[i].maxIndex == 999999) {
					this->formatQueue[i].maxIndex = index;
					closing = true;
					break;
				}
			}

			if (!closing) {
				this->AddToFormatQueue(FormatRange(index, 999999, "underline"));
			}
			
			this->text = this->text.substr(0, index) + this->text.substr(index + 1);
			this->RealignFormatQueue(index, 1);
		}
	}

	// I (MadCreativity) wrote this function, so it may be faulty.
	sf::Color HexToColor(std::string hex) {
		if (hex.size() != 6) throw std::invalid_argument("Hex is not 6 characters long.");
		std::transform(hex.begin(), hex.end(), hex.begin(), [](unsigned char c) { return std::tolower(c); });

		int32_t c = 0x000000FF;

		for (int i = 0; i < hex.size(); i++) {
			int val = 0;

			if (hex[i] >= 'a' && hex[i] <= 'f') {
				val = hex[i] - 'a' + 10;
			}
			else {
				val = stoi(std::to_string(hex[i]));
			}

			c += pow(16, 7 - i) * val;
		}

		return sf::Color(c);
	}

	void FontEngine::ParseColors() {
		int escapePoint = 0;
		size_t index = 0;
		int count = 0;

		while (true) {
			while (true) {
				count++;
				index = this->text.find("#", escapePoint);
				if (this->escaped.find('#') != this->escaped.end() && this->escaped['#'][count]) {
					escapePoint = index + 1;
				}
				else {
					break;
				}
			}

			if (index == std::string::npos) {
				break;
			}

			if (this->text.find("##", escapePoint) == index) {
				for (int i = this->formatQueue.size()  - 1; i >= 0; i--) {
					if (this->formatQueue[i].type == "colour" && this->formatQueue[i].maxIndex == 999999) {
						this->formatQueue[i].maxIndex = index;
						break;
					}
				}
				count++;

				this->text = this->text.substr(0, index) + this->text.substr(index + 2);
				this->RealignFormatQueue(index, 2);
			}
			else {
				this->AddToFormatQueue(FormatRange(index, 999999, "colour", HexToColor(this->text.substr(index + 1, index + 7 - (index + 1)))));
				this->text = this->text.substr(0, index) + this->text.substr(index + 7, this->text.size() - (index + 7));
				this->RealignFormatQueue(index, 7);
			}
		}
	}

	void FontEngine::AddToFormatQueue(FormatRange format) {
		int newPlace = this->formatQueue.size();

		for (int i = 0; i < this->formatQueue.size(); i++) {
			if (this->formatQueue[i].minIndex > format.minIndex) {
				newPlace = i;
				break;
			}
		}

		this->formatQueue.insert(this->formatQueue.begin() + newPlace, format);
	}

	void FontEngine::RealignFormatQueue(int startPos, int shiftSize) {
		for (int i = 0; i < this->formatQueue.size(); i++) {
			FormatRange* curFormat = &this->formatQueue[i];

			if (curFormat->maxIndex > startPos && curFormat->maxIndex != 999999) {
				curFormat->maxIndex -= shiftSize;
			}
			if (curFormat->minIndex > startPos) {
				curFormat->minIndex -= shiftSize;
			}
		}
	}

	void FontEngine::ParsePrefix(std::string prefix) {
		this->formatQueue.push_back(FormatRange(0, this->text.size(), "colour", this->PrefixColouration(prefix)));
	}

	sf::Color FontEngine::PrefixColouration(std::string prefix) {
		std::transform(prefix.begin(), prefix.end(), prefix.begin(), [](unsigned char c) { return std::tolower(c); });

		if (this->prefixColours[prefix]) {
			return sf::Color(this->prefixColours[prefix]);
		}
		else {
			return sf::Color::Black;
		}
	}

	bool FontEngine::NextBatch() {
		this->RealignFormatQueue(-1, this->BatchLength());
		this->lines.erase(this->lines.begin() + 0, this->lines.begin() + std::min(this->lines.size(), (size_t)floor(this->height / this->lineHeight)));
		return this->lines.size();
	}

	bool FontEngine::OnLastBatch() {
		return floor(this->height / this->lineHeight) >= this->lines.size();
	}

	bool FontEngine::IsShowingAll() {
		return this->end >= this->BatchLength();
	}

	int FontEngine::BatchLength() {
		int len = 0;
		int i;

		for (i = 0; i < floor(this->height / this->lineHeight) && i < this->lines.size(); i++) {
			len += this->lines[i].size();
		}

		return len;
	}

	void FontEngine::ShowAll() {
		this->end = this->BatchLength() + 1;
	}

	void FontEngine::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		int i;
		int lenCount;
		int linePos = 0;
		int strStart, strEnd;
		int currentFormat = 0;
		std::vector<FormatRange> currentFormats = {};
		int nextStop;
		std::string curLine;

		i = 0;
		lenCount = 0;
		int offsetX = 0;

		sf::Text textWriter;
		textWriter.setFont(*AssetManager::GetFontByName(this->fontName)->GetAsset().get());
		textWriter.setCharacterSize(this->fontSize);

		while (i < floor(this->height / this->lineHeight) && i < this->lines.size()) {
			textWriter.setFillColor(this->color);
			textWriter.setStyle(this->fontStyle);
			
			curLine = this->lines[i];

			auto curColor = this->color;
			bool underlining = false;

			nextStop = curLine.size();

			if (currentFormat < this->formatQueue.size() && this->formatQueue[currentFormat].minIndex <= lenCount + linePos) {
				currentFormats.push_back(this->formatQueue[currentFormat]);
				currentFormat++;
			}
			
			for (int k = currentFormats.size() - 1; k >= 0; k--) {
				if (currentFormats[k].maxIndex <= lenCount + linePos) {
					currentFormats.erase(currentFormats.begin() + k);
				}
			}

			for (int k = 0; k < currentFormats.size(); k++) {
				if (currentFormats[k].type == "colour") {
					curColor = currentFormats[k].extra;

				}
				else if (currentFormats[k].type == "underline") {
					underlining = true;
				}
				else if (currentFormats[k].type == "italic") {
					textWriter.setStyle(sf::Text::Italic | this->fontStyle);
				}
			}

			if (currentFormat < this->formatQueue.size()  && this->formatQueue[currentFormat].minIndex < lenCount + curLine.size()) {
				if (this->formatQueue[currentFormat].minIndex < this->end) {
					nextStop = std::min(nextStop, this->formatQueue[currentFormat].minIndex - lenCount);
				}
			}

			for (int k = 0; k < currentFormats.size(); k++) {
				if (currentFormats[k].maxIndex < this->end) {
					nextStop = std::min(nextStop, currentFormats[k].maxIndex - lenCount);
				}
			}

			if (nextStop != curLine.size()) {
				strStart = linePos;
				strEnd = nextStop;
				linePos += strEnd - strStart;
			}
			else {
				if (lenCount + curLine.size() <= this->end) { //if the line wouldn't take me past the displayed length
					strEnd = curLine.size(); //do the whole line
				}
				else { //otherwise, if the line would take me past the displayed length
					strEnd = this->end - lenCount; //only show up to the limit
				}

				if (lenCount + linePos >= this->start) { //if the start of the line is within the bounds of the displayed length
					strStart = linePos; //display from the start of the line
				}
				else if (lenCount + curLine.size() >= this->start) { //otherwise, if any part of the line should be displayed
					strStart = this->start - (lenCount)+linePos; //display from where we should start

					sf::Text textMeasurer(textWriter);
					textMeasurer.setString(curLine.substr(linePos, strStart - linePos));
					offsetX += textMeasurer.getGlobalBounds().width;
				}
				else { //otherwise, don't show this line at all
					strStart = linePos;
					strEnd = linePos;
				}

				linePos = -1;
			}

			int numChars = strEnd - strStart;

			if (numChars > 0) {
				int startX = this->x + offsetX;
				int startY = this->y + i * this->lineHeight;

				textWriter.setFillColor(curColor);
				textWriter.setPosition(startX, startY);
				textWriter.setString(curLine.substr(strStart, strEnd - strStart));
				
				if (this->align == "center") {
					textWriter.setOrigin(sf::Vector2f(textWriter.getLocalBounds().width / 2, 0));
				}
				else if (this->align == "right") {
					textWriter.setOrigin(sf::Vector2f(textWriter.getLocalBounds().width, 0));
				}
				else {
					textWriter.setOrigin(sf::Vector2f(0, 0));
				}

				target.draw(textWriter, states);

				offsetX += textWriter.getGlobalBounds().width;

				if (underlining && strStart < strEnd) {
					sf::RectangleShape underlineShape(sf::Vector2f(numChars * this->charWidth, this->lineHeight - 3 + 0.6));
					underlineShape.setPosition(startX, startY + this->lineHeight - 3);
					underlineShape.setFillColor(curColor);
					target.draw(underlineShape, states);
				}
			}

			if (linePos == -1) {
				lenCount += this->lines[i].size() + 1;
				linePos = 0;
				offsetX = 0;
				i++;
			}
		}
	}
}