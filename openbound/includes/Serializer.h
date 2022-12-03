#ifndef SBURB_SERIALIZER_H
#define SBURB_SERIALIZER_H

#include <pugixml.hpp>
#include "Common.h"
#include "Sprite.h"
#include "Asset.h"

namespace SBURB {
    class Serializer {
    public:
        static std::string Serialize();
        static std::string SerializeAssets(std::string output);
        static std::string SerializeTemplates(std::string output);
        static std::string SerializeHud(std::string output);
        static std::string SerializeLooseObjects(std::string output);
        static std::string SerializeRooms(std::string output);
        static std::string SerializeGameState(std::string output);
        static std::string SerializeActionQueues(std::string output);

        // SerializeAttribute used to be done with templates, but it was such a hassle, I got annoyed and stopped.
        static std::string SerializeAttribute(std::string name, bool value, bool defaultValue = false) {
            return value != defaultValue ? " " + name + "='" + std::to_string(value) + "' " : "";
        }

        static std::string SerializeAttribute(std::string name, int value, int defaultValue = 0) {
            return value != defaultValue ? " " + name + "='" + std::to_string(value) + "' " : "";
        }

        static std::string SerializeAttribute(std::string name, std::string value, std::string defaultValue = "") {
            return value != defaultValue ? " " + name + "='" + value + "' " : "";
        }

        static std::string Serializer::SerializeAttribute(std::string name, Vector2 value)
        {
            std::string sub = " " + name + "='";
            sub += value.x + ",";
            sub += value.y;
            sub += "' ";
            return sub;
        }

        static std::string Serializer::SerializeAttribute(std::string name, Vector4 value)
        {
            std::string sub = " " + name + "='";
            sub += value.x + ",";
            sub += value.y + ",";
            sub += value.z + ",";
            sub += value.w;
            sub += "' ";
            return sub;
        }

        static pugi::xml_document ParseXML(std::string inText);

        static bool LoadSerialFromXML(std::string path, bool keepOld = false);
        static bool LoadSerial(pugi::xml_document* doc, bool keepOld = false);
        static bool LoadDependencies(pugi::xml_node node);
        static bool LoadSerialAssets(pugi::xml_node node);
        static void LoadSerialAsset(pugi::xml_node node);

        static void LoadSerialState();

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

        static std::shared_ptr<Asset> ParseSerialAsset(pugi::xml_node node);

        static void SerialLoadDialogSprites(pugi::xml_node dialogSprites);
        static void SerialLoadEffects(pugi::xml_node effectsNode);
        static void SerialLoadRoomSprites(std::shared_ptr<Room> newRoom, pugi::xml_object_range<pugi::xml_named_node_iterator> roomSprites);
        static void SerialLoadRoomPaths(std::shared_ptr<Room> newRoom, pugi::xml_node pathsNode);
        static void SerialLoadRoomTriggers(std::shared_ptr<Room> newRoom, pugi::xml_node triggersNode);
    };
}

#endif