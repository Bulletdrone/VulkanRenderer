#pragma once
#include <RenderObjects/RenderShape.h>
#include <Tools/ResourceLoader.h>

enum class ShapeType
{
	Triangle,
	Rectangle,
	SkyBoxRect,
	Pavillion
};

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
	BaseRenderObject* CreateRenderObject(bool& r_NewObject, const uint32_t a_RenderID, ShapeType a_ShapeType, Transform* a_Transform, Material& a_Material);

	//Remove all RenderObjects from the Vector.
	void ClearRenderObjects();

	/*  Check if a RenderObject already exists, creates a new one if it doesn't exist..
		@param a_ShapeType, The kind of object that you want to create.
		@return false if the shape was already loaded, true if the shape was made. */
	bool CheckRenderObject(ShapeType a_ShapeType);

	/*  Remove all RenderObjects and instantiate new shapes.
		@param a_AvailableRenderObjects, The new shapes you want to load in. */
	void ResetRenderObjects(const std::vector<uint32_t>& a_AvailableRenderObjects);


private:
	void CreateRenderObject(ShapeType a_ShapeType);

	uint32_t m_AllShapes = 4;
	std::vector<RenderObjectData*> m_RenderObjectsData;
	std::vector<uint32_t> m_CurrentLoadedObjectsID;
};