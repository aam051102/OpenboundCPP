#ifndef SBURB_SERIALIZER_H
#define SBURB_SERIALIZER_H

#include <pugixml.hpp>
#include "Common.h"
#include "Sprite.h"
#include "Asset.h"

namespace SBURB {
    class Serializer {
    public:
        static std::wstring Serialize();
        static std::wstring SerializeAssets(std::wstring output);
        static std::wstring SerializeTemplates(std::wstring output);
        static std::wstring SerializeHud(std::wstring output);
        static std::wstring SerializeLooseObjects(std::wstring output);
        static std::wstring SerializeRooms(std::wstring output);
        static std::wstring SerializeGameState(std::wstring output);
        static std::wstring SerializeActionQueues(std::wstring output);

        // SerializeAttribute used to be done with templates, but it was such a hassle, I got annoyed and stopped.
        static std::wstring SerializeAttribute(std::wstring name, bool value, bool defaultValue = false) {
            return value != defaultValue ? L" " + name + L"='" + std::to_wstring(value) + L"' " : L"";
        }

        static std::wstring SerializeAttribute(std::wstring name, int value, int defaultValue = 0) {
            return value != defaultValue ? L" " + name + L"='" + std::to_wstring(value) + L"' " : L"";
        }

        static std::wstring SerializeAttribute(std::wstring name, std::wstring value, std::wstring defaultValue = L"") {
            return value != defaultValue ? L" " + name + L"='" + value + L"' " : L"";
        }

        static std::wstring Serializer::SerializeAttribute(std::wstring name, Vector2 value)
        {
            std::wstring sub = L" " + name + L"='";
            sub += value.x + L",";
            sub += value.y;
            sub += L"' ";
            return sub;
        }

        static std::wstring Serializer::SerializeAttribute(std::wstring name, Vector4 value)
        {
            std::wstring sub = L" " + name + L"='";
            sub += value.x + L",";
            sub += value.y + L",";
            sub += value.z + L",";
            sub += value.w;
            sub += L"' ";
            return sub;
        }

        static pugi::xml_document ParseXML(std::wstring inText);

        static bool LoadSerialFromXML(std::wstring path, bool keepOld = false);
        static bool LoadSerialFromXMLMemory(std::wstring memory, bool keepOld = false);
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
        static void ParseActions(pugi::xml_node spriteNode, std::shared_ptr<Sprite> sprite);
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
        static void SerialLoadRoomSprites(std::shared_ptr<Room> newRoom, std::vector<pugi::xml_node> roomSprites);
        static void SerialLoadRoomPaths(std::shared_ptr<Room> newRoom, pugi::xml_node pathsNode);
        static void SerialLoadRoomTriggers(std::shared_ptr<Room> newRoom, pugi::xml_node triggersNode);
    };
}

#endif