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

namespace SBURB
{
    static std::map<std::string, pugi::xml_node> templateClasses;
    static int loadingDepth = 0;
    static std::vector<pugi::xml_node> loadQueue;

    std::string Serializer::Serialize()
    {
        Sburb *sburbInst = Sburb::GetInstance();

        std::string loadedFiles = "";
        bool loadedFilesExist = false;

        for (auto key : sburbInst->loadedFiles)
        {
            loadedFiles = loadedFiles + (loadedFilesExist ? "," : "") + key;
            loadedFilesExist = true;
        }

        auto character = sburbInst->GetCharacter();
        auto bgm = sburbInst->GetBGM();

        std::string output = "<sburb" +
                             std::string(" char='") + character->GetName() +
                             (bgm ? "' bgm='" + bgm->GetName() + (bgm->GetStartLoop() ? "," + std::to_string(bgm->GetStartLoop()) : "") : "") +
                             (sburbInst->GetScale().x != 1 ? "' scale='" + sburbInst->GetScale().x : "") +
                             (sburbInst->GetNextQueueId() > 0 ? "' nextQueueId='" + sburbInst->GetNextQueueId() : "") +
                             (sburbInst->resourcePath != "" ? ("' resourcePath='" + sburbInst->resourcePath) : "") +
                             (sburbInst->levelPath != "" ? ("' levelPath='" + sburbInst->levelPath) : "") +
                             (loadedFilesExist ? ("' loadedFiles='" + loadedFiles) : "") +
                             "'>\n";
        output = Serializer::SerializeAssets(output);
        output = Serializer::SerializeTemplates(output);
        output = Serializer::SerializeHud(output);
        output = Serializer::SerializeLooseObjects(output);
        output = Serializer::SerializeRooms(output);
        output = Serializer::SerializeGameState(output);
        output = Serializer::SerializeActionQueues(output);

        output = output + "\n</sburb>";

        return output;
    }

    std::string EncodeXML(std::string str)
    {
        return replace(replace(replace(replace(str, "&", "&amp;"), "<", "&lt;"), ">", "&gt;"), "\"", "&quot;");
    };

    std::string Serializer::SerializeGameState(std::string output)
    {
        output = output + "\n<gameState>\n";

        for (auto state : Sburb::GetInstance()->GetGameState())
        {
            output = output + "  <" + state.first + ">" + EncodeXML(state.second) + "</" + state.first + ">";
        }

        output = output + "\n</gameState>\n";

        return output;
    }

    std::string Serializer::SerializeActionQueues(std::string output)
    {
        output = output + "<actionQueues>";

        for (auto actionQueue : Sburb::GetInstance()->GetActionQueues())
        {
            if (actionQueue->GetCurrentAction())
            {
                output = actionQueue->Serialize(output);
            }
        }
        output = output + "\n</actionQueues>\n";

        return output;
    }

