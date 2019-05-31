#pragma once

class Material;
class MeshInstanceBuffer;
struct MeshBatchElement
{
	RHIBuffer* VertexBuffer = nullptr;
	RHIBuffer* IndexBuffer = nullptr;
	RHIBuffer* TransformBuffer = nullptr;
	int NumPrimitives = 0;
	int NumInstances = 0;
	Material* MaterialInUse = nullptr;
	bool IsVisible = true;
	bool bTransparent = false;
};

class MeshBatch
{
public:
	MeshBatch();
	~MeshBatch();
	void AddMeshElement(MeshBatchElement* element);
	//private:
	std::vector<MeshBatchElement*> elements;
	bool CastShadow = true;
	bool MainPassCulled = false;
	bool ShadowPassCulled = false;
	GameObject* Owner = nullptr;
	void Update();
	//Are we instanced?
	bool IsinstancedBatch = false;
	//if this is null this is the instance control node.
	//else we are subnode.
	MeshBatch* InstanceOwner = nullptr;

	MeshInstanceBuffer* InstanceBuffer = nullptr;
};

