#pragma once

class ShaderBindingTable;
class RayTracingCommandList;
class ShadowAtlasStorageNode;
class Scene;
//This handles render interaction with RT features
//It abstracts between the Software and API support modes and handles build AS's etc.
//it will handle different types of Acceleration structure for multiple types of devices e.g. RTX GPU + Software RT GPU. 

class RayTracingEngine
{
public:
	RayTracingEngine();
	~RayTracingEngine();
	static RayTracingEngine* Get();
	void EnqueueForBuild(LowLevelAccelerationStructure* Struct);
	void EnqueueForBuild(HighLevelAccelerationStructure* Struct);
	//call as early as possible to start on async compute
	void BuildForFrame(RHICommandList * List);

	void OnFirstFrame();

	void BuildStructures(RHICommandList* AsyncbuildList);
	static RayTracingCommandList* CreateRTList(DeviceContext* Device);

	void UpdateFromScene(Scene* S);
	void AddHitTable(ShaderBindingTable* Table);
	HighLevelAccelerationStructure* GetHighLevelStructure();
	std::vector<ShaderBindingTable*> Tables;
private:

	std::vector<LowLevelAccelerationStructure*> LASToBuild;
	std::vector<HighLevelAccelerationStructure*> HASToBuild;
	HighLevelAccelerationStructure* CurrnetHL = nullptr;
	
	bool Build = false;
	bool UseTlasUpdate = false;

};

