#include "Animation.h"
#include "Serializer.h"
#include "AssetManager.h"
#include "BatchHandler.h"
#include "Sburb.h"

namespace SBURB
{
    Animation::Animation(std::wstring name, std::wstring sheetName, int x, int y, int colSize, int rowSize, int startPos, int length, std::wstring frameInterval, int loopNum, std::wstring followUp, bool flipX, bool flipY, bool sliced, int numCols, int numRows)
    {
        this->sheetName = sheetName;
        this->sliced = sliced;
        this->startPos = startPos;
        this->length = length;
        this->curInterval = 0;
        this->curFrame = 0;
        this->name = name;
        this->loopNum = loopNum;
        this->curLoop = 0;
        this->followUp = followUp;

        if (sliced)
        {
            this->numRows = numRows;
            this->numCols = numCols;
            this->rowSize = rowSize;
            this->colSize = colSize;
            this->sheets = {};

            for (int colNum = 0; colNum < this->numCols; colNum++)
            {
                for (int rowNum = 0; rowNum < this->numRows; rowNum++)
                {
                    std::shared_ptr<AssetGraphic> texture = AssetManager::GetGraphicByName(sheetName + L"_" + std::to_wstring(colNum) + L"_" + std::to_wstring(rowNum));

                    if (texture)
                    {
                        if (this->sheets.find(colNum) == this->sheets.end())
                        {
                            this->sheets[colNum] = {};
                        }

                        this->sheets[colNum][rowNum] = texture;
                    }
                }
            }
        }
        else
        {
            this->sheet = AssetManager::GetGraphicByName(sheetName);
            this->rowSize = rowSize ? rowSize : this->sheet->GetAsset()->getSize().y;
            this->colSize = colSize ? colSize : this->sheet->GetAsset()->getSize().x;
            this->numRows = this->sheet->GetAsset()->getSize().y / this->rowSize;
            this->numCols = this->sheet->GetAsset()->getSize().x / this->colSize;
        }

        if (frameInterval == L"")
        {
            this->frameInterval = 1;
        }
        else
        {
            if (frameInterval.find(L":") == std::wstring::npos)
            {
                this->frameInterval = stoi(frameInterval);
            }
            else
            {
                std::vector<std::wstring> intervals = split(frameInterval, L",");
                this->frameIntervals = {};

                for (int i = 0; i < intervals.size(); i++)
                {
                    std::vector<std::wstring> pair = split(intervals[i], L":");
                    this->frameIntervals[stoi(pair[0])] = stoi(pair[1]);
                }
                if (!this->frameIntervals[0])
                {
                    this->frameIntervals[0] = 1;
                }
                this->frameInterval = this->frameIntervals[this->curFrame];
            }
        }

        this->SetX(x);
        this->SetY(y);

        this->SetFlipX(flipX);
        this->SetFlipY(flipY);
    }

    Animation::~Animation()
    {
    }

    void Animation::NextFrame()
    {
        this->curFrame++;

        if (this->curFrame >= this->length)
        {
            if (this->curLoop == this->loopNum)
            {
                this->curFrame = this->length - 1;
            }
            else
            {
                this->curFrame = 0;
                if (this->loopNum >= 0)
                {
                    this->curLoop++;
                }
            }
        }

        if (this->frameIntervals[this->curFrame])
        {
            this->frameInterval = this->frameIntervals[this->curFrame];
        }
    }

    void Animation::Update()
    {
        this->curInterval++;

        while (this->curInterval > this->frameInterval)
        {
            this->curInterval -= this->frameInterval;
            NextFrame();
        }
    }

