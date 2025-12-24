#pragma once

#include "Chunk.h"
#include "../render/Mesh.h"
#include "../render/RenderObject.h"

class ChunkMesher
{
    public:
        static Render::RenderObject buildMesh(const Chunk& chunk);
};