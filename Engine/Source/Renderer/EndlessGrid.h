#pragma once

#include "Utilities/Memory.h"
#include "Buffer.h"
#include "Pipeline.h"
#include "glm/glm.hpp"
#include <vector>

namespace House
{
	class EndlessGrid : public MEM::RefCounted
	{
    public:
        EndlessGrid();
		uint32_t _VertexCount = 6;
        MEM::Ref<Buffer> _GridBuffer;
	};
}