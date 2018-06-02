#pragma once
#include <iostream>
#include <vector>
#include "RenderBaseTypes.h"
#include "Renderable.h"
#include "RHI/RHICommandList.h"
#include "Core/Assets/MeshLoader.h"
class Mesh :public Renderable
{
public:
	Mesh();
	Mesh(std::string filename, MeshLoader::FMeshLoadingSettings& Settings);
	~Mesh();
	void Render(RHICommandList* list) override;
	void LoadMeshFromFile(std::string filename, MeshLoader::FMeshLoadingSettings & Settings);
private:
	RHIBuffer* VertexBuffer = nullptr;
	RHIBuffer* IndexBuffer = nullptr;
};

