#include "Serializer.h"
#include "Sburb.h"
#include "Logger.h"
#include "Parser.h"
#include "AssetManager.h"
#include "AssetPath.h"
#include "AssetMovie.h"
#include "AssetGraphic.h"
#include "AssetAudio.h"
#include "AssetFont.h"
#include "AssetText.h"
#include <filesystem>
#include <thread>

namespace SBURB
{
    static std::map<std::wstring, pugi::xml_node> templateClasses;
    static int loadingDepth = 0;
    static std::vector<pugi::xml_node> loadQueue;
    static pugi::xml_document templateDoc;

    std::wstring Serializer::Serialize()
    {
        Sburb* sburbInst = Sburb::GetInstance();

        std::wstring loadedFiles = L"";
        bool loadedFilesExist = false;

        /*for (auto key : sburbInst->loadedFiles)
        {
            loadedFiles = loadedFiles + (loadedFilesExist ? L"," : L"") + key;
            loadedFilesExist = true;
        }*/

        auto character = sburbInst->GetCharacter();
        auto bgm = sburbInst->GetBGM();

        std::wstring output = L"<sburb" +
            std::wstring(L" char='") + character->GetName() +
            (bgm ? L"' bgm='" + bgm->GetName() + (bgm->GetStartLoop() ? L"," + std::to_wstring(bgm->GetStartLoop()) : L"") : L"") +
            (sburbInst->GetScale().x != 1 ? L"' scale='" + sburbInst->GetScale().x : L"") +
            (sburbInst->GetNextQueueId() > 0 ? L"' nextQueueId='" + sburbInst->GetNextQueueId() : L"") +
            (sburbInst->resourcePath != L"" ? (L"' resourcePath='" + sburbInst->resourcePath) : L"") +
            (sburbInst->levelPath != L"" ? (L"' levelPath='" + sburbInst->levelPath) : L"") +
            (loadedFilesExist ? (L"' loadedFiles='" + loadedFiles) : L"") +
            L"'>\n";
        output = Serializer::SerializeAssets(output);
        output = Serializer::SerializeTemplates(output);
        output = Serializer::SerializeHud(output);
        output = Serializer::SerializeLooseObjects(output);
        output = Serializer::SerializeRooms(output);
        output = Serializer::SerializeGameState(output);
        output = Serializer::SerializeActionQueues(output);

        output = output + L"\n</sburb>";

        return output;
    }

    std::wstring EncodeXML(std::wstring str)
    {
        return replace(replace(replace(replace(str, L"&", L"&amp;"), L"<", L"&lt;"), L">", L"&gt;"), L"\"", L"&quot;");
    };

    std::wstring Serializer::SerializeGameState(std::wstring output)
    {
        output = output + L"\n<gameState>\n";

        for (auto state : Sburb::GetInstance()->GetGameState())
        {
            output = output + L"  <" + state.first + L">" + EncodeXML(state.second) + L"</" + state.first + L">";
        }

        output = output + L"\n</gameState>\n";

        return output;
    }

    std::wstring Serializer::SerializeActionQueues(std::wstring output)
    {
        output = output + L"<actionQueues>";

        for (auto actionQueue : Sburb::GetInstance()->GetActionQueues())
        {
            if (actionQueue->GetCurrentAction())
            {
                output = actionQueue->Serialize(output);
            }
        }
        output = output + L"\n</actionQueues>\n";

        return output;
    }