    std::string Serializer::SerializeAssets(std::string output)
    {
        output = output + "\n<assets>";

        for (auto asset : assets)
        {
            std::shared_ptr<Asset> curAsset = assets[asset];
            std::string innerHTML = "";

            if (curAsset->GetType() == "graphic")
            {

                innerHTML += std::static_pointer_cast<AssetGraphic>(curAsset)->GetPath();
            }
            else if (curAsset->GetType() == "audio")
            {
                bool firstSrc = false;
                for (auto srcVal : std::static_pointer_cast<AssetAudio>(curAsset)->GetSources())
                {
                    innerHTML += (firstSrc ? ";" : "") + srcVal;

                    firstSrc = true;
                }
            }
            else if (curAsset->GetType() == "path")
            {
                auto pathAsset = std::static_pointer_cast<AssetPath>(curAsset);
                for (int i = 0; i < pathAsset->GetPoints().size(); i++)
                {
                    innerHTML = innerHTML + std::to_string(pathAsset->GetPoints()[i].x) + "," + std::to_string(pathAsset->GetPoints()[i].y);
                    if (i != pathAsset->GetPoints().size() - 1)
                    {
                        innerHTML = innerHTML + ";";
                    }
                }
            }
            else if (curAsset->GetType() == "movie")
            {
                innerHTML += std::static_pointer_cast<AssetMovie>(curAsset)->GetPath();
            }
            else if (curAsset->GetType() == "font")
            {
                // Pretty sure this was broken originally. May need to look more into it.
                innerHTML += std::static_pointer_cast<AssetFont>(curAsset)->GetSources()[0];
            }
            else if (curAsset->GetType() == "text")
            {
                innerHTML += escape(trim(std::static_pointer_cast<AssetText>(curAsset)->GetText()).c_str());
            }

            output = output + "\n<asset name='" + curAsset->GetName() + "' type='" + curAsset->GetType() + "' ";

            output = output + " >";
            output = output + innerHTML;
            output = output + "</asset>";
        }

        output = output + "\n</assets>\n";
        output = output + "\n<effects>";

        for (auto effect : Sburb::GetInstance()->GetEffects())
        {
            output = effect.second->Serialize(output);
        }

        output = output + "\n</effects>\n";
        return output;
    }

    std::string Serializer::SerializeTemplates(std::string output)
    {
        output = output + "\n<classes>";

        std::ostringstream serializeStream;

        for (auto templateNode : templateClasses)
        {
            serializeStream.clear();
            templateNode.second.print(serializeStream, "", pugi::format_raw);
            output += serializeStream.str();
        }

        output = output + "\n</classes>\n";
        return output;
    }

    std::string Serializer::SerializeHud(std::string output)
    {
        output = output + "\n<hud>";

        for (auto content : Sburb::GetInstance()->GetHud())
        {
            output = content.second->Serialize(output);
        }

        output = Sburb::GetInstance()->GetDialoger()->Serialize(output);

        auto animations = Sburb::GetInstance()->GetDialoger()->GetDialogSpriteLeft()->GetAnimations();
        output = output + "\n<dialogsprites>";

        for (auto animation : animations)
        {
            output = animation.second->Serialize(output);
        }

        output = output + "\n</dialogsprites>";
        output = output + "\n</hud>\n";
        return output;
    }

    std::string Serializer::SerializeLooseObjects(std::string output)
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
                output = theSprite->Serialize(output);
            }
        }

        for (auto button : Sburb::GetInstance()->GetButtons())
        {
            auto theButton = button.second;

            if (!Sburb::GetInstance()->GetHud(theButton->GetName()))
            {
                output = theButton->Serialize(output);
            }
        }

        return output;
    }

    std::string Serializer::SerializeRooms(std::string output)
    {
        output = output + "\n<rooms>\n";

        for (auto room : Sburb::GetInstance()->GetRooms())
        {
            output = room.second->Serialize(output);
        }

        output = output + "\n</rooms>\n";

        return output;
    }

    bool Serializer::LoadSerialFromXML(std::string path, bool keepOld)
    {
        Sburb::GetInstance()->HaltUpdateProcess();
        path = Sburb::GetInstance()->levelPath + path;

        if (keepOld && Sburb::GetInstance()->loadedFiles[path])
        {
            Sburb::GetInstance()->StartUpdateProcess();
            return;
        }
        else
        {
            Sburb::GetInstance()->loadedFiles[path] = true;
        }

        pugi::xml_document doc;
        pugi::xml_parse_result initDocRes = doc.load_file(path.c_str());

        if (initDocRes.status != pugi::status_ok)
        {
            std::string errMsg = "For " + path + ": " + initDocRes.description();
            GlobalLogger->Log(Logger::Error, errMsg);
            return false;
        }

        // NOTE: DOC NEEDS TO BE KEPT ALIVE!!
        Serializer::LoadSerial(&doc, keepOld);
    }

    // IS THIS DOC KEPT ALIVE? I DON'T KNOW.
    pugi::xml_document Serializer::ParseXML(std::string inText)
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
    }

    bool Serializer::LoadSerial(pugi::xml_document *doc, bool keepOld)
    {
        pugi::xml_node rootNode = doc->child("sburb");

        if (!keepOld)
        {
            PurgeAssets();
            Sburb::GetInstance()->PurgeState();
        }

        std::string levelPath = rootNode.attribute("levelPath").value();
        if (levelPath != "")
        {
            if (levelPath[levelPath.length() - 1] == '/')
            {
                Sburb::GetInstance()->levelPath = levelPath;
            }
            else
            {
                Sburb::GetInstance()->levelPath = levelPath + "/";
            }
        }

        std::string resourcePath = rootNode.attribute("resourcePath").value();
        if (resourcePath != "")
        {
            Sburb::GetInstance()->resourcePath = resourcePath;
        }

        std::string name = rootNode.attribute("name").value();
        if (name != "")
            Sburb::GetInstance()->name = name;

        std::string version = rootNode.attribute("version").value();
        if (version != "")
            Sburb::GetInstance()->version = version;

        std::string width = rootNode.attribute("width").value();
        if (width != "")
        {
            Sburb::GetInstance()->window.SetSize({stoi(width), Sburb::GetInstance()->window.GetSize().y});
        }

        std::string height = rootNode.attribute("height").value();
        if (height != "")
        {
            Sburb::GetInstance()->window.SetSize({Sburb::GetInstance()->window.GetSize().y, stoi(height)});
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
        pugi::xml_node dependenciesNode = node.child("dependencies");

        if (dependenciesNode)
        {
            auto dependencyNodes = dependenciesNode.children("dependency");

            for (pugi::xml_node dependencyNode : dependencyNodes)
            {
                auto dependencyPath = dependencyNode.text().as_string();

                LoadSerialFromXML(Sburb::GetInstance()->levelPath + trim(dependencyPath));
            }
        }

        return true;
    }

    bool Serializer::LoadSerialAssets(pugi::xml_node node)
    {
        std::string description = node.attribute("description").value();
        if (description != "")
        {
            Sburb::GetInstance()->SetDescription(description);
        }
        else
        {
            Sburb::GetInstance()->SetDescription("assets");
        }

        pugi::xml_node assetsNode = node.child("assets");

        if (assetsNode)
        {
            auto assetNodes = assetsNode.children("asset");

            for (pugi::xml_node assetNode : assetNodes)
            {
                if (!AssetManager::CheckIsLoaded(assetNode.attribute("name").value()))
                {
                    LoadSerialAsset(assetNode);
                }
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
        std::string name = node.attribute("name").value();
        std::string type = node.attribute("type").value();
        std::string value = trim(node.text().as_string());

        std::string blobUrlsAttr = node.attribute("blob-urls").value();
        std::vector<std::string> blobUrls = {};

        if (blobUrlsAttr != "")
        {
            blobUrls = split(blobUrlsAttr, ";");
        }

        std::shared_ptr<Asset> asset;
        if (type == "graphic")
        {
            asset = std::make_shared<AssetGraphic>(name, value);
        }
        else if (type == "audio")
        {
            asset = std::make_shared<AssetAudio>(name, split(value, ";"));
        }
        else if (type == "path")
        {
            std::vector<std::string> splitPoints = split(value, ";");

            std::vector<Vector2> points;
            for (auto point : splitPoints)
            {
                auto pointCoords = split(point, ",");
                points.push_back(Vector2(stoi(pointCoords[0]), stoi(pointCoords[1])));
            }

            asset = std::make_shared<AssetPath>(name, points);
        }
        else if (type == "movie")
        {
            asset = std::make_shared<AssetMovie>(name, value);
        }
        else if (type == "font")
        {
            asset = std::make_shared<AssetFont>(name, split(value, ","));
        }
        else if (type == "text")
        {
            asset = std::make_shared<AssetText>(name, value);
        }

        return asset;
    }

    void Serializer::LoadSerialState()
    {
        // NOTE: USED TO HAVE A BIG LOOP HERE. NOT SURE WHAT TO DO ABOUT IT???

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

            // should be last
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
        auto classes = node.child("classes");

        if (!classes)
        {
            auto templates = classes.children();
            for (pugi::xml_node templateNode : templates)
            {
                if (templateNode.name() != "#text" && templateNode.name() != "#comment")
                {
                    ApplyTemplateClasses(templateNode);

                    templateClasses[templateNode.attribute("class").as_string()] = templateNode;
                }
            }

            node.remove_child(classes);
        }
    }

    void Serializer::ApplyTemplateClasses(pugi::xml_node node)
    {
        for (auto templateNode : templateClasses)
        {
            auto candidates = node.children(templateNode.second.name());

            for (pugi::xml_node candidate : candidates)
            {
                TryToApplyTemplate(templateNode.second, candidate);
            }
        }
    }

    void Serializer::TryToApplyTemplate(pugi::xml_node templateNode, pugi::xml_node candidateNode)
    {
        std::string templateClass = templateNode.attribute("class").as_string();
        std::string candClass = candidateNode.attribute("class").as_string();
        if (candClass != "" && candClass == templateClass)
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
            if (!candidateNode.attribute(tempAttribute.name()))
            {
                candidateNode.attribute(tempAttribute.name()).set_value(tempAttribute.as_string());
            }
        }

        for (auto tempChild : tempChildren)
        {
            candidateNode.append_copy(tempChild);
        }
    }

    void Serializer::ParseButtons(pugi::xml_node node)
    {
        auto newButtons = node.children("spritebutton");

        for (pugi::xml_node curButton : newButtons)
        {
            auto newButton = Parser::ParseSpriteButton(curButton);
            Sburb::GetInstance()->SetButton(newButton->GetName(), newButton);
        }
    }

    void Serializer::ParseSprites(pugi::xml_node node)
    {
        auto newSprites = node.children("sprite");

        for (pugi::xml_node curSprite : newSprites)
        {
            auto newSprite = Parser::ParseSprite(curSprite);
            Sburb::GetInstance()->SetSprite(newSprite->GetName(), newSprite);
            ParseActions(curSprite, *newSprite);
        }
    }

    void Serializer::ParseActions(pugi::xml_node spriteNode, Sprite sprite)
    {
        auto newActions = spriteNode.children();

        for (pugi::xml_node curAction : newActions)
        {
            if (curAction.name() == "#text")
            {
                continue;
            }
            if (curAction.name() == "action")
            {
                auto newAction = Parser::ParseAction(curAction);
                sprite.AddAction(newAction);
            }
        }
    }

    void Serializer::ParseCharacters(pugi::xml_node node)
    {
        auto newChars = node.children("character");

        for (pugi::xml_node curChar : newChars)
        {
            auto newChar = Parser::ParseCharacter(curChar);
            Sburb::GetInstance()->SetSprite(newChar->GetName(), std::static_pointer_cast<Sprite>(newChar));
            ParseActions(curChar, *newChar);
        }
    }

    void Serializer::ParseFighters(pugi::xml_node node)
    {
        auto newFighters = node.children("fighter");

        for (pugi::xml_node curFighter : newFighters)
        {
            auto newFighter = Parser::ParseFighter(curFighter);
            Sburb::GetInstance()->SetSprite(newFighter->GetName(), std::static_pointer_cast<Sprite>(newFighter));
            ParseActions(curFighter, *newFighter);
        }
    }

    void Serializer::ParseRooms(pugi::xml_node node)
    {
        auto newRooms = node.children("room");

        for (pugi::xml_node curRoom : newRooms)
        {
            auto newRoom = Parser::ParseRoom(curRoom);
            Sburb::GetInstance()->SetRoom(newRoom->GetName(), newRoom);
        }
    }

    void Serializer::ParseGameState(pugi::xml_node node)
    {
        auto newGameStates = node.children("gameState");

        for (pugi::xml_node curGameState : newGameStates)
        {
            auto children = curGameState.children();

            for (pugi::xml_node child : children)
            {
                if (child.type() == pugi::xml_node_type::node_pcdata)
                    continue;

                std::string key = node.name();
                std::string value = node.first_child().value();
                Sburb::GetInstance()->SetGameState(key, value);
            }
        }
    }

    void Serializer::ParseHud(pugi::xml_node node)
    {
        pugi::xml_node hudNode = node.child("hud");

        if (hudNode)
        {
            auto children = hudNode.children();

            for (pugi::xml_node child : children)
            {
                if (child.name() == "spritebutton")
                {
                    std::string name = child.attribute("name").as_string();
                    Sburb::GetInstance()->SetHud(name, Sburb::GetInstance()->GetButton(name));
                }
            }
        }

        Serializer::ParseDialoger(node);
        Serializer::ParseDialogSprites(node);
    }

    void Serializer::ParseDialoger(pugi::xml_node node)
    {
        auto dialogerNode = node.child("dialoger");

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

            Sburb::GetInstance()->SetDialoger(std::make_shared<Dialoger>(Parser::ParseDialoger(dialogerNode)));
            Sburb::GetInstance()->GetDialoger()->SetDialogSpriteLeft(dialogSpriteLeft);
            Sburb::GetInstance()->GetDialoger()->SetDialogSpriteRight(dialogSpriteRight);
        }
    }

    void Serializer::ParseDialogSprites(pugi::xml_node node)
    {
        pugi::xml_node hudNode = node.child("hud");

        if (hudNode)
        {
            auto dialogSprites = hudNode.child("dialogsprites");

            if (dialogSprites)
            {
                Serializer::SerialLoadDialogSprites(dialogSprites);
            }
        }
    }

    void Serializer::ParseEffects(pugi::xml_node node)
    {
        auto effects = node.child("effects");

        if (effects)
        {
            Serializer::SerialLoadEffects(effects);
        }
    }

    void Serializer::ParseState(pugi::xml_node node)
    {
        std::string character = node.attribute("char").as_string();
        if (character != "")
        {
            Sburb::GetInstance()->SetCharacter(std::static_pointer_cast<Character>(Sburb::GetInstance()->GetSprite(character)));
            Sburb::GetInstance()->SetFocus(Sburb::GetInstance()->GetCharacter());
            Sburb::GetInstance()->GetCharacter()->BecomePlayer();
        }

        std::string mode = node.attribute("mode").as_string();
        if (mode != "")
        {
            Sburb::GetInstance()->SetEngineMode(mode);
        }

        int scale = node.attribute("scale").as_int();
        if (scale)
        {
            Sburb::GetInstance()->SetScale(Vector2(scale, scale));
        }

        int nextQueueId = node.attribute("nextQueueId").as_int();
        if (nextQueueId)
        {
            Sburb::GetInstance()->SetNextQueueId(nextQueueId);
        }

        std::string curRoom = node.attribute("curRoom").as_string();
        if (curRoom != "")
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

        std::string bgm = node.attribute("bgm").as_string();
        if (bgm != "")
        {
            std::vector<std::string> params = split(bgm, ",");
            Sburb::GetInstance()->ChangeBGM(std::make_shared<Music>(params[0], std::stof(params.size() > 1 ? params[1] : "0")));
        }

        std::shared_ptr<Action> initAction;
        std::string initActionName = node.attribute("startAction").as_string();

        if (initActionName != "")
        {
            for (pugi::xml_node tmp : node.children())
            {
                if (tmp.name() == "action" && tmp.attribute("name").as_string() == initActionName)
                {
                    initAction = std::make_shared<Action>(Parser::ParseAction(tmp));
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
        auto element = node.child("actionQueues");
        if (!element)
        {
            return;
        }

        auto newActionQueues = element.children();
        for (pugi::xml_node curActionQueues : newActionQueues)
        {
            if (curActionQueues.name() == "#text")
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
            Sburb::GetInstance()->SetDialoger(std::make_shared<Dialoger>());
        }

        if (!dialoger->GetDialogSpriteLeft())
        {
            dialoger->SetDialogSpriteLeft(std::make_shared<Sprite>("dialogSprite", -1000, Sburb::GetInstance()->GetViewSize().y, 0, 0));
            dialoger->SetDialogSpriteRight(std::make_shared<Sprite>("dialogSprite", Sburb::GetInstance()->GetViewSize().x + 1000, Sburb::GetInstance()->GetViewSize().y, 0, 0));
        }

        auto animations = dialogSprites.children("animation");
        for (pugi::xml_node anim : animations)
        {
            dialoger->GetDialogSpriteLeft()->AddAnimation(std::make_shared<Animation>(Parser::ParseAnimation(anim)));
            dialoger->GetDialogSpriteRight()->AddAnimation(std::make_shared<Animation>(Parser::ParseAnimation(anim)));
        }
    }

    void Serializer::SerialLoadEffects(pugi::xml_node effectsNode)
    {
        auto animations = effectsNode.children("animation");
        for (pugi::xml_node anim : animations)
        {
            auto newEffect = Parser::ParseAnimation(anim);
            Sburb::GetInstance()->SetEffect(newEffect->GetName(), newEffect);
        }
    }

    void Serializer::SerialLoadRoomSprites(std::shared_ptr<Room> newRoom, pugi::xml_object_range<pugi::xml_named_node_iterator> roomSprites)
    {
        for (pugi::xml_node curSprite : roomSprites)
        {
            std::shared_ptr<Sprite> actualSprite = Sburb::GetInstance()->GetSprite(curSprite.attribute("name").as_string());
            newRoom->AddSprite(actualSprite);
        }
    }

    void Serializer::SerialLoadRoomPaths(std::shared_ptr<Room> newRoom, pugi::xml_node pathsNode)
    {
        auto walkables = pathsNode.children("walkable");
        for (pugi::xml_node node : walkables)
        {
            newRoom->AddWalkable(AssetManager::GetPathByName(node.attribute("path").as_string()));
        }

        auto unwalkables = pathsNode.children("unwalkable");
        for (pugi::xml_node node : unwalkables)
        {
            newRoom->AddUnwalkable(AssetManager::GetPathByName(node.attribute("path").as_string()));
        }

        auto motionPaths = pathsNode.children("motionpath");
        for (pugi::xml_node node : motionPaths)
        {
            newRoom->AddMotionPath(
                AssetManager::GetPathByName(node.attribute("path").as_string()),
                node.attribute("xtox").as_float(1),
                node.attribute("xtoy").as_float(),
                node.attribute("ytox").as_float(),
                node.attribute("ytoy").as_float(1),
                node.attribute("dx").as_float(),
                node.attribute("dy").as_float());
        }
    }

    void Serializer::SerialLoadRoomTriggers(std::shared_ptr<Room> newRoom, pugi::xml_node triggersNode)
    {
        auto candidates = triggersNode.children();
        for (pugi::xml_node candidate : candidates)
        {
            if (candidate.name() == "trigger")
            {
                newRoom->AddTrigger(std::make_shared<Trigger>(Parser::ParseTrigger(candidate)));
            }
        }
    }

    template <typename T>
    std::string Serializer::SerializeAttribute(std::string name, T value, T defaultValue)
    {
        return value != defaultValue ? " " + name + "='" + std::to_string(value) + "' " : "";
    }

    template <>
    std::string Serializer::SerializeAttribute(std::string name, Vector2 value, Vector2 defaultValue)
    {
        std::string sub = " " + name + "='";
        sub += value.x + ",";
        sub += value.y;
        sub += "' ";
        return sub;
    }

    template <>
    std::string Serializer::SerializeAttribute(std::string name, Vector4 value, Vector4 defaultValue)
    {
        std::string sub = " " + name + "='";
        sub += value.x + ",";
        sub += value.y + ",";
        sub += value.z + ",";
        sub += value.w;
        sub += "' ";
        return sub;
    }
}