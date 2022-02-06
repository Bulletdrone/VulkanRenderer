#pragma once
#include <RenderObjects/RenderShape.h>

class RenderFactory
{
public:
	RenderFactory();
	~RenderFactory();

	/*  Create and returns a renderobject.
		@param r_NewObject, a reference, returns true if a new mesh was created. Means it needs to have the right buffers depending on the Rendering API.
		@param a_RenderID, the RenderID the object will recieve.
		@param a_ShapeType, The kind of object.
		@param a_Transform, the Transform of the Object.
		@param a_PipelineID, the Pipeline the object will use.
		@return the created pointer to a renderobject.*/ 
	BaseRenderObject* CreateRenderObject(const uint32_t a_RenderID, Transform* a_Transform, Material& a_Material, const char* a_MeshPath);

private:
	void CreateRenderObject();
};