    std::wstring Serializer::SerializeAssets(std::wstring output)
    {
        output = output + L"\n<assets>";
        /*
        for (auto asset : assets)
        {
            std::shared_ptr<Asset> curAsset = assets[asset];
            std::wstring innerHTML = L"";

            if (curAsset->GetType() == L"graphic")
            {

                innerHTML += std::static_pointer_cast<AssetGraphic>(curAsset)->GetPath();
            }
            else if (curAsset->GetType() == L"audio")
            {
                bool firstSrc = false;
                for (auto srcVal : std::static_pointer_cast<AssetAudio>(curAsset)->GetSources())
                {
                    innerHTML += (firstSrc ? L";" : L"") + srcVal;

                    firstSrc = true;
                }
            }
            else if (curAsset->GetType() == L"path")
            {
                auto pathAsset = std::static_pointer_cast<AssetPath>(curAsset);
                for (int i = 0; i < pathAsset->GetPoints().size(); i++)
                {
                    innerHTML = innerHTML + std::to_string(pathAsset->GetPoints()[i].x) + L"," + std::to_string(pathAsset->GetPoints()[i].y);
                    if (i != pathAsset->GetPoints().size() - 1)
                    {
                        innerHTML = innerHTML + L";";
                    }
                }
            }
            else if (curAsset->GetType() == L"movie")
            {
                innerHTML += std::static_pointer_cast<AssetMovie>(curAsset)->GetPath();
            }
            else if (curAsset->GetType() == L"font")
            {
                // Pretty sure this was broken originally. May need to look more into it.
                innerHTML += std::static_pointer_cast<AssetFont>(curAsset)->GetSources()[0];
            }
            else if (curAsset->GetType() == L"text")
            {
                innerHTML += escape(trim(std::static_pointer_cast<AssetText>(curAsset)->GetText()).c_str());
            }

            output = output + L"\n<asset name='" + curAsset->GetName() + L"' type='" + curAsset->GetType() + L"' L";

            output = output + L" >";
            output = output + innerHTML;
            output = output + L"</asset>";
        }
        */
        output = output + L"\n</assets>\n";
        output = output + L"\n<effects>";

        for (auto effect : Sburb::GetInstance()->GetEffects())
        {
            output = effect.second->Serialize(output);
        }

        output = output + L"\n</effects>\n";
        return output;
    }

    std::wstring Serializer::SerializeTemplates(std::wstring output)
    {
        output = output + L"\n<classes>";

        std::wostringstream serializeStream;

        for (auto templateNode : templateClasses)
        {
            serializeStream.str(L"");
            templateNode.second.print(serializeStream, L"", pugi::format_raw);
            output += serializeStream.str();
        }

        output = output + L"\n</classes>\n";
        return output;
    }

    std::wstring Serializer::SerializeHud(std::wstring output)
    {
        output = output + L"\n<hud>";

        // TODO: STOP DOING THIS! IT REGISTERS AS A REMOVAL OF SHARED_PTR COUNT, BUT NOT AN ADDITION, SO THEY GET REMOVED EARLY. FIX EVERYWHERE
        /*for (auto content : Sburb::GetInstance()->GetHud())
        {
            output = content.second->Serialize(output);
        }*/

        output = Sburb::GetInstance()->GetDialoger()->Serialize(output);

        auto animations = Sburb::GetInstance()->GetDialoger()->GetDialogSpriteLeft()->GetAnimations();
        output = output + L"\n<dialogsprites>";

        for (auto animation : animations)
        {
            output = animation.second->Serialize(output);
        }

        output = output + L"\n</dialogsprites>";
        output = output + L"\n</hud>\n";
        return output;
    }

    std::wstring Serializer::SerializeLooseObjects(std::wstring output)
    {
        for (auto sprite : Sburb::GetInstance()->GetSprites())
        {
            auto theSprite = sprite.second;
            bool contained = false;

            for (auto room : Sburb::GetInstance()->GetRooms())
            {
                if (room.second->Contains(theSprite))
                {
                    contained = true;
                    break;
                }
            }

            if (!contained)
            {
                // TODO: Figure out why dialogBox isn't defined. Why is a definition attempt even made?
                if (!theSprite)
                {
                    std::wcout << sprite.first << std::endl;
                    continue;
                }
                output = theSprite->Serialize(output);
            }
        }

        /*for (auto button : Sburb::GetInstance()->GetButtons())
        {
            auto theButton = button.second;

            if (!Sburb::GetInstance()->GetHud(theButton->GetName()))
            {
                output = theButton->Serialize(output);
            }
        }*/

        return output;
    }

