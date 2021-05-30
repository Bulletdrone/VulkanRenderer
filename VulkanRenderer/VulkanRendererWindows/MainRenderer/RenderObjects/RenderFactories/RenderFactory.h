#pragma once
#include <RenderObjects/RenderShape.h>

enum class ShapeType
{
	Triangle,
	Rectangle,
	SkyBoxRect
};

class RenderFactory
{
public:
	RenderFactory();
	~RenderFactory();

	BaseRenderObject* CreateRenderObject(const size_t a_RenderID, ShapeType a_ShapeType, Transform* a_Transform, PipeLineData* a_PipeLineData);


	void ClearSceneObjects();
	void SetSceneObjects(std::vector<ShapeType>& a_AvailableShapes);

	void ResetSceneObjects(std::vector<ShapeType>& a_AvailableShapes);


private:
	size_t m_AllShapes = 3;
	std::vector<RenderObjectData*> m_RenderObjectsData;
	std::vector<size_t> m_CurrentLoadedObjectsID;
};