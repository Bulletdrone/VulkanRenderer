#include "MeshData.h"

#include <iostream>

void MeshData::DeleteBuffers(VkDevice& r_Device)
{
	vertices.DeleteBuffers(r_Device);
	indices.DeleteBuffers(r_Device);
}