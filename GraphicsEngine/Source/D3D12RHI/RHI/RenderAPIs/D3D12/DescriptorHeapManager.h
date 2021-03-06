#pragma once

class DXDescriptor;
class DescriptorHeap;
class D3D12CommandList;
class D3D12DeviceContext;
class DescriptorHeapManager
{
public:
	DescriptorHeapManager(D3D12DeviceContext* Device);
	void AllocateMainHeap(int size);
	~DescriptorHeapManager();
	DXDescriptor* AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, int size = 1);
	void CheckAndRealloc(int size, D3D12CommandList * list);
	void ClearMainHeap();
	DescriptorHeap* GetMainHeap();
	void BindHeap(D3D12CommandList* list);
	void RebindHeap(D3D12CommandList * list);
	static void CheckAndRealloc(DescriptorHeap ** Target, int num);
	static void Reallocate(DescriptorHeap** TargetHeat, int newsize);
	void EndOfFrame();
private:
	D3D12DeviceContext* Device = nullptr;
	DescriptorHeap* MainHeap = nullptr;
	DescriptorHeap* SamplerHeap = nullptr;
	bool DidJustResize = false;
	const char* TimerName = "Descriptors Spent";
};

