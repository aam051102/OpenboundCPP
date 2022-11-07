#ifndef SBURB_VERTEX_BUFFER_H
#define SBURB_VERTEX_BUFFER_H

namespace SBURB
{
    class VertexBuffer
    {
    public:
        VertexBuffer(const void *data, unsigned int size);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;

    private:
        unsigned int glID;
    };
}

#endif