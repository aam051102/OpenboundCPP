#ifndef SBURB_SERIALIZER_H
#define SBURB_SERIALIZER_H

#include <pugixml.hpp>
#include "Common.h"
#include "Asset.h"
#include "Sprite.h"

namespace SBURB {
    class Serializer {
    public:
        static bool LoadSerial(std::string path);
        static bool LoadDependencies(pugi::xml_node node);
        static bool LoadSerialAssets(pugi::xml_node node);
        static bool LoadSerialAsset(pugi::xml_node node);

        static void LoadSerialState(pugi::xml_node node);

        static void ParseTemplateClasses(pugi::xml_node node);
        static void ApplyTemplateClasses(pugi::xml_node node);
        static void TryToApplyTemplate(pugi::xml_node templateNode, pugi::xml_node candidateNode);

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

        static Asset ParseSerialAsset(pugi::xml_node node);
    };
}

#endif