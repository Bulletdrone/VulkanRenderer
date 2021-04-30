#include "MeshData.h"

MeshData::MeshData(std::vector<Vertex> a_Vertices)
{
	m_Vertices = a_Vertices;
	//indices = a_Indices;

	//SETUP THE THING.
	SetupMesh();
}

MeshData::~MeshData()
{

}

void MeshData::SetupMesh()
{

}