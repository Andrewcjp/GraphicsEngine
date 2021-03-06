#include "HighLevelAccelerationStructure.h"
#include "LowLevelAccelerationStructure.h"


HighLevelAccelerationStructure::HighLevelAccelerationStructure(DeviceContext* Device, const AccelerationStructureDesc & desc)
{
	Context = Device;
	Desc = desc;
}


HighLevelAccelerationStructure::~HighLevelAccelerationStructure()
{}

void HighLevelAccelerationStructure::AddEntity(LowLevelAccelerationStructure * Struct)
{
	ContainedEntites.push_back(Struct);
}

void HighLevelAccelerationStructure::RemoveEntity(LowLevelAccelerationStructure * Struct)
{
	VectorUtils::Remove(ContainedEntites, Struct);
}

void HighLevelAccelerationStructure::ClearEntitys()
{
	ContainedEntites.clear();
}

void HighLevelAccelerationStructure::AddInstance(const AccelerationStructureInstanceDesc & desc)
{
	InstanceDescritors.push_back(desc);
}

void HighLevelAccelerationStructure::RemoveInstance(const AccelerationStructureInstanceDesc & desc)
{
	NoImpl();
}

void HighLevelAccelerationStructure::ClearInstances()
{
	InstanceDescritors.clear();
}

void HighLevelAccelerationStructure::Update(RHICommandList* List)
{

}

void HighLevelAccelerationStructure::Build(RHICommandList * list)
{

}

void HighLevelAccelerationStructure::InitialBuild()
{

}

int HighLevelAccelerationStructure::GetValidEntites() const
{
	return InstanceDescritors.size();
	int valid = 0;
	for (auto i : ContainedEntites)
	{
		if (i->IsValid())
		{
			valid++;
		}
	}
	return valid;
}