    std::wstring Serializer::SerializeRooms(std::wstring output)
    {
        output = output + L"\n<rooms>\n";

        for (auto room : Sburb::GetInstance()->GetRooms())
        {
            output = room.second->Serialize(output);
        }

        output = output + L"\n</rooms>\n";

        return output;
    }

    bool Serializer::LoadSerialFromXML(std::wstring path, bool keepOld)
    {
        Sburb::GetInstance()->HaltUpdateProcess();
        path = Sburb::GetInstance()->basePath + Sburb::GetInstance()->levelPath + path;

        if (keepOld && Sburb::GetInstance()->CheckFileIsLoaded(path))
        {
            Sburb::GetInstance()->StartUpdateProcess();
            return true;
        }
        else
        {
            Sburb::GetInstance()->SetFileIsLoaded(path);
        }

        pugi::xml_document doc;
        pugi::xml_parse_result initDocRes = doc.load_file(path.c_str());

        if (initDocRes.status != pugi::status_ok)
        {
            std::string errMsg = "For " + std::string(path.begin(), path.end()) + ": " + initDocRes.description();
            GlobalLogger->Log(Logger::Error, errMsg);
            return false;
        }

        return Serializer::LoadSerial(&doc, keepOld);
    }

    bool Serializer::LoadSerialFromXMLMemory(std::wstring memory, bool keepOld)
    {
        //Sburb::GetInstance()->HaltUpdateProcess();

        pugi::xml_document doc;
        pugi::xml_parse_result initDocRes = doc.load_string(memory.c_str());

        if (initDocRes.status != pugi::status_ok)
        {
            std::string errMsg = initDocRes.description();
            GlobalLogger->Log(Logger::Error, errMsg);
            return false;
        }

        return Serializer::LoadSerial(&doc, keepOld);
    }

    // IS THIS DOC KEPT ALIVE? PROBABLY NOT!
    pugi::xml_document Serializer::ParseXML(std::wstring inText)
    {
        pugi::xml_document doc;
        doc.load_string(inText.c_str());
        return doc;
    }

    void PurgeAssets()
    {
        AssetManager::ClearFonts();
        AssetManager::ClearPaths();
        AssetManager::ClearAudio();
        AssetManager::ClearGraphics();
        AssetManager::ClearText();
        AssetManager::ClearMovies();
        AssetManager::ClearFilePaths();
    }

    bool Serializer::LoadSerial(pugi::xml_document* doc, bool keepOld)
    {
        pugi::xml_node rootNode = doc->child(L"sburb");

        if (!keepOld)
        {
            templateDoc.reset();
            PurgeAssets();
            Sburb::GetInstance()->PurgeState();

            // Load default fonts, if they exist
            auto fontVerdana = std::make_shared<AssetFont>(L"Verdana", std::vector({ std::wstring(L"local:Verdana") }));
            AssetManager::LoadAsset(fontVerdana);


            // TODO: Load fallback fonts
            auto _A = std::make_shared<AssetFont>(L"Meiryo", std::vector({ std::wstring(L"local:Meiryo") }));
            AssetManager::LoadAsset(_A);


        }

        std::wstring levelPath = rootNode.attribute(L"levelPath").as_string();
        if (levelPath != L"")
        {
            if (levelPath[levelPath.length() - 1] == '/')
            {
                Sburb::GetInstance()->levelPath = levelPath;
            }
            else
            {
                Sburb::GetInstance()->levelPath = levelPath + L"/";
            }
        }

        std::wstring resourcePath = rootNode.attribute(L"resourcePath").value();
        if (resourcePath != L"")
        {
            Sburb::GetInstance()->resourcePath = resourcePath;
        }

        std::wstring name = rootNode.attribute(L"name").value();
        if (name != L"")
            Sburb::GetInstance()->SetName(name);

        std::wstring version = rootNode.attribute(L"version").value();
        if (version != L"")
            Sburb::GetInstance()->version = version;

        std::wstring width = rootNode.attribute(L"width").value();
        if (width != L"")
        {
            Sburb::GetInstance()->SetDimensions(stoi(width), Sburb::GetInstance()->GetViewSize().y);
        }

        std::wstring height = rootNode.attribute(L"height").value();
        if (height != L"")
        {
            Sburb::GetInstance()->SetDimensions(Sburb::GetInstance()->GetViewSize().y, stoi(height));
        }

        loadingDepth++;
        LoadDependencies(rootNode);
        loadingDepth--;
        LoadSerialAssets(rootNode);
        loadQueue.push_back(rootNode);
        LoadSerialState();

        return true;
    }

