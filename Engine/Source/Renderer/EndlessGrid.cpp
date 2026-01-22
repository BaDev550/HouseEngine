#include "hepch.h"
#include "EndlessGrid.h"

namespace House
{
	EndlessGrid::EndlessGrid()
	{
		std::vector<glm::vec3> vertices = {
			glm::vec3(1.0f,  1.0f,  0.0f),
			glm::vec3(-1.0f, -1.0f,  0.0f),
			glm::vec3(-1.0f,  1.0f,  0.0f),
			glm::vec3(-1.0f, -1.0f,  0.0f),
			glm::vec3(1.0f,  1.0f,  0.0f),
			glm::vec3(1.0f, -1.0f,  0.0f)
		};
		_VertexCount = static_cast<uint32_t>(vertices.size());
		uint64_t bufferSize = sizeof(glm::vec3) * vertices.size();
		MEM::Ref<Buffer> stagingBuffer = Buffer::Create(
			bufferSize,
			BufferType::TransferSrc,
			MemoryProperties::HOST_VISIBLE | MemoryProperties::HOST_COHERENT
		);
		stagingBuffer->Map();
		stagingBuffer->WriteToBuffer(vertices.data());
		stagingBuffer->Unmap();

		_GridBuffer = Buffer::Create(
			bufferSize,
			BufferType::TransferDst | BufferType::VertexBuffer,
			MemoryProperties::DEVICE
		);
		Renderer::CopyBuffer(stagingBuffer, _GridBuffer, bufferSize);
	}
}