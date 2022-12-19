#include "BatchHandler.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include "AssetManager.h"

#define BATCH_SIZE 512

namespace SBURB
{
    BatchHandler::BatchHandler()
        : currentTexName(""), verticesInitialized(false),
          offset(0), verticesSize(0), target(nullptr)
    {
    }

    void BatchHandler::DrawPrimitive(const sf::VertexArray &coords, sf::RenderTarget &target)
    {
        if (currentTexName != "")
            DrawBatch();
        currentTexName = "";

        if (this->target == nullptr)
            this->target = &target;

        if (!verticesInitialized)
            InitializeVertices();

        if (offset >= verticesSize)
            GrowVertices();

        sf::Vertex *quad = &vertices[offset];

        quad[0].position = coords[0].position;
        quad[1].position = coords[1].position;
        quad[2].position = coords[2].position;
        quad[3].position = coords[3].position;

        quad[0].color = coords[0].color;
        quad[1].color = coords[1].color;
        quad[2].color = coords[2].color;
        quad[3].color = coords[3].color;

        offset += 4;
    }

    void BatchHandler::DrawSpriteRect(std::string textureName, const sf::VertexArray &coords, sf::RenderTarget &target)
    {
        if (textureName != currentTexName)
        {
            if (currentTexName != "")
                DrawBatch();
            this->currentTexName = textureName;
        }

        if (this->target == nullptr)
            this->target = &target;

        if (!verticesInitialized)
            InitializeVertices();

        if (offset >= verticesSize)
            GrowVertices();

        sf::Vertex *quad = &vertices[offset];

        quad[0].position = coords[0].position;
        quad[1].position = coords[1].position;
        quad[2].position = coords[2].position;
        quad[3].position = coords[3].position;

        quad[0].texCoords = coords[0].texCoords;
        quad[1].texCoords = coords[1].texCoords;
        quad[2].texCoords = coords[2].texCoords;
        quad[3].texCoords = coords[3].texCoords;

        quad[0].color = coords[0].color;
        quad[1].color = coords[1].color;
        quad[2].color = coords[2].color;
        quad[3].color = coords[3].color;

        offset += 4;
    }

    void BatchHandler::DrawBatch()
    {
        sf::RenderStates states = sf::RenderStates();
        
        if (currentTexName != "") {
            auto texAsset = AssetManager::GetGraphicByName(currentTexName)->Load();
            states.texture = texAsset.get();
            AssetManager::GetGraphicByName(currentTexName)->Unload();
        }

        if (offset != verticesSize) {
            vertices.resize(offset);
        }

        target->draw(vertices, states);
        verticesInitialized = false;
        offset = 0;
        verticesSize = 0;
        vertices.clear();
        target = nullptr;
    }

    void BatchHandler::InitializeVertices()
    {
        vertices.setPrimitiveType(sf::Quads);
        vertices.resize(BATCH_SIZE);
        verticesSize = BATCH_SIZE;
        offset = 0;
        verticesInitialized = true;
    }

    void BatchHandler::GrowVertices()
    {
        verticesSize += BATCH_SIZE;
        vertices.resize(verticesSize);
    }
}