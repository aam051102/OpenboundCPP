#ifndef SBURB_BATCH_HANDLER_H
#define SBURB_BATCH_HANDLER_H

#include <SFML/Graphics/VertexArray.hpp>

namespace SBURB
{
    class BatchHandler
    {
    public:
        // Singleton Initialization
        inline static BatchHandler &getInstance()
        {
            static BatchHandler instance;
            return instance;
        }

        // Delete methods we don't want
        BatchHandler(BatchHandler const &) = delete;
        void operator=(BatchHandler const &) = delete;

        // Actual public method
        void DrawSpriteRect(std::string textureName, const sf::VertexArray &coords, sf::RenderTarget &target);
        void DrawPrimitive(const sf::VertexArray &coords, sf::RenderTarget &target);
        void DrawBatch();
        inline bool BatchExists() const { return verticesSize != 0; }
        inline void Reset() { currentTexName = ""; }

    private:
        BatchHandler();

        // Methods
        void InitializeVertices();
        void GrowVertices();

        // Members
        bool verticesInitialized;
        std::string currentTexName;
        int offset;
        int verticesSize;
        sf::VertexArray vertices;
        sf::RenderTarget *target;
    };
}

#endif