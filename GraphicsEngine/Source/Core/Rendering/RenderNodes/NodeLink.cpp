#include "NodeLink.h"
#include "StorageNodeFormats.h"
#include "StorageNode.h"
#include "RHI/DeviceContext.h"

NodeLink::NodeLink()
{
}

NodeLink::NodeLink(EStorageType::Type Type, const std::string& format, const std::string& InputName, RenderNode* Owner)
	:DataFormat(format)
{
	TargetType = Type;
	SetLinkName(InputName);
	if (InputName.length() == 0)
	{
		LinkName = format;
	}
	OwnerNode = Owner;
}

NodeLink::~NodeLink()
{
}

bool NodeLink::SetStore(StorageNode* target)
{
	if (target == nullptr)
	{
		return true;
	}
	if (TargetType != target->StoreType)
	{
		Log::LogMessage("Incorrect Type ");
		DEBUGBREAK;
		return false;
	}
#if 0
	if (RHI::GetFrameCount() == 0)
	{
		if (DataFormat != target->DataFormat)
		{
			if (DataFormat != StorageFormats::DefaultFormat)
			{
				Log::LogMessage("Incorrect Data Format Expected: '" + DataFormat + "' got : '" + target->DataFormat + "'", Log::Warning);
			}
		}
	}
#endif
	StoreTarget = target;
	return true;
}

bool NodeLink::IsValid()
{
	return StoreTarget != nullptr;
}

StorageNode* NodeLink::GetStoreTarget() const
{
	return StoreTarget;
}

std::string NodeLink::GetLinkName() const
{
	return LinkName;
}

void NodeLink::SetLinkName(const std::string& val)
{
	LinkName = val;
}

void NodeLink::Validate(RenderGraph::ValidateArgs& args, RenderNode* parent)
{
	if (DataFormat == StorageFormats::DontCare)
	{
		return;
	}
	if (GetStoreTarget() == nullptr)
	{
		std::string OutputMsg = "Input '" + LinkName + "' on node '" + parent->GetName() + "' Unset";
		if (IsOptional)
		{
			args.AddWarning("Optional " + OutputMsg);
		}
		else
		{
			args.AddError(OutputMsg);
		}
		return;
	}
	//if (StoreLink != nullptr && DataFormat != StoreLink->DataFormat && StoreLink->DataFormat != StorageFormats::DontCare)
	//{
	//	PushWrongFormat(parent, args, StoreLink->DataFormat);
	//}
	if (StoreTarget != nullptr)
	{
		if (StoreTarget->DataFormat != DataFormat && StoreTarget->DataFormat != StorageFormats::DontCare)
		{
			PushWrongFormat(parent, args, StoreTarget->DataFormat);
		}
		if ((StoreTarget->GetDeviceObject() == nullptr ? 0 : StoreTarget->GetDeviceObject()->GetDeviceIndex()) != parent->GetDeviceIndex() && StoreTarget->StoreType != EStorageType::ShadowData)
		{
			args.AddError("Device mismatch");
			DEBUGBREAK;
		}
	}

}

void NodeLink::PushWrongFormat(RenderNode* parent, RenderGraph::ValidateArgs& args, const std::string& badformat)
{
	std::string output = "Node '" + parent->GetName() + "' with NodeLink: " + GetLinkName() + " Incorrect Data Format Expected: '" + DataFormat + "' got: '" + badformat + "'";
	if (args.ErrorWrongFormat)
	{
		args.AddError(output);
	}
	else
	{
		args.AddWarning(output);
	}
}

void NodeLink::SetOptional()
{
	IsOptional = true;
}
