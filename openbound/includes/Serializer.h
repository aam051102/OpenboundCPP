#ifndef SBURB_SERIALIZER_H
#define SBURB_SERIALIZER_H

#include <pugixml.hpp>
#include "Common.h"
#include "Sprite.h"

namespace SBURB {
    class Serializer {
    public:
        template <typename T>
        static std::string SerializeAttribute(std::string name, T value, T defaultValue = T());

        static bool LoadSerial(std::string path);
        static bool LoadDependencies(pugi::xml_node node);
        static bool LoadSerialAssets(pugi::xml_node node);
        static bool LoadSerialAsset(pugi::xml_node node);

        static void LoadSerialState(pugi::xml_node node);

        static void ParseTemplateClasses(pugi::xml_node node);
        static void ApplyTemplateClasses(pugi::xml_node node);
        static void TryToApplyTemplate(pugi::xml_node templateNode, pugi::xml_node candidateNode);
        static void ApplyTemplate(pugi::xml_node templateNode, pugi::xml_node candidateNode);

        static void ParseButtons(pugi::xml_node node);
        static void ParseSprites(pugi::xml_node node);
        static void ParseActions(pugi::xml_node spriteNode, Sprite sprite);
        static void ParseCharacters(pugi::xml_node node);
        static void ParseFighters(pugi::xml_node node);
        static void ParseRooms(pugi::xml_node node);
        static void ParseGameState(pugi::xml_node node);
        static void ParseHud(pugi::xml_node node);
        static void ParseDialoger(pugi::xml_node node);
        static void ParseDialogSprites(pugi::xml_node node);
        static void ParseEffects(pugi::xml_node node);
        static void ParseState(pugi::xml_node node);
        static void ParseActionQueues(pugi::xml_node node);

        static AssetTexture ParseSerialAsset(pugi::xml_node node);

        static void SerialLoadDialogSprites(pugi::xml_node dialogSprites);
        static void SerialLoadEffects(pugi::xml_node effectsNode);
        static void SerialLoadRoomSprites(std::shared_ptr<Room> newRoom, pugi::xml_object_range<pugi::xml_named_node_iterator> roomSprites);
        static void SerialLoadRoomPaths(std::shared_ptr<Room> newRoom, pugi::xml_node pathsNode);
        static void SerialLoadRoomTriggers(std::shared_ptr<Room> newRoom, pugi::xml_node triggersNode);
    };
}

#endif