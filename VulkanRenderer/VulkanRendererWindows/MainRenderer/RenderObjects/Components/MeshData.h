#pragma once
#include "Structs/BufferData.h"

struct MeshData
{
	void DeleteBuffers(VkDevice& r_Device);

	BufferData<Vertex>* GetVertexData() const { return vertices; }
	BufferData<uint32_t>* GetIndexData() const { return indices; }

	BufferData<Vertex>* vertices;
	BufferData<uint32_t>* indices;
};