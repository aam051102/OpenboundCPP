#ifndef SBURB_VERTEX_ARRAY_H
#define SBURB_VERTEX_ARRAY_H

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace SBURB
{
    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        void AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout);

        void Bind() const;
        void Unbind() const;

    private:
        unsigned int glID;
    };
}

#endif