    void Animation::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
    {
        sf::RenderStates localStates = states;
        localStates.transform *= getTransform();

        if (this->sliced)
        {
            // TODO: Resize rendered slices to be inside of camera view for optimization??? May not be necessary. Already culling the most serious cases
            // TODO: Support flipped slices

            for (int colNum = 0; colNum < this->numCols; colNum++)
            {
                for (int rowNum = 0; rowNum < this->numRows; rowNum++)
                {
                    if (this->sheets.find(colNum) != this->sheets.end() && this->sheets.at(colNum).find(rowNum) != this->sheets.at(colNum).end())
                    {
                        std::shared_ptr<AssetGraphic> sheet = this->sheets.at(colNum).at(rowNum);
                        int frameX = 0;
                        int frameY = 0;
                        int drawWidth = sheet->GetAsset()->getSize().x;
                        int drawHeight = sheet->GetAsset()->getSize().y;
                        int offsetX = colNum * this->colSize;
                        int offsetY = rowNum * this->rowSize;

                        sf::FloatRect transformRect(sf::Vector2f(offsetX, offsetY), sf::Vector2f(drawWidth, drawHeight));
                        transformRect = localStates.transform.transformRect(transformRect);

                        // Cull
                        sf::IntRect cam = sf::IntRect(sf::Vector2(Sburb::GetInstance()->GetViewPos().x, Sburb::GetInstance()->GetViewPos().y), sf::Vector2(Sburb::GetInstance()->GetViewSize().x, Sburb::GetInstance()->GetViewSize().y));

                        if (
                            (transformRect.left + transformRect.width < cam.left || transformRect.left > cam.left + cam.width) ||
                            (transformRect.top + transformRect.height < cam.top || transformRect.top > cam.top + cam.height)
                            ) {
                            continue;
                        }

                        // Render
                        sf::VertexArray arr(sf::PrimitiveType::Triangles, 6);

                        const float leftPos = this->flipX ? (transformRect.left + transformRect.width) : transformRect.left;
                        const float rightPos = this->flipX ? transformRect.left : (transformRect.left + transformRect.width);

                        const float topPos = this->flipY ? (transformRect.top + transformRect.height) : transformRect.top;
                        const float bottomPos = this->flipY ? transformRect.top : (transformRect.top + transformRect.height);

                        arr[0].position = sf::Vector2f(leftPos, topPos);
                        arr[1].position = sf::Vector2f(rightPos, topPos);
                        arr[2].position = sf::Vector2f(leftPos, bottomPos);
                        arr[3].position = sf::Vector2f(leftPos, bottomPos);
                        arr[4].position = sf::Vector2f(rightPos, topPos);
                        arr[5].position = sf::Vector2f(rightPos, bottomPos);

                        arr[0].texCoords = sf::Vector2f(frameX, frameY);
                        arr[1].texCoords = sf::Vector2f(frameX + drawWidth, frameY);
                        arr[2].texCoords = sf::Vector2f(frameX, frameY + drawHeight);
                        arr[3].texCoords = sf::Vector2f(frameX, frameY + drawHeight);
                        arr[4].texCoords = sf::Vector2f(frameX + drawWidth, frameY);
                        arr[5].texCoords = sf::Vector2f(frameX + drawWidth, frameY + drawHeight);

                        arr[0].color = sf::Color::White;
                        arr[1].color = sf::Color::White;
                        arr[2].color = sf::Color::White;
                        arr[3].color = sf::Color::White;
                        arr[4].color = sf::Color::White;
                        arr[5].color = sf::Color::White;

                        BatchHandler::getInstance().DrawSpriteRect(this->sheetName + L"_" + std::to_wstring(colNum) + L"_" + std::to_wstring(rowNum), arr, target);
                    }
                }
            }
        }
        else
        {
            int colNum = ((this->startPos + this->curFrame) % this->numCols);
            int rowNum = (floor((this->startPos + this->curFrame - colNum) / this->numCols));
            int frameX = colNum * this->colSize;
            int frameY = rowNum * this->rowSize;
            int drawWidth = this->colSize;
            int drawHeight = this->rowSize;

            sf::FloatRect transformRect(sf::Vector2f(0, 0), sf::Vector2f(drawWidth, drawHeight));
            transformRect = localStates.transform.transformRect(transformRect);

            if (this->flipX) {
                transformRect.left += -this->x;
                transformRect.left += -this->x;
                transformRect.left -= transformRect.width;
            }

            // Cull
            sf::IntRect cam = sf::IntRect(sf::Vector2(Sburb::GetInstance()->GetViewPos().x, Sburb::GetInstance()->GetViewPos().y), sf::Vector2(Sburb::GetInstance()->GetViewSize().x, Sburb::GetInstance()->GetViewSize().y));

            if (
                (transformRect.left + transformRect.width < cam.left || transformRect.left > cam.left + cam.width) ||
                (transformRect.top + transformRect.height < cam.top || transformRect.top > cam.top + cam.height)
                ) {
                return;
            }

            // Render
            sf::VertexArray arr(sf::PrimitiveType::Triangles, 6);

            const float leftPos = this->flipX ? (transformRect.left + transformRect.width) : transformRect.left;
            const float rightPos = this->flipX ? transformRect.left : (transformRect.left + transformRect.width);

            const float topPos = this->flipY ? (transformRect.top + transformRect.height) : transformRect.top;
            const float bottomPos = this->flipY ? transformRect.top : (transformRect.top + transformRect.height);

            arr[0].position = sf::Vector2f(leftPos, topPos);
            arr[1].position = sf::Vector2f(rightPos, topPos);
            arr[2].position = sf::Vector2f(leftPos, bottomPos);
            arr[3].position = sf::Vector2f(leftPos, bottomPos);
            arr[4].position = sf::Vector2f(rightPos, topPos);
            arr[5].position = sf::Vector2f(rightPos, bottomPos);

            arr[0].texCoords = sf::Vector2f(frameX, frameY);
            arr[1].texCoords = sf::Vector2f(frameX + drawWidth, frameY);
            arr[2].texCoords = sf::Vector2f(frameX, frameY + drawHeight);
            arr[3].texCoords = sf::Vector2f(frameX, frameY + drawHeight);
            arr[4].texCoords = sf::Vector2f(frameX + drawWidth, frameY);
            arr[5].texCoords = sf::Vector2f(frameX + drawWidth, frameY + drawHeight);

            arr[0].color = sf::Color::White;
            arr[1].color = sf::Color::White;
            arr[2].color = sf::Color::White;
            arr[3].color = sf::Color::White;
            arr[4].color = sf::Color::White;
            arr[5].color = sf::Color::White;

            BatchHandler::getInstance().DrawSpriteRect(this->sheetName, arr, target);
        }
    }

