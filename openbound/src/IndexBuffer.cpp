#include "IndexBuffer.h"

namespace SBURB
{
    IndexBuffer::IndexBuffer(const unsigned int *data, unsigned int count)
        : count(count)
    {
    }

    IndexBuffer::~IndexBuffer()
    {
    }

    void IndexBuffer::Bind() const
    {
    }

    void IndexBuffer::Unbind() const
    {
    }
}