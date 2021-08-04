#pragma once
#include "Structs/BufferData.h"

class MeshData
{
public:
	MeshData(const std::vector<Vertex> a_Vertices, const std::vector<uint32_t> a_Indices);
	~MeshData();

	void DeleteBuffers(VkDevice& r_Device);

	BufferData<Vertex>* GetVertexData() const { return m_Vertices; }
	BufferData<uint32_t>* GetIndexData() const { return m_Indices; }

private:
	BufferData<Vertex>* m_Vertices;
	BufferData<uint32_t>* m_Indices;
};