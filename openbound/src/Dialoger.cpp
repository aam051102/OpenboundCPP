#include "Dialoger.h"
#include "Serializer.h"
#include "AssetManager.h"
#include "Sburb.h"
#include "BatchHandler.h"

namespace SBURB
{
    Dialoger::Dialoger(Vector2 hiddenPos, Vector2 alertPos, Vector2 talkPosLeft, Vector2 talkPosRight, Vector2 spriteStartRight, Vector2 spriteEndRight, Vector2 spriteStartLeft, Vector2 spriteEndLeft, Vector4 alertTextDimensions, Vector4 leftTextDimensions, Vector4 rightTextDimensions, std::wstring type)
    {
        this->name = L"default";
        this->currentDialog = L"";

        this->talking = false;
        this->queue = {};
        this->extraArgs = L"";
        this->dialog = std::make_shared<FontEngine>();

        this->hiddenPos = hiddenPos;
        this->alertPos = alertPos;
        this->talkPosLeft = talkPosLeft;
        this->talkPosRight = talkPosRight;

        this->spriteStartRight = spriteStartRight;
        this->spriteEndRight = spriteEndRight;

        this->spriteStartLeft = spriteStartLeft;
        this->spriteEndLeft = spriteEndLeft;

        this->alertTextDimensions = alertTextDimensions;
        this->leftTextDimensions = leftTextDimensions;
        this->rightTextDimensions = rightTextDimensions;

        this->pos = Vector2(hiddenPos.x, hiddenPos.y);

        this->actor = L"";
        this->dialogSide = L"Left";
        this->graphic = nullptr;
        this->box = nullptr;
        this->defaultBox = nullptr;

        this->type = type;
        this->HandleType();
        this->inPosition = false;

        this->dialogSpriteLeft = nullptr;
        this->dialogSpriteRight = nullptr;
    }

    Dialoger::~Dialoger()
    {
    }

    void Dialoger::HandleType()
    {
        if (this->type == L"social")
        {
            this->hashes = std::make_shared<FontEngine>();
            this->hashes->SetFormatted(false);

            this->choices = {};
        }
    }

    void Dialoger::Nudge()
    {
        if (this->inPosition)
        {
            if (this->dialog->IsShowingAll())
            {
                if (this->dialog->NextBatch())
                {
                    this->dialog->ShowSubText(0, 1);
                }
                else
                {
                    if (this->queue.size() > 0)
                    {
                        this->NextDialog();
                    }
                    else
                    {
                        this->talking = false;
                    }
                }
            }
            else
            {
                this->dialog->ShowAll();
            }
        }
    }

    void Dialoger::SkipAll()
    {
        this->talking = false;
    }

    void Dialoger::StartDialog(std::wstring info)
    {
        this->inPosition = false;
        this->actor = L"";
        this->currentDialog = info;
        this->queue = split(info, L"@");

        for (int i = this->queue.size() - 2; i >= 0; i--)
        {
            std::wstring line = this->queue[i];
            int escapeCount = 0;
            int index = line.size() - 1;

            while (index >= 0 && line[index] == L'/')
            {
                escapeCount++;
                index--;
            }

            if (escapeCount % 2 == 1)
            {
                this->queue[i] += L"@" + this->queue[i + 1];
                this->queue.erase(this->queue.begin() + i + 1);
            }
        }

        if (this->type == L"social")
        {
            this->hashes->SetText(L"");
        }

        std::reverse(this->queue.begin(), this->queue.end());
        this->queue.pop_back();
        this->NextDialog();

        if (this->type == L"social")
        {
            Sburb::GetInstance()->GetButton(L"spadeButton")->StartAnimation(L"state0");
            Sburb::GetInstance()->GetButton(L"heartButton")->StartAnimation(L"state0");

            if (this->actor != L"" && !this->choices[this->currentDialog])
            {
                this->choices[this->currentDialog] = 0;
            }
            else
            {
                if (this->choices[this->currentDialog] == 1)
                {
                    Sburb::GetInstance()->GetButton(L"heartButton")->StartAnimation(L"state1");
                }
                else
                {
                    Sburb::GetInstance()->GetButton(L"spadeButton")->StartAnimation(L"state1");
                }
            }
        }

        this->box->SetX(-this->box->GetWidth());
        this->talking = true;
    }

    void Dialoger::NextDialog()
    {
        std::wstring nextDialog = trim(this->queue.back());
        this->queue.pop_back();

        this->dialog->SetText(nextDialog);
        this->dialog->ShowSubText(0, 0);

        std::wstring prefix = nextDialog.substr(0, nextDialog.find(L" "));
        if (prefix.find(L"~") != std::wstring::npos)
        {
            size_t firstIndex = prefix.find(L"~");
            size_t lastIndex = prefix.size();
            size_t ampIndex = prefix.find(L"%");
            if (ampIndex != std::wstring::npos && ampIndex > firstIndex)
            {
                lastIndex = ampIndex;
            }

            size_t colIndex = prefix.find(L":");
            if (colIndex != std::wstring::npos && colIndex < lastIndex)
            {
                lastIndex = colIndex;
            }

            std::wstring resource = prefix.substr(firstIndex + 1, lastIndex - (firstIndex + 1));
            prefix = prefix.substr(0, firstIndex) + prefix.substr(lastIndex);

            this->graphic = Sburb::GetInstance()->GetSprite(resource);

            if (!this->graphic)
            {
                std::shared_ptr<AssetGraphic> img = AssetManager::GetGraphicByName(resource);
                this->graphic = std::make_shared<Sprite>();
                this->graphic->AddAnimation(std::make_shared<Animation>(L"image", img->GetName(), 0, 0, (int)img->GetAsset()->getSize().x, (int)img->GetAsset()->getSize().y, 0, 1, L"1"));
                this->graphic->StartAnimation(L"image");
            }
        }
        else
        {
            this->graphic = nullptr;
        }

        if (prefix.find(L"%") != std::wstring::npos)
        {
            size_t firstIndex = prefix.find(L"%");
            size_t lastIndex = prefix.size();

            size_t colIndex = prefix.find(L":");

            if (colIndex >= 0 && colIndex < lastIndex)
            {
                lastIndex = colIndex;
            }

            std::wstring resource = prefix.substr(firstIndex + 1, lastIndex - (firstIndex + 1));
            prefix = prefix.substr(0, firstIndex) + prefix.substr(lastIndex);

            this->SetBox(resource);
        }
        else
        {
            this->box = this->defaultBox;
        }

        if (prefix.find(L":") != std::wstring::npos)
        {
            size_t firstIndex = prefix.find(L":");
            size_t lastIndex = prefix.size();

            std::wstring resource = prefix.substr(firstIndex + 1, lastIndex - (firstIndex + 1));
            prefix = prefix.substr(0, firstIndex) + prefix.substr(lastIndex);

            this->extraArgs = resource;
            if (this->type == L"social")
            {
                this->hashes->SetText(trim(replace(replace(this->extraArgs, L"#", L" #"), L"-", L" ")));
            }
        }
        else
        {
            this->extraArgs = L"";
            if (this->type == L"social")
            {
                this->hashes->SetText(L"");
            }
        }

        if (prefix == L"!")
        {
            this->actor = L"";
            this->dialogSide = L"Left";
        }
        else
        {
            std::wstring newActor;
            if (prefix.find(L"_") != std::wstring::npos)
            {
                newActor = prefix.substr(0, prefix.find(L"_"));
            }
            else
            {
                newActor = prefix.substr(0, 2);
            }
            if (this->actor == L"")
            {
                this->dialogSide = L"Left";
                std::shared_ptr<Sprite> sprite = this->DialogOnSide(this->dialogSide);
                Vector2 desiredPos = this->StartOnSide(this->OppositeSide(this->dialogSide));
                sprite->SetX(desiredPos.x);
                sprite->SetY(desiredPos.y);
            }
            else if (this->actor != newActor)
            {
                this->dialogSide = this->OppositeSide(this->dialogSide);
                std::shared_ptr<Sprite> sprite = this->DialogOnSide(this->dialogSide);
                Vector2 desiredPos = this->StartOnSide(this->dialogSide);
                sprite->SetX(desiredPos.x);
                sprite->SetY(desiredPos.y);
            }

            this->actor = newActor;
            this->DialogOnSide(this->dialogSide)->StartAnimation(prefix);
        }
    }

    std::wstring Dialoger::OppositeSide(std::wstring side)
    {
        if (side == L"Left")
        {
            return L"Right";
        }
        else
        {
            return L"Left";
        }
    }

    std::shared_ptr<Sprite> Dialoger::DialogOnSide(std::wstring side)
    {
        if (side == L"Left")
        {
            return this->dialogSpriteLeft;
        }
        else
        {
            return this->dialogSpriteRight;
        }
    }

    Vector2 Dialoger::StartOnSide(std::wstring side)
    {
        if (side == L"Left")
        {
            return this->spriteStartLeft;
        }
        else
        {
            return this->spriteStartRight;
        }
    }

    Vector2 Dialoger::EndOnSide(std::wstring side)
    {
        if (side == L"Left")
        {
            return this->spriteEndLeft;
        }
        else
        {
            return this->spriteEndRight;
        }
    }

    bool Dialoger::MoveToward(Vector2& startPos, Vector2 endPos, int speed)
    {
        if (abs(startPos.x - endPos.x) > speed)
        {
            startPos.x = startPos.x + (speed * abs(endPos.x - startPos.x) / (endPos.x - startPos.x));
        }
        else
        {
            startPos.x = endPos.x;
        }

        if (abs(startPos.y - endPos.y) > speed)
        {
            startPos.y = startPos.y + (speed * abs(endPos.y - startPos.y) / (endPos.y - startPos.y));
        }
        else
        {
            startPos.y = endPos.y;
        }

        return startPos.y == endPos.y && startPos.x == endPos.x;
    }

    bool Dialoger::MoveToward(std::shared_ptr<Sprite> sprite, Vector2 pos, int speed)
    {
        if (abs(sprite->GetX() - pos.x) > speed)
        {
            sprite->SetX(sprite->GetX() + (speed * abs(pos.x - sprite->GetX()) / (pos.x - sprite->GetX())));
        }
        else
        {
            sprite->SetX(pos.x);
        }

        if (abs(sprite->GetY() - pos.y) > speed)
        {
            sprite->SetY(sprite->GetY() + (speed * abs(pos.y - sprite->GetY()) / (pos.y - sprite->GetY())));
        }
        else
        {
            sprite->SetY(pos.y);
        }

        return sprite->GetY() == pos.y && sprite->GetX() == pos.x;
    }

    void Dialoger::Update()
    {
        this->setPosition(sf::Vector2f(Sburb::GetInstance()->GetViewPos().x, Sburb::GetInstance()->GetViewPos().y));

        std::shared_ptr<SpriteButton> closeButton;
        std::shared_ptr<SpriteButton> spadeButton;
        std::shared_ptr<SpriteButton> heartButton;
        std::shared_ptr<SpriteButton> bubbleButton;
        std::shared_ptr<Sprite> hashTagBar;

        if (this->type == L"social")
        {
            closeButton = Sburb::GetInstance()->GetButton(L"closeButton");
            spadeButton = Sburb::GetInstance()->GetButton(L"spadeButton");
            heartButton = Sburb::GetInstance()->GetButton(L"heartButton");
            bubbleButton = Sburb::GetInstance()->GetButton(L"bubbleButton");
            hashTagBar = Sburb::GetInstance()->GetSprite(L"hashTagBar");
        }

        bool init = false;
        if (this->talking)
        {
            Vector2 desiredPos;
            bool ready = true;

            if (this->actor == L"")
            {
                desiredPos = this->alertPos;
                this->inPosition = true;
            }
            else
            {
                desiredPos = this->dialogSide == L"Left" ? this->talkPosLeft : this->talkPosRight;
                ready = this->MoveToward(this->DialogOnSide(this->dialogSide), this->EndOnSide(this->dialogSide));
                this->MoveToward(this->DialogOnSide(this->OppositeSide(this->dialogSide)), this->StartOnSide(this->OppositeSide(this->dialogSide)));
            }

            init = false;

            if (this->MoveToward(this->pos, desiredPos, 110) && ready)
            {

                if (this->dialog->GetStart() == this->dialog->GetEnd())
                {
                    this->inPosition = true;
                    Vector4 dialogDimensions = this->DecideDialogDimensions();
                    this->dialog->SetDimensions(dialogDimensions.x, dialogDimensions.y, dialogDimensions.z, dialogDimensions.w);
                    init = true;
                }
                this->dialog->ShowSubText(this->dialog->GetStart(), this->dialog->GetEnd() + 2);

                if (this->actor != L"")
                {
                    this->DialogOnSide(this->dialogSide)->Update();
                }

                if (this->type == L"social")
                {
                    if (this->queue.size() == 0)
                    {
                        if (this->actor != L"")
                        {
                            spadeButton->Update();
                            heartButton->Update();
                            bubbleButton->Update();
                        }
                    }
                    else
                    {
                        closeButton->Update();
                    }

                    hashTagBar->Update();
                }
            }
            else
            {
                this->inPosition = false;
            }

            if (this->graphic)
            {
                this->graphic->SetX(this->pos.x);
                this->graphic->SetY(this->pos.y);
                this->graphic->Update();
            }
        }
        else
        {
            this->graphic = nullptr;
            this->MoveToward(this->pos, this->hiddenPos, 120);

            if (this->actor != L"")
            {
                if (this->MoveToward(this->DialogOnSide(this->dialogSide), this->StartOnSide(this->OppositeSide(this->dialogSide))))
                {
                    Vector2 pos1 = this->StartOnSide(this->dialogSide);
                    std::shared_ptr<Sprite> sprite1 = this->DialogOnSide(this->dialogSide);
                    sprite1->SetX(pos1.x);
                    sprite1->SetY(pos1.y);

                    Vector2 pos2 = this->StartOnSide(this->OppositeSide(this->dialogSide));
                    std::shared_ptr<Sprite> sprite2 = this->DialogOnSide(this->OppositeSide(this->dialogSide));
                    sprite2->SetX(pos2.x);
                    sprite2->SetY(pos2.y);
                    this->actor = L"";
                }
            }
        }

        this->box->SetX(this->pos.x);
        this->box->SetY(this->pos.y);

        if (this->type == L"social")
        {
            hashTagBar->SetX(this->pos.x);
            hashTagBar->SetY(this->pos.y + this->box->GetHeight());

            if (this->dialogSide == L"Right")
            {
                spadeButton->SetX(hashTagBar->GetX() + 20);
                heartButton->SetX(hashTagBar->GetX() + 60);
                bubbleButton->SetX(hashTagBar->GetX() + 100);
            }
            else
            {
                spadeButton->SetX(hashTagBar->GetX() + hashTagBar->GetAnimation()->GetColSize() - 120);
                heartButton->SetX(hashTagBar->GetX() + hashTagBar->GetAnimation()->GetColSize() - 80);
                bubbleButton->SetX(hashTagBar->GetX() + hashTagBar->GetAnimation()->GetColSize() - 40);
            }

            spadeButton->SetY(hashTagBar->GetY() + 15);
            heartButton->SetY(hashTagBar->GetY() + 15);
            bubbleButton->SetY(hashTagBar->GetY() + 15);

            if (this->actor != L"")
            {
                if (spadeButton->GetAnimation()->GetName() == L"state1")
                {
                    this->choices[this->currentDialog] = -1;
                }
                else if (heartButton->GetAnimation()->GetName() == L"state1")
                {
                    this->choices[this->currentDialog] = 1;
                }
                else
                {
                    this->choices[this->currentDialog] = 0;
                }
            }

            if (init)
            {
                if (this->dialogSide == L"Right")
                {
                    this->hashes->SetDimensions(this->dialog->GetX(), hashTagBar->GetY() + 13, this->dialog->GetWidth(), hashTagBar->GetAnimation()->GetRowSize() - 10);
                }
                else
                {
                    this->hashes->SetDimensions(this->dialog->GetX(), hashTagBar->GetY() + 13, this->dialog->GetWidth(), hashTagBar->GetAnimation()->GetRowSize() - 10);
                }
            }
            if (this->dialog->IsShowingAll() && this->dialog->OnLastBatch())
            {
                this->hashes->ShowAll();
            }
            else
            {
                this->hashes->ShowSubText(0, 0);
            }
        }

        this->box->Update();
    }

    Vector4 Dialoger::DecideDialogDimensions()
    {
        if (this->actor == L"")
        {
            return Vector4(this->pos.x + this->alertTextDimensions.x,
                this->pos.y + this->alertTextDimensions.y,
                this->alertTextDimensions.z,
                this->alertTextDimensions.w);
        }
        else if (this->dialogSide == L"Left")
        {
            return Vector4(this->pos.x + this->leftTextDimensions.x,
                this->pos.y + this->leftTextDimensions.y,
                this->leftTextDimensions.z,
                this->leftTextDimensions.w);
        }
        else
        {
            return Vector4(this->pos.x + this->rightTextDimensions.x,
                this->pos.y + this->rightTextDimensions.y,
                this->rightTextDimensions.z,
                this->rightTextDimensions.w);
        }
    }

    void Dialoger::SetBox(std::wstring box)
    {
        std::shared_ptr<Sprite> dialogBox = Sburb::GetInstance()->GetSprite(box);
        if (!dialogBox)
        {
            std::shared_ptr<AssetGraphic> boxAsset = AssetManager::GetGraphicByName(box);

            dialogBox = std::make_shared<Sprite>(std::wstring(L"dialogBox"), Sburb::GetInstance()->GetViewSize().x + 1, 1000, boxAsset->GetAsset()->getSize().x, boxAsset->GetAsset()->getSize().y, 0, 0, 0);
            dialogBox->AddAnimation(std::make_shared<Animation>(std::wstring(L"image"), boxAsset->GetName(), 0, 0, boxAsset->GetAsset()->getSize().x, boxAsset->GetAsset()->getSize().y, 0, 1, L"1"));
            dialogBox->StartAnimation(L"image");
        }

        if (!this->box)
        {
            this->defaultBox = dialogBox;
        }
        else
        {
            dialogBox->SetX(this->box->GetX());
            dialogBox->SetY(this->box->GetY());
        }

        this->box = dialogBox;
    }

    std::wstring Dialoger::Serialize(std::wstring output)
    {
        output += L"\n<dialoger " +
            Serializer::SerializeAttribute(L"hiddenPos", this->hiddenPos) +
            Serializer::SerializeAttribute(L"alertPos", this->alertPos) +
            Serializer::SerializeAttribute(L"talkPosLeft", this->talkPosLeft) +
            Serializer::SerializeAttribute(L"talkPosRight", this->talkPosRight) +
            Serializer::SerializeAttribute(L"spriteStartRight", this->spriteStartRight) +
            Serializer::SerializeAttribute(L"spriteEndRight", this->spriteEndRight) +
            Serializer::SerializeAttribute(L"spriteStartLeft", this->spriteStartLeft) +
            Serializer::SerializeAttribute(L"spriteEndLeft", this->spriteEndLeft) +
            Serializer::SerializeAttribute(L"alertTextDimensions", this->alertTextDimensions) +
            Serializer::SerializeAttribute(L"leftTextDimensions", this->leftTextDimensions) +
            Serializer::SerializeAttribute(L"rightTextDimensions", this->rightTextDimensions);
        output += Serializer::SerializeAttribute(L"type", this->type);
        output += L"box='" + this->box->GetAnimation()->GetSheet()->GetName() + L"' ";
        output += L">";
        output += L"</dialoger>";
        return output;
    }

    void Dialoger::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
    {
        if (this->type == L"social")
        {
            target.draw(*Sburb::GetInstance()->GetSprite(L"hashTagBar"), states);
        }

        target.draw(*this->box, states);

        // Force box render
        if (BatchHandler::getInstance().BatchExists())
            BatchHandler::getInstance().DrawBatch();

        if (this->graphic)
        {
            target.draw(*this->graphic, states);
        }

        if (this->talking)
        {
            target.draw(*this->dialog, states);

            if (this->type == L"social")
            {
                if (this->queue.size() > 0)
                {
                    target.draw(*Sburb::GetInstance()->GetButton(L"closeButton"), states);
                }

                if (this->dialog->GetStart() != this->dialog->GetEnd())
                {
                    target.draw(*this->hashes, states);

                    if (this->queue.size() == 0 && this->actor != L"")
                    {
                        target.draw(*Sburb::GetInstance()->GetButton(L"spadeButton"), states);
                        target.draw(*Sburb::GetInstance()->GetButton(L"heartButton"), states);
                        target.draw(*Sburb::GetInstance()->GetButton(L"bubbleButton"), states);
                    }
                }
            }
        }

        if (this->actor != L"")
        {
            target.draw(*this->dialogSpriteLeft, states);

            if (this->dialogSpriteRight->GetAnimation())
            {
                this->dialogSpriteRight->GetAnimation()->SetFlipX(true);
            }

            target.draw(*this->dialogSpriteRight, states);
        }
    }
}