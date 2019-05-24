#include "Stdafx.h"
#include "HMD.h"
#include "VRCamera.h"
#include "DebugHMD.h"
#include "ViveHMD.h"


HMD::HMD()
{}


HMD::~HMD()
{}

HMD* HMD::Create(bool forcedebug /*= false*/)
{
	//if needed return different HMD class
	if (ViveHMD::CanCreate() && !forcedebug)
	{
		Log::LogMessage("Found VR HMD");
		return new ViveHMD();
	}
	Log::LogMessage("VR HMD Debug device used");
	return new DebugHMD();
}

VRCamera * HMD::GetVRCamera()
{
	return CameraInstance;
}

void HMD::Init()
{
	CameraInstance = new VRCamera();
}

void HMD::Update()
{}

void HMD::OutputToEye(FrameBuffer * buffer, EEye::Type eye)
{}

void HMD::UpdateProjection(float aspect)
{
	CameraInstance->GetEyeCam(EEye::Left)->UpdateProjection(aspect);
	CameraInstance->GetEyeCam(EEye::Right)->UpdateProjection(aspect);
}

glm::ivec2 HMD::GetDimentions()
{
	return glm::ivec2(1080, 1200);
}

bool HMD::IsActive()
{
	return false;
}
