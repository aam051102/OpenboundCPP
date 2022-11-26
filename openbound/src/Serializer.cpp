#include "Serializer.h"
#include "Game.h"
#include "Logger.h"
#include "Parser.h"

namespace SBURB {
    bool Serializer::LoadSerial(std::string path) {
        pugi::xml_document doc;
        pugi::xml_parse_result initDocRes = doc.load_file(path.c_str());

        if (initDocRes.status != pugi::status_ok) {
            std::string errMsg = "For " + path + ": " + initDocRes.description();
            GlobalLogger->Log(Logger::Error, errMsg);
            return false;
        }

        pugi::xml_node rootNode = doc.child("sburb");

        std::string levelPath = rootNode.attribute("levelPath").value();
        if (levelPath != "") {
            if (levelPath[levelPath.length() - 1] == '/') {
                Game::GetInstance()->levelPath = levelPath;
            }
            else {
                Game::GetInstance()->levelPath = levelPath + "/";
            }
        }

        std::string resourcePath = rootNode.attribute("resourcePath").value();
        if (resourcePath != "") {
            Game::GetInstance()->resourcePath = resourcePath;
        }

        std::string name = rootNode.attribute("name").value();
        if (name != "") Game::GetInstance()->name = name;

        std::string version = rootNode.attribute("version").value();
        if (version != "") Game::GetInstance()->version = version;

        std::string width = rootNode.attribute("width").value();
        if (width != "") {
            Game::GetInstance()->window.SetSize({stoi(width), Game::GetInstance()->window.GetSize().y});
        }

        std::string height = rootNode.attribute("height").value();
        if (height != "") {
            Game::GetInstance()->window.SetSize({Game::GetInstance()->window.GetSize().y, stoi(height)});
        }

        LoadDependencies(rootNode);
        LoadSerialAssets(rootNode);
        LoadSerialState(rootNode);

        return true;
    }

    bool Serializer::LoadDependencies(pugi::xml_node node) {
        pugi::xml_node dependenciesNode = node.child("dependencies");

        if (dependenciesNode) {
            auto dependencyNodes = dependenciesNode.children("dependency");

            for (pugi::xml_node dependencyNode : dependencyNodes) {
                auto dependencyPath = dependencyNode.text().as_string();

                LoadSerial(Game::GetInstance()->levelPath + trim(dependencyPath));
            }
        }

        return true;
    }

    bool Serializer::LoadSerialAssets(pugi::xml_node node) {
        std::string description = node.attribute("description").value();
        if (description != "") {
            Game::GetInstance()->assetManager.SetDescription(description);
        }
        else {
            Game::GetInstance()->assetManager.SetDescription("assets");
        }

        pugi::xml_node assetsNode = node.child("assets");

        if (assetsNode) {
            auto assetNodes = assetsNode.children("asset");

            for (pugi::xml_node assetNode : assetNodes) {
                if (!Game::GetInstance()->assetManager.CheckIsLoaded(assetNode.attribute("name").value())) {
                    LoadSerialAsset(assetNode);
                }
            }
        }

        return true;
    }

    bool Serializer::LoadSerialAsset(pugi::xml_node node) {
        auto newAsset = ParseSerialAsset(node);
        Game::GetInstance()->assetManager.LoadAsset(newAsset);
        return true;
    }

    Asset Serializer::ParseSerialAsset(pugi::xml_node node) {
        std::string name = node.attribute("name").value();
        std::string type = node.attribute("type").value();
        std::string value = trim(node.text().as_string());

        std::string blobUrlsAttr = node.attribute("blob-urls").value();
        std::vector<std::string> blobUrls = {};

        if (blobUrlsAttr != "") {
            blobUrls = split(blobUrlsAttr, ";");
        }

        Asset asset;
        if (type == "graphic") {
            asset = CreateGraphicAsset(name, value, blobUrls);
        }
        else if (type == "audio") {
            asset = CreateAudioAsset(name, split(value, ";"), blobUrls);
        }
        else if (type == "path") {
            std::vector<std::string> points = split(value, ";");
            Path path = Path();

            for (auto point : points) {
                auto pointCoords = split(point, ",");
                path.Push({ stoi(pointCoords[0]), stoi(pointCoords[1]) });
            }

            asset = CreatePathAsset(name, path);
        }
        else if (type == "movie") {
            asset = CreateMovieAsset(name, value, blobUrls);
        }
        else if (type == "font") {
            asset = CreateFontAsset(name, value);
        }
        else if (type == "text") {
            asset = CreateTextAsset(name, value);
        }

        return asset;
    }

    void Serializer::LoadSerialState(pugi::xml_node node) {
        // Don't load state until assets are all loaded
        if (updateLoop) {
            clearTimeout(updateLoop);
            updateLoop = null;
        }
        if (!Game::GetInstance()->assetManager.FinishedLoading()) {
            updateLoop = setTimeout(function() { loadSerialState(); }, 500);
            return;
        }

        while (loadQueue.length > 0) {
            var input = loadQueue[0];
            loadQueue.splice(0, 1);
            //These two have to be first
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

            //should be last
            ParseState(input);

            //Relies on Sburb.nextQueueId being set when no Id is provided
            ParseActionQueues(input);
        }

        if (loadQueue.length == 0 && loadingDepth == 0) {
            Sburb.startUpdateProcess();
        }
    }

    void Serializer::ParseTemplateClasses(pugi::xml_node node) {
        auto classes = node.child("classes");

        if (!classes) {
            auto templates = classes.children();
            for (pugi::xml_node templateNode : templates) {
                if (templateNode.name() != "#text" && templateNode.name() != "#comment") {
                    ApplyTemplateClasses(templateNode);

                    templateClasses[templateNode.attribute("class").as_string()] = templateNode.cloneNode(true);
                }
            }

            node.remove_child(classes);
        }
    }

    void Serializer::ApplyTemplateClasses(pugi::xml_node node) {
        for (pugi::xml_node templateNode : templateClasses) {
            auto candidates = node.children(templateNode.name());

            for (pugi::xml_node candidate : candidates) {
                TryToApplyTemplate(templateNode, candidate);
            }
        }
    }

    void Serializer::TryToApplyTemplate(pugi::xml_node templateNode, pugi::xml_node candidateNode) {

    }

    void Serializer::ParseButtons(pugi::xml_node node) {
        auto newButtons = node.children("spritebutton");
        
        for (pugi::xml_node curButton : newButtons) {
            SpriteButton newButton = Parser::ParseSpriteButton(curButton);
            buttons[newButton.name] = newButton;
        }
    }

    void Serializer::ParseSprites(pugi::xml_node node) {
        auto newSprites = node.children("sprite");

        for (pugi::xml_node curSprite : newSprites) {
            Sprite newSprite = Parser::ParseSprite(curSprite);
            sprites[newSprite.name] = newSprite;
            ParseActions(curSprite, newSprite);
        }
    }

    void Serializer::ParseActions(pugi::xml_node spriteNode, Sprite sprite) {
        auto newActions = spriteNode.children();

        for (pugi::xml_node curAction : newActions) {
            if (curAction.name() == "#text") {
                continue;
            }
            if (curAction.name() == "action") {
                Action newAction = Parser::ParseAction(curAction);
                sprite.AddAction(newAction);
            }
        }
    }

    void Serializer::ParseCharacters(pugi::xml_node node) {
        auto newChars = node.children("character");

        for (pugi::xml_node curChar : newChars) {
            Character newChar = Parser::ParseCharacter(curChar);
            sprites[newChar.name] = newChar;
            ParseActions(curChar, newChar);
        }
    }

    void Serializer::ParseFighters(pugi::xml_node node) {
        auto newFighters = node.children("fighter");

        for (pugi::xml_node curFighter : newFighters) {
            Fighter newFighter = Parser::ParseFighter(curFighter);
            sprites[newFighter.name] = newFighter;
            ParseActions(curFighter, newFighter);
        }
    }

    void Serializer::ParseRooms(pugi::xml_node node) {
        auto newRooms = node.children("room");

        for (pugi::xml_node curRoom : newRooms) {
            Room newRoom = Parser::ParseRoom(curRoom);
            rooms[newRoom.name] = newRoom;
        }
    }

    void Serializer::ParseGameState(pugi::xml_node node) {
        auto newGameStates = node.children("gameState");

        for (pugi::xml_node curGameState : newGameStates) {
            auto children = curGameState.children();

            for (pugi::xml_node child : children) {
                if (child.type() == pugi::xml_node_type::node_pcdata)
                    continue;

                std::string key = node.name();
                std::string value = node.first_child().value();
                gameState[key] = value;
            }
        }
    }

    void Serializer::ParseHud(pugi::xml_node node) {
        pugi::xml_node hudNode = node.child("hud");

        if (hudNode) {
            auto children = hudNode.children();

            for (pugi::xml_node child : children) {
                if (child.name()  == "spritebutton") {
                    std::string name = child.attribute("name").as_string();
                    hud[name] = buttons[name];
                }
            }
        }

        ParseDialoger(node);
        ParseDialogSprites(node);
    }

    void Serializer::ParseDialoger(pugi::xml_node node) {
        pugi::xml_node dialogerNode = node.child("dialoger");

        if (dialogerNode) {
            var dialogSpriteLeft = null;
            var dialogSpriteRight = null;
            
            if (dialoger) {
                dialogSpriteLeft = dialoger.dialogSpriteLeft;
                dialogSpriteRight = dialoger.dialogSpriteRight;
            }

            dialoger = Parser::ParseDialoger(dialogerNode);
            dialoger.dialogSpriteLeft = dialogSpriteLeft;
            dialoger.dialogSpriteRight = dialogSpriteRight;
        }
    }

    void Serializer::ParseDialogSprites(pugi::xml_node node) {
        pugi::xml_node hudNode = node.child("hud");

        if (hudNode) {
            auto dialogSprites = hudNode.child("dialogsprites");

            if (dialogSprites) {
                SerialLoadDialogSprites(dialogSprites);
            }
        }
    }

    void Serializer::ParseEffects(pugi::xml_node node) {
        auto effects = node.child("effects");

        if (effects) {
            SerialLoadEffects(effects);
        }
    }

    void Serializer::ParseState(pugi::xml_node node) {
        std::string character = node.attribute("char").as_string();
        if (character != "") {
            Sburb.focus = Sburb.character = Sburb.sprites[character];
            Sburb.character.BecomePlayer();
        }

        std::string mode = node.attribute("mode").as_string();
        if (mode != "") {
            Sburb.engineMode = mode;
        }

        int scale = node.attribute("scale").as_int();
        if (scale) {
            Sburb.Stage.scaleX = Sburb.Stage.scaleY = scale;
        }

        int nextQueueId = node.attribute("nextQueueId").as_int();
        if (nextQueueId) {
            Sburb.nextQueueId = nextQueueId;
        }

        std::string curRoom = node.attribute("curRoom").as_string();
        if (curRoom != "") {
            Sburb.curRoom = Sburb.rooms[curRoom];
            Sburb.curRoom.enter();
        }
        else if (Sburb.curRoom == nullptr && Sburb.character != nullptr) {
            for (auto room : Sburb.rooms) {
                if (room.Contains(Sburb.character)) {
                    Sburb.curRoom = room;
                    Sburb.curRoom.Enter();
                    break;
                }
            }
        }

        std::string bgm = node.attribute("bgm").as_string();
        if (bgm != "") {
            std::vector<std::string> params = split(bgm, ",");
            Sburb.changeBGM(BGM(Sburb.assets[params[0]], std::stof(params.size() > 1 ? params[1] : "0")));
        }

        std::shared_ptr<Action> initAction;
        std::string initActionName = node.attribute("startAction").as_string();

        if (initActionName != "") {
            for (pugi::xml_node tmp : node.children()) {
                if (tmp.name() == "action" && tmp.attribute("name").as_string() == initActionName) {
                    initAction = std::make_shared<Action>(Parser::ParseAction(tmp));
                    continue;
                }
            }

            if (initAction) {
                Sburb.PerformAction(initAction);
            }
        }
    }

    void Serializer::ParseActionQueues(pugi::xml_node node) {
        auto element = node.child("actionQueues");
        if (!element) {
            return;
        }

        auto newActionQueues = element.children();
        for (pugi::xml_node curActionQueues : newActionQueues) {
            if (curActionQueues.name()  == "#text") {
                continue;
            }

            ActionQueue actionQueue = Parser::ParseActionQueue(curActionQueues);
            actionQueues.push_back(actionQueue);
        }
    }

    template<typename T>
    std::string Serializer::SerializeAttribute(std::string name, T value, T defaultValue) {
        return value != defaultValue ? " " + name + "='" + std::to_string(value) + "' " : "";
    }


    template <>
    std::string Serializer::SerializeAttribute(std::string name, Vector2 value, Vector2 defaultValue) {
        std::string sub = " " + name + "='";
        sub += value.x + ",";
        sub += value.y;
        sub += "' ";
        return sub;
    }
    
    template <>
    std::string Serializer::SerializeAttribute(std::string name, Vector4 value, Vector4 defaultValue) {
        std::string sub = " " + name + "='";
        sub += value.x + ",";
        sub += value.y + ",";
        sub += value.z + ",";
        sub += value.w;
        sub += "' ";
        return sub;
    }
}