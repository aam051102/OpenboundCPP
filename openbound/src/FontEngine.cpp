#include "FontEngine.h"
#include "Sburb.h"

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
            { "aa", 0xa10000 }, { "aradia", 0xa10000 },
            { "ac", 0x416600 }, { "nepeta", 0x416600 },
            { "ag", 0x005682 }, { "vriska", 0x005682 },
            { "at", 0xa15000 }, { "tavros", 0xa15000 },
            { "ca", 0x6a006a }, { "eridan", 0x6a006a },
            { "cc", 0x77003c }, { "feferi", 0x77003c },
            { "cg", 0x626262 }, { "karkat", 0x626262 },
            { "ct", 0x000056 }, { "equius", 0x000056 },
            { "ga", 0x008141 }, { "kanaya", 0x008141 },
            { "gc", 0x008282 }, { "terezi", 0x008282 },
            { "ta", 0xa1a100 }, { "sollux", 0xa1a100 },
            { "tc", 0x2b0057 }, { "gamzee", 0x2b0057 },
            { "dave", 0xe00707 },
            { "meenah", 0x77003c },
            { "rose", 0xb536da },
            { "aranea", 0x005682 },
            { "kankri", 0xff0000 },
            { "porrum", 0x008141 },
            { "latula", 0x008282 },
        };

		this->textWriter.setFont(*Sburb::GetInstance()->GetFont(this->fontName));
		this->textWriter.setFillColor(this->color);
		this->textWriter.setCharacterSize(this->fontSize);
		this->textWriter.setStyle(this->fontStyle);
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

			if (Sburb.stage.measureText(this->text.substr(lineStart, i + 1 - lineStart)).width > this->width) {
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
		int index;
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

			this->text = this->text.substr(0, index) + this->text.substr(index + 1, this->text.size());
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
		int index = 0;
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
			
			if (index == -1) {
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

	void FontEngine::ParseColors() {
		int escapePoint = 0;
		int index = 0;
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

			if (index == -1) {
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
				this->text = this->text.substr(0, index) + this->text.substr(index + 7, this->text.size());
				this->RealignFormatQueue(index, 7);
			}
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
		std::string lowercasePrefix;
		std::transform(prefix.begin(), prefix.end(), prefix.begin(), [](unsigned char c) { return std::tolower(c); });

		if (this->prefixColours[lowercasePrefix]) {
			return sf::Color(this->prefixColours[lowercasePrefix]);
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
		// TODO
	}
}