    bool Serializer::LoadDependencies(pugi::xml_node node)
    {
        pugi::xml_node dependenciesNode = node.child(L"dependencies");

        if (dependenciesNode)
        {
            auto dependencyNodes = GetNestedChildren(&dependenciesNode, L"dependency");

            for (pugi::xml_node dependencyNode : dependencyNodes)
            {
                auto dependencyPath = dependencyNode.text().as_string();

                LoadSerialFromXML(trim(dependencyPath), true);
            }
        }

        return true;
    }

    bool Serializer::LoadSerialAssets(pugi::xml_node node)
    {
        std::wstring description = node.attribute(L"description").value();
        if (description != L"")
        {
            Sburb::GetInstance()->SetDescription(description);
        }
        else
        {
            Sburb::GetInstance()->SetDescription(L"assets");
        }

        pugi::xml_node assetsNode = node.child(L"assets");

        if (assetsNode)
        {
            auto assetNodes = GetNestedChildren(&assetsNode, L"asset");
            AssetManager::AddToTotalAssets(assetNodes.size());

            for (pugi::xml_node assetNode : assetNodes)
            {
                /*if (!AssetManager::CheckIsLoaded(assetNode.attribute(L"name").value()))
                {*/
                LoadSerialAsset(assetNode);
                // }
            }
        }

        return true;
    }

    void Serializer::LoadSerialAsset(pugi::xml_node node)
    {
        auto newAsset = ParseSerialAsset(node);

        AssetManager::LoadAsset(newAsset);
    }

    std::shared_ptr<Asset> Serializer::ParseSerialAsset(pugi::xml_node node)
    {
        std::wstring name = node.attribute(L"name").value();
        std::wstring type = node.attribute(L"type").value();
        std::wstring value = trim(node.text().as_string());

        std::wstring blobUrlsAttr = node.attribute(L"blob-urls").value();
        std::vector<std::wstring> blobUrls = {};

        if (blobUrlsAttr != L"")
        {
            blobUrls = split(blobUrlsAttr, L";");
        }

        std::shared_ptr<Asset> asset;

        if (type == L"graphic")
        {
            asset = std::make_shared<AssetGraphic>(name, value);
        }
        else if (type == L"audio")
        {
            asset = std::make_shared<AssetAudio>(name, split(value, L";"));
        }
        else if (type == L"path")
        {
            std::vector<std::wstring> splitPoints = split(value, L";");

            std::vector<Vector2> points;
            for (auto point : splitPoints)
            {
                auto pointCoords = split(point, L",");
                points.push_back(Vector2(stoi(pointCoords[0]), stoi(pointCoords[1])));
            }

            asset = std::make_shared<AssetPath>(name, points);
        }
        else if (type == L"movie")
        {
            asset = std::make_shared<AssetMovie>(name, value);
        }
        else if (type == L"font")
        {
            asset = std::make_shared<AssetFont>(name, split(value, L","));
        }
        else if (type == L"text")
        {
            asset = std::make_shared<AssetText>(name, value);
        }

        return asset;
    }

