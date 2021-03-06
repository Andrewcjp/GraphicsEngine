#pragma once
namespace EEye
{
	enum Type
	{
		Left,
		Right,		
		Limit
	};
}
class VRCamera;
class FrameBuffer;
class HMD
{
public:
	HMD();
	virtual ~HMD();
	static HMD* Create();
	VRCamera* GetVRCamera();
	virtual void Init();
	virtual void Update();
	virtual void OutputToEye(FrameBuffer* buffer, EEye::Type eye);
	void UpdateProjection(float aspect);
	virtual glm::ivec2 GetDimentions();
	virtual bool IsActive();
protected:
	VRCamera* CameraInstance = nullptr;
};

