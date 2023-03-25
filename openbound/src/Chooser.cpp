#include "Chooser.h"
#include "Sburb.h"
#include "AssetManager.h"

constexpr int MIN_WIDTH = 160;

namespace SBURB {
    Chooser::Chooser() {
        this->choosing = false;
        this->choices = {};
        this->choice = 0;
        this->dialogs = {};
        this->time = 0;
    }

    Chooser::~Chooser()
    {
    }

    void Chooser::NextChoice()
    {
		if (this->choices.size() == 0) return;
        this->choice = (this->choice + 1) % this->choices.size();
    }

    void Chooser::PrevChoice()
    {
		if (this->choices.size() == 0) return;
        this->choice = (this->choice - 1 + this->choices.size()) % this->choices.size();
    }

    void Chooser::BeginChoosing(int x, int y)
    {
		int width = MIN_WIDTH;
		int height = 0;
		FontEngine basis = FontEngine();

		for (int i = 0; i < this->choices.size(); i++) {
			width = std::max(width, (int)(this->choices[i]->GetName().size()  + 3) * basis.GetCharWidth()  + 10);
		}

		height = basis.GetLineHeight() * this->choices.size()  + 10;

		Vector2 stageSize = Sburb::GetInstance()->GetViewSize();
		Vector2 stagePos = Sburb::GetInstance()->GetViewPos();


		if (x < stagePos.x + 10) {
			x = stagePos.x + 10;
		}
		if (y < stagePos.y + 10) {
			y = stagePos.y + 10;
		}
		if (x + width > stagePos.x + stageSize.x - 10) {
			x = stagePos.x + stageSize.x - width - 10;
		}
		if (y + height > stagePos.y + stageSize.y - 10) {
			y = stagePos.y + stageSize.y - height - 10;
		}

		this->choosing = true;
		this->choice = 0;
		this->dialogs = {};
		for (int i = 0; i < this->choices.size(); i++) {
			FontEngine curEngine = FontEngine(L" > " + this->choices[i]->GetName());
			curEngine.ShowSubText(0, 1);
			curEngine.SetDimensions(x, y + i * curEngine.GetLineHeight(), curEngine.GetWidth(), curEngine.GetHeight());
			this->dialogs.push_back(curEngine);
		}
    }

    void Chooser::Update()
    {
		if (this->choosing) {
			this->time++;
			int fps = Sburb::GetInstance()->GetFPS();

			for (int i = 0; i < this->dialogs.size(); i++) {
				FontEngine* curDialog = &this->dialogs[i];
				curDialog->ShowSubText(curDialog->GetStart(), curDialog->GetEnd() + 1);

				if (i == this->choice) {
					if (this->time % fps < fps / 2) {
						curDialog->SetStart(2);
					}
					else {
						curDialog->SetStart(0);
					}
					curDialog->SetColor(sf::Color(0xccccccff));
				}
				else {
					curDialog->SetStart(0);
					curDialog->SetColor(sf::Color(0xffffffff));
				}
			}
		}
    }

    void Chooser::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
		if (this->choosing && this->dialogs.size() > 0) {
			int x = 0;
			int y = 0;
			int width = MIN_WIDTH;
			int height = 0, i;

			x = this->dialogs[0].GetX();
			y = this->dialogs[0].GetY() - 1;

			sf::Text textMeasurer;
			for (i = 0; i < this->dialogs.size(); i++) {
				auto font = AssetManager::GetFontByName(this->dialogs[i].GetFontName());
				textMeasurer.setFont(*font->GetAsset().get());
				textMeasurer.setCharacterSize(this->dialogs[i].GetFontSize());
				textMeasurer.setStyle(font->GetStyle());
				textMeasurer.setString(this->dialogs[i].GetLine(0));
				
				width = std::max(width, (int)textMeasurer.getLocalBounds().width + 10);
			}

			height = this->dialogs[0].GetLineHeight() * this->dialogs.size();

			// NOTE: Not super efficient; may be optimized later.
			sf::RectangleShape orangeRectShape(sf::Vector2f(width + 12, height + 13));
			orangeRectShape.setPosition(sf::Vector2f(x - 6, y - 6));
			orangeRectShape.setFillColor(sf::Color(0xff9900ff));

			sf::RectangleShape yellowRectShape(sf::Vector2f(width + 4, height + 5));
			yellowRectShape.setPosition(sf::Vector2f(x - 2, y - 2));
			yellowRectShape.setFillColor(sf::Color(0xffff00ff));

			sf::RectangleShape blackRectShape(sf::Vector2f(width, height));
			blackRectShape.setPosition(sf::Vector2f(x, y));
			blackRectShape.setFillColor(sf::Color(0x000000ff));

			target.draw(orangeRectShape, states);
			target.draw(yellowRectShape, states);
			target.draw(blackRectShape, states);

			for (i = 0; i < this->dialogs.size(); i++) {
				target.draw(this->dialogs[i], states);
			}
		}
    }
}