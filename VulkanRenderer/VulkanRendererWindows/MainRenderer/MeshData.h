#pragma once
#include "Structs/BufferData.h"

class MeshData
{
public:
	MeshData(const std::vector<Vertex> a_Vertices, const std::vector<uint16_t> a_Indices);
	~MeshData();

	void DeleteBuffers(VkDevice& r_Device);

	BufferData<Vertex>* GetVertexData() const { return m_Vertices; }
	BufferData<uint16_t>* GetIndexData() const { return m_Indices; }

private:
	BufferData<Vertex>* m_Vertices;
	BufferData<uint16_t>* m_Indices;
};