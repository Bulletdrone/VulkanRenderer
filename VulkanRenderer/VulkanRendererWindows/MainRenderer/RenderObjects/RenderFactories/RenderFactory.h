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

	BaseRenderObject* CreateRenderObject(const size_t a_RenderID, ShapeType a_ShapeType, Transform* a_Transform, uint32_t a_PipelineID);


	void ClearSceneObjects();
	void SetSceneObjects(std::vector<ShapeType>& a_AvailableShapes);

	void ResetSceneObjects(std::vector<ShapeType>& a_AvailableShapes);


private:
	size_t m_AllShapes = 4;
	std::vector<RenderObjectData*> m_RenderObjectsData;
	std::vector<size_t> m_CurrentLoadedObjectsID;
};