    void Animation::Reset()
    {
        this->curFrame = 0;
        this->curInterval = 0;
        this->curLoop = 0;
    }

    bool Animation::HasPlayed()
    {
        return this->curLoop == this->loopNum && this->curFrame == this->length - 1;
    }

    bool Animation::IsVisuallyUnder(int x, int y)
    {
        if (x >= this->x && x <= this->x + this->colSize)
        {
            if (y >= this->y && y <= this->y + this->rowSize)
            {
                return true;
            }
        }

        return false;
    }

    void Animation::SetColSize(int newSize)
    {
        this->colSize = newSize;
        this->numCols = this->sheet->GetAsset()->getSize().x / this->colSize;
        Reset();
    }

    void Animation::SetRowSize(int newSize)
    {
        this->rowSize = newSize;
        this->numRows = this->sheet->GetAsset()->getSize().y / this->rowSize;
        Reset();
    }

    void Animation::SetX(int newX)
    {
        this->x = newX;
        this->setPosition(sf::Vector2f(this->x, this->y));
    };

    void Animation::SetY(int newY)
    {
        this->y = newY;
        this->setPosition(sf::Vector2f(this->x, this->y));
    };

    void Animation::SetFlipX(bool newFlipX)
    {
        this->flipX = newFlipX;
    };

    void Animation::SetFlipY(bool newFlipY)
    {
        this->flipY = newFlipY;
    };

    std::shared_ptr<Animation> Animation::Clone(int x, int y)
    {
        return std::make_shared<Animation>(this->name, this->sheetName, x + this->x, y + this->y, this->colSize, this->rowSize, this->startPos, this->length, std::to_wstring(this->frameInterval), this->loopNum, this->followUp, this->flipX, this->flipY, this->sliced, this->numCols, this->numRows);
    }

    std::wstring Animation::Serialize(std::wstring output)
    {
        std::wstring frameInterval = L"";
        bool firstInterval = true;

        if (!this->frameIntervals.empty())
        {
            for (std::pair<int, int> interval : this->frameIntervals)
            {
                frameInterval = frameInterval + (firstInterval ? L"" : L",") + std::to_wstring(interval.first) + L":" + std::to_wstring(interval.second);
                firstInterval = false;
            }
        }
        else if (this->frameInterval != 1)
        {
            frameInterval = this->frameInterval;
        }

        output = output + L"\n<animation " +
            (L"sheet='" + this->sheetName + L"' ") +
            ((this->name != L"image") ? L"name='" + this->name + L"' " : L"") +
            Serializer::SerializeAttribute(L"x", this->x) +
            Serializer::SerializeAttribute(L"y", this->y) +
            ((this->rowSize != this->sheet->GetAsset()->getSize().y) ? L"rowSize='" + std::to_wstring(this->rowSize) + L"' " : L"") +
            ((this->colSize != this->sheet->GetAsset()->getSize().x) ? L"colSize='" + std::to_wstring(this->colSize) + L"' " : L"") +
            Serializer::SerializeAttribute(L"startPos", this->startPos) +
            ((this->length != 1) ? L"length='" + std::to_wstring(this->length) + L"' " : L"") +
            ((frameInterval != L"") ? L"frameInterval='" + frameInterval + L"' " : L"") +
            ((this->loopNum != -1) ? L"loopNum='" + std::to_wstring(this->loopNum) + L"' " : L"") +
            Serializer::SerializeAttribute(L"followUp", this->followUp) +
            Serializer::SerializeAttribute(L"flipX", this->flipX) +
            Serializer::SerializeAttribute(L"flipY", this->flipY) +
            (this->sliced ? (L"sliced='true' numCols='" + std::to_wstring(this->numCols) + L"' numRows='" + std::to_wstring(this->numRows) + L"' ") : (L"")) +
            L" />";

        return output;
    }
}