    void Serializer::LoadSerialState()
    {
        // NOTE: USED TO HAVE A BIG LOOP HERE. NOT SURE WHAT TO DO ABOUT IT???
        // TODO: MAYBE ADD THE LOOP BACK?

        while (loadQueue.size() > 0)
        {
            pugi::xml_node input = loadQueue[0];
            loadQueue.erase(loadQueue.begin() + 0);

            // These two have to be first
            ParseTemplateClasses(input);
            ApplyTemplateClasses(input);

            ParseButtons(input);
            ParseSprites(input);
            ParseCharacters(input);
            ParseFighters(input);
            ParseRooms(input);
            ParseGameState(input);

            ParseHud(input);
            ParseEffects(input);

            // Should be last
            ParseState(input);

            // Relies on Sburb.nextQueueId being set when no Id is provided
            ParseActionQueues(input);
        }

        if (loadQueue.size() == 0 && loadingDepth == 0)
        {
            Sburb::GetInstance()->StartUpdateProcess();
        }
    }

    void Serializer::ParseTemplateClasses(pugi::xml_node node)
    {
        auto classes = node.child(L"classes");

        if (classes)
        {
            auto templates = classes.children();

            for (pugi::xml_node templateNode : templates)
            {
                if (std::wstring(templateNode.name()) != L"#text" && std::wstring(templateNode.name()) != L"#comment")
                {
                    ApplyTemplateClasses(templateNode);

                    pugi::xml_node templateCopyNode = templateDoc.append_copy(templateNode);
                    templateClasses[templateCopyNode.attribute(L"class").as_string()] = templateCopyNode;
                }
            }

            node.remove_child(classes);
        }
    }

    void Serializer::ApplyTemplateClasses(pugi::xml_node node)
    {
        for (auto templateNode : templateClasses)
        {
            auto candidates = GetNestedChildren(&node, templateNode.second.name());

            for (pugi::xml_node candidate : candidates)
            {
                TryToApplyTemplate(templateNode.second, candidate);
            }
        }
    }

    void Serializer::TryToApplyTemplate(pugi::xml_node templateNode, pugi::xml_node candidateNode)
    {
        std::wstring templateClass = templateNode.attribute(L"class").as_string();
        std::wstring candClass = candidateNode.attribute(L"class").as_string();

        if (candClass != L"" && candClass == templateClass)
        {
            Serializer::ApplyTemplate(templateNode, candidateNode);
        }
    }

    void Serializer::ApplyTemplate(pugi::xml_node templateNode, pugi::xml_node candidateNode)
    {

        auto tempChildren = templateNode.children();
        auto candChildren = candidateNode.children();

        for (auto tempAttribute : templateNode.attributes())
        {
            if (std::wstring(candidateNode.attribute(tempAttribute.name()).as_string()) == L"")
            {
                candidateNode.append_attribute(tempAttribute.name()).set_value(tempAttribute.as_string());
            }
        }

        for (auto tempChild : tempChildren)
        {
            candidateNode.append_copy(tempChild);
        }
    }

    void Serializer::ParseButtons(pugi::xml_node node)
    {
        auto newButtons = GetNestedChildren(&node, L"spritebutton");

        for (pugi::xml_node curButton : newButtons)
        {
            auto newButton = Parser::ParseSpriteButton(curButton);
            Sburb::GetInstance()->SetButton(newButton->GetName(), newButton);
        }
    }

    void Serializer::ParseSprites(pugi::xml_node node)
    {
        auto newSprites = GetNestedChildren(&node, L"sprite");

        for (pugi::xml_node curSprite : newSprites)
        {
            auto newSprite = Parser::ParseSprite(curSprite);
            Sburb::GetInstance()->SetSprite(newSprite->GetName(), newSprite);
            ParseActions(curSprite, newSprite);
        }
    }

    void Serializer::ParseActions(pugi::xml_node spriteNode, std::shared_ptr<Sprite> sprite)
    {
        auto newActions = spriteNode.children();
        for (pugi::xml_node curAction : newActions)
        {
            if (std::wstring(curAction.name()) == L"#text")
            {
                continue;
            }

            if (std::wstring(curAction.name()) == L"action")
            {
                auto newAction = Parser::ParseAction(curAction);
                sprite->AddAction(newAction);
            }
        }
    }

    void Serializer::ParseCharacters(pugi::xml_node node)
    {
        auto newChars = GetNestedChildren(&node, L"character");

        for (pugi::xml_node curChar : newChars)
        {
            auto newChar = Parser::ParseCharacter(curChar);
            Sburb::GetInstance()->SetSprite(newChar->GetName(), std::static_pointer_cast<Sprite>(newChar));
            ParseActions(curChar, newChar);
        }
    }

    void Serializer::ParseFighters(pugi::xml_node node)
    {
        auto newFighters = GetNestedChildren(&node, L"fighter");

        for (pugi::xml_node curFighter : newFighters)
        {
            auto newFighter = Parser::ParseFighter(curFighter);
            Sburb::GetInstance()->SetSprite(newFighter->GetName(), std::static_pointer_cast<Sprite>(newFighter));
            ParseActions(curFighter, newFighter);
        }
    }

    void Serializer::ParseRooms(pugi::xml_node node)
    {
        auto newRooms = GetNestedChildren(&node, L"room");

        for (pugi::xml_node curRoom : newRooms)
        {
            auto newRoom = Parser::ParseRoom(curRoom);
            Sburb::GetInstance()->SetRoom(newRoom->GetName(), newRoom);
        }
    }

    void Serializer::ParseGameState(pugi::xml_node node)
    {
        auto newGameStates = GetNestedChildren(&node, L"gameState");

        for (pugi::xml_node curGameState : newGameStates)
        {
            auto children = curGameState.children();

            for (pugi::xml_node child : children)
            {
                if (child.type() == pugi::xml_node_type::node_pcdata)
                    continue;

                std::wstring key = node.name();
                std::wstring value = node.first_child().value();
                Sburb::GetInstance()->SetGameState(key, value);
            }
        }
    }

    void Serializer::ParseHud(pugi::xml_node node)
    {
        pugi::xml_node hudNode = GetNestedChild(&node, L"hud");

        if (hudNode)
        {
            auto children = hudNode.children();

            for (pugi::xml_node child : children)
            {
                if (std::wstring(child.name()) == L"spritebutton")
                {
                    std::wstring name = child.attribute(L"name").as_string();
                    Sburb::GetInstance()->SetHud(name, Sburb::GetInstance()->GetButton(name));
                }
            }
        }

        Serializer::ParseDialoger(node);
        Serializer::ParseDialogSprites(node);
    }

    void Serializer::ParseDialoger(pugi::xml_node node)
    {
        auto dialogerNode = GetNestedChild(&node, L"dialoger");

        if (dialogerNode)
        {
            std::shared_ptr<Sprite> dialogSpriteLeft = nullptr;
            std::shared_ptr<Sprite> dialogSpriteRight = nullptr;

            std::shared_ptr<Dialoger> oldDialoger = Sburb::GetInstance()->GetDialoger();

            if (oldDialoger)
            {
                dialogSpriteLeft = oldDialoger->GetDialogSpriteLeft();
                dialogSpriteRight = oldDialoger->GetDialogSpriteRight();
            }

            Sburb::GetInstance()->SetDialoger(Parser::ParseDialoger(dialogerNode));
            Sburb::GetInstance()->GetDialoger()->SetDialogSpriteLeft(dialogSpriteLeft);
            Sburb::GetInstance()->GetDialoger()->SetDialogSpriteRight(dialogSpriteRight);
        }
    }

    void Serializer::ParseDialogSprites(pugi::xml_node node)
    {
        pugi::xml_node hudNode = node.child(L"hud");

        if (hudNode)
        {
            auto dialogSprites = hudNode.child(L"dialogsprites");

            if (dialogSprites)
            {
                Serializer::SerialLoadDialogSprites(dialogSprites);
            }
        }
    }

    void Serializer::ParseEffects(pugi::xml_node node)
    {
        auto effects = node.child(L"effects");

        if (effects)
        {
            Serializer::SerialLoadEffects(effects);
        }
    }

    void Serializer::ParseState(pugi::xml_node node)
    {
        std::wstring character = node.attribute(L"char").as_string();
        if (character != L"")
        {
            Sburb::GetInstance()->SetCharacter(std::static_pointer_cast<Character>(Sburb::GetInstance()->GetSprite(character)));
            Sburb::GetInstance()->SetFocus(Sburb::GetInstance()->GetCharacter());
            Sburb::GetInstance()->GetCharacter()->BecomePlayer();
        }

        std::wstring mode = node.attribute(L"mode").as_string();
        if (mode != L"")
        {
            Sburb::GetInstance()->SetEngineMode(mode);
        }

        int scale = node.attribute(L"scale").as_int();
        if (scale)
        {
            Sburb::GetInstance()->SetScale(Vector2(scale, scale));
        }

        int nextQueueId = node.attribute(L"nextQueueId").as_int();
        if (nextQueueId)
        {
            Sburb::GetInstance()->SetNextQueueId(nextQueueId);
        }

        std::wstring curRoom = node.attribute(L"curRoom").as_string();
        if (curRoom != L"")
        {
            Sburb::GetInstance()->SetCurrentRoom(Sburb::GetInstance()->GetRoom(curRoom));
            Sburb::GetInstance()->GetCurrentRoom()->Enter();
        }
        else if (Sburb::GetInstance()->GetCurrentRoom() == nullptr && Sburb::GetInstance()->GetCharacter() != nullptr)
        {
            for (auto room : Sburb::GetInstance()->GetRooms())
            {
                if (room.second->Contains(Sburb::GetInstance()->GetCharacter()))
                {
                    Sburb::GetInstance()->SetCurrentRoom(room.second);
                    Sburb::GetInstance()->GetCurrentRoom()->Enter();
                    break;
                }
            }
        }

        std::wstring bgm = node.attribute(L"bgm").as_string();
        if (bgm != L"")
        {
            std::vector<std::wstring> params = split(bgm, L",");
            Sburb::GetInstance()->ChangeBGM(std::make_shared<Music>(params[0], std::stod(params.size() > 1 ? params[1] : L"0") * 1000));
        }

        std::shared_ptr<Action> initAction;
        std::wstring initActionName = node.attribute(L"startAction").as_string();

        if (initActionName != L"")
        {
            for (pugi::xml_node tmp : node.children())
            {
                if (std::wstring(tmp.name()) == L"action" && tmp.attribute(L"name").as_string() == initActionName)
                {
                    initAction = Parser::ParseAction(tmp);
                    continue;
                }
            }

            if (initAction)
            {
                Sburb::GetInstance()->PerformAction(initAction);
            }
        }
    }

    void Serializer::ParseActionQueues(pugi::xml_node node)
    {
        auto element = node.child(L"actionQueues");
        if (!element)
        {
            return;
        }

        auto newActionQueues = element.children();
        for (pugi::xml_node curActionQueues : newActionQueues)
        {
            if (std::wstring(curActionQueues.name()) == L"#text")
            {
                continue;
            }

            auto actionQueue = Parser::ParseActionQueue(curActionQueues);
            Sburb::GetInstance()->AddActionQueue(actionQueue);
        }
    }

    void Serializer::SerialLoadDialogSprites(pugi::xml_node dialogSprites)
    {
        auto dialoger = Sburb::GetInstance()->GetDialoger();
        if (!dialoger)
        {
            Sburb::GetInstance()->SetDialoger(std::make_shared<Dialoger>(Vector2(0, 0), Vector2(0, 0), Vector2(0, 0), Vector2(0, 0), Vector2(0, 0), Vector2(0, 0), Vector2(0, 0), Vector2(0, 0), Vector4(0, 0, 0, 0), Vector4(0, 0, 0, 0), Vector4(0, 0, 0, 0), L"standard"));
            dialoger = Sburb::GetInstance()->GetDialoger();
        }

        if (!dialoger->GetDialogSpriteLeft())
        {
            dialoger->SetDialogSpriteLeft(std::make_shared<Sprite>(L"dialogSprite", -1000, Sburb::GetInstance()->GetViewSize().y, 0, 0));
            dialoger->SetDialogSpriteRight(std::make_shared<Sprite>(L"dialogSprite", Sburb::GetInstance()->GetViewSize().x + 1000, Sburb::GetInstance()->GetViewSize().y, 0, 0));
        }

        auto animations = GetNestedChildren(&dialogSprites, L"animation");
        for (pugi::xml_node anim : animations)
        {
            dialoger->GetDialogSpriteLeft()->AddAnimation(Parser::ParseAnimation(anim));
            dialoger->GetDialogSpriteRight()->AddAnimation(Parser::ParseAnimation(anim));
        }
    }

    void Serializer::SerialLoadEffects(pugi::xml_node effectsNode)
    {
        auto animations = GetNestedChildren(&effectsNode, L"animation");
        for (pugi::xml_node anim : animations)
        {
            auto newEffect = Parser::ParseAnimation(anim);
            Sburb::GetInstance()->SetEffect(newEffect->GetName(), newEffect);
        }
    }

    void Serializer::SerialLoadRoomSprites(std::shared_ptr<Room> newRoom, std::vector<pugi::xml_node> roomSprites)
    {
        for (pugi::xml_node curSprite : roomSprites)
        {
            std::shared_ptr<Sprite> actualSprite = Sburb::GetInstance()->GetSprite(curSprite.attribute(L"name").as_string());
            newRoom->AddSprite(actualSprite);
        }
    }

    void Serializer::SerialLoadRoomPaths(std::shared_ptr<Room> newRoom, pugi::xml_node pathsNode)
    {
        auto walkables = GetNestedChildren(&pathsNode, L"walkable");
        for (pugi::xml_node node : walkables)
        {
            newRoom->AddWalkable(AssetManager::GetPathByName(node.attribute(L"path").as_string()));
        }

        auto unwalkables = GetNestedChildren(&pathsNode, L"unwalkable");
        for (pugi::xml_node node : unwalkables)
        {
            newRoom->AddUnwalkable(AssetManager::GetPathByName(node.attribute(L"path").as_string()));
        }

        auto motionPaths = GetNestedChildren(&pathsNode, L"motionpath");
        for (pugi::xml_node node : motionPaths)
        {
            newRoom->AddMotionPath(
                AssetManager::GetPathByName(node.attribute(L"path").as_string()),
                node.attribute(L"xtox").as_float(1),
                node.attribute(L"xtoy").as_float(),
                node.attribute(L"ytox").as_float(),
                node.attribute(L"ytoy").as_float(1),
                node.attribute(L"dx").as_float(),
                node.attribute(L"dy").as_float());
        }
    }

    void Serializer::SerialLoadRoomTriggers(std::shared_ptr<Room> newRoom, pugi::xml_node triggersNode)
    {
        auto candidates = triggersNode.children();
        for (pugi::xml_node candidate : candidates)
        {
            if (std::wstring(candidate.name()) == L"trigger")
            {
                newRoom->AddTrigger(Parser::ParseTrigger(candidate));
            }
        }
    }
}