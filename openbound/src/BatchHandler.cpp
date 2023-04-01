#include "BatchHandler.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include "AssetManager.h"

#define BATCH_SIZE 512

namespace SBURB
{
    BatchHandler::BatchHandler()
        : currentTexName(L""), verticesInitialized(false),
        offset(0), verticesSize(0), target(nullptr)
    {
    }

    void BatchHandler::DrawPrimitive(const sf::VertexArray& coords, sf::RenderTarget& target)
    {
        if (currentTexName != L"")
            DrawBatch();
        currentTexName = L"";

        if (this->target == nullptr)
            this->target = &target;

        if (!verticesInitialized)
            InitializeVertices();

        if (offset >= verticesSize)
            GrowVertices();

        sf::Vertex* quad = &vertices[offset];

        quad[0].position = coords[0].position;
        quad[1].position = coords[1].position;
        quad[2].position = coords[2].position;
        quad[3].position = coords[3].position;
        quad[4].position = coords[4].position;
        quad[5].position = coords[5].position;

        quad[0].color = coords[0].color;
        quad[1].color = coords[1].color;
        quad[2].color = coords[2].color;
        quad[3].color = coords[3].color;
        quad[4].color = coords[4].color;
        quad[5].color = coords[5].color;

        offset += 6;
    }

    void BatchHandler::DrawSpriteRect(std::wstring textureName, const sf::VertexArray& coords, sf::RenderTarget& target)
    {
        if (textureName != currentTexName)
        {
            if (currentTexName != L"")
                DrawBatch();
            this->currentTexName = textureName;
        }

        if (this->target == nullptr)
            this->target = &target;

        if (!verticesInitialized)
            InitializeVertices();

        if (offset >= verticesSize)
            GrowVertices();

        sf::Vertex* quad = &vertices[offset];

        quad[0].position = coords[0].position;
        quad[1].position = coords[1].position;
        quad[2].position = coords[2].position;
        quad[3].position = coords[3].position;
        quad[4].position = coords[4].position;
        quad[5].position = coords[5].position;

        quad[0].texCoords = coords[0].texCoords;
        quad[1].texCoords = coords[1].texCoords;
        quad[2].texCoords = coords[2].texCoords;
        quad[3].texCoords = coords[3].texCoords;
        quad[4].texCoords = coords[4].texCoords;
        quad[5].texCoords = coords[5].texCoords;

        quad[0].color = coords[0].color;
        quad[1].color = coords[1].color;
        quad[2].color = coords[2].color;
        quad[3].color = coords[3].color;
        quad[4].color = coords[4].color;
        quad[5].color = coords[5].color;

        offset += 6;
    }

    void BatchHandler::DrawBatch()
    {
        sf::RenderStates states = sf::RenderStates();
        if (currentTexName != L"")
            states.texture = AssetManager::GetGraphicByName(currentTexName)->GetAsset().get();
        if (offset != verticesSize)
            vertices.resize(offset);
        target->draw(vertices, states);
        verticesInitialized = false;
        offset = 0;
        verticesSize = 0;
        vertices.clear();
        target = nullptr;
    }

    void BatchHandler::InitializeVertices()
    {
        vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
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