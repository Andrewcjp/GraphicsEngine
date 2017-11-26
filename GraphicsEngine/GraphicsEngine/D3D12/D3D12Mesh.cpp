#include "stdafx.h"
#include "D3D12Mesh.h"
#include "D3D12RHI.h"
#include "Rendering/Core/Triangle.h"
#include "../Core/Assets/OBJFileReader.h"
D3D12Mesh::D3D12Mesh()
{
	CreateVertexBuffer();
}


D3D12Mesh::~D3D12Mesh()
{
}

void D3D12Mesh::Render()
{

}
void D3D12Mesh::Render(ID3D12GraphicsCommandList * list)
{
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	list->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	list->DrawInstanced(m_numtriangles*3, 1, 0, 0);
}
void D3D12Mesh::CreateVertexBuffer()
{
#define tri 0
	// Create the vertex buffer.
	{
#if tri
		float m_aspectRatio = 1.7f;
		//// Define the geometry for a triangle.
		Vertex triangleVertices[] =
		{
			{ { 0.0f, 0.25f * m_aspectRatio, 0.0f },{ 0.5f, 0.0f } },
			{ { 0.25f, -0.25f * m_aspectRatio, 0.0f },{ 1.0f, 1.0f } },
			{ { -0.25f, -0.25f * m_aspectRatio, 0.0f },{ 0.0f, 1.0f } }
		};
		const UINT vertexBufferSize = sizeof(triangleVertices);
		m_numtriangles = 1;
#else
		Triangle* mesh;
		m_numtriangles = importOBJMesh(L"C:\\Users\\AANdr\\Dropbox\\Engine\\Engine\\Repo\\GraphicsEngine\\x64\\asset\\models\\House.obj", &mesh);
		const UINT vertexBufferSize = sizeof(OGLVertex)*m_numtriangles * 3;
#endif
		

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		ThrowIfFailed(D3D12RHI::Instance->m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_vertexBuffer)));

		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
#if tri
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
#else
		memcpy(pVertexDataBegin, mesh, sizeof(OGLVertex)*m_numtriangles*3);
#endif
		m_vertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
#if tri
		m_vertexBufferView.StrideInBytes = sizeof(Vertex);
#else

		m_vertexBufferView.StrideInBytes = sizeof(OGLVertex);
#endif
		m_vertexBufferView.SizeInBytes = vertexBufferSize;
	}
}