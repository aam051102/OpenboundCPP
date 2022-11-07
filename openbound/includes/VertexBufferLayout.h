#ifndef SBURB_VERTEX_BUFFER_LAYOUT_H
#define SBURB_VERTEX_BUFFER_LAYOUT_H

#include <vector>

namespace SBURB
{
    class VertexBufferLayout
    {
    public:
        struct Element
        {
            unsigned int type;
            unsigned int count;
            bool normalized;

            static unsigned int GetSizeOfType(unsigned int type);
        };

        VertexBufferLayout();

        template <typename T>
        void Push(unsigned int count);

        inline const std::vector<Element> GetElements() const { return elements; }
        inline unsigned int GetStride() const { return stride; }

    private:
        std::vector<Element> elements;
        unsigned int stride;
    };
}

#endif