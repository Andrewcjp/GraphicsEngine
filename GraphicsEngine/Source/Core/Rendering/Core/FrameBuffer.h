#pragma  once
class DeviceContext;
class RHIGPUSyncEvent;
class RHICommandList;
class RHIInterGPUStagingResource;
struct RHIFrameBufferDesc;
class FrameBuffer : public IRHIResourse
{
public:
	RHI_API FrameBuffer(DeviceContext* device, const RHIFrameBufferDesc& Desc);
	RHI_API virtual ~FrameBuffer();
	RHI_API int GetWidth() const;
	RHI_API int GetHeight() const;
	virtual DeviceContext* GetDevice() = 0;

	RHI_API RHIFrameBufferDesc& GetDescription();
	void Resize(int width, int height);
	RHI_API void SFRResize();

	static void CopyHelper(FrameBuffer* Target, DeviceContext* TargetDevice, EGPUCOPYTIMERS::Type Stat = EGPUCOPYTIMERS::MGPUCopy, EDeviceContextQueue::Type CopyQ = EDeviceContextQueue::Copy);
	RHI_API virtual void CopyToOtherBuffer(FrameBuffer * OtherBuffer, RHICommandList* List);
	static void CopyHelper_Async_OneFrame(FrameBuffer * Target, DeviceContext * TargetDevice);
	static void CopyHelper_Async(FrameBuffer * Target, DeviceContext * TargetDevice);
	virtual const RHIPipeRenderTargetDesc& GetPiplineRenderDesc() = 0;
	RHI_API virtual void BindDepthWithColourPassthrough(class RHICommandList* list, FrameBuffer* PassThrough);

	///Needs to called before buffer is read for final present
	void ResolveSFR(FrameBuffer* SumBuffer);
	int GetTransferSize();
	void ResetTransferStat();
	static void CopyHelper_NewSync(FrameBuffer * Target, DeviceContext * TargetDevice, EGPUCOPYTIMERS::Type Stat, EDeviceContextQueue::Type CopyQ = EDeviceContextQueue::Copy);

	RHI_API virtual void Release() override;


	RHI_API  size_t GetSizeOnGPU() ;
	static void AutoUpdateSize(RHIFrameBufferDesc & desc, DeviceContext* Device = nullptr);
	//Helper that uses the SizeMode in the desc to resize FB
	void AutoResize();

	void CopyToStagingResource(RHIInterGPUStagingResource* Res, RHICommandList* List);
	void CopyFromStagingResource(RHIInterGPUStagingResource* Res, RHICommandList* list);

	//state changers
	//For render target -> read the ChangeDepth flag is ignored 
	RHI_API virtual void SetResourceState(RHICommandList* List, EResourceState::Type State, bool ChangeDepth = false, EResourceTransitionMode::Type TransitionMode = EResourceTransitionMode::Direct);
	void MakeReadyForComputeUse(RHICommandList* List, bool Depth = false);
	void MakeReadyForCopy(RHICommandList * list, bool changeDepth = false);
	void MakeReadyForPixel(RHICommandList* List, bool Depth = false);
	RHI_API EResourceState::Type GetCurrentState()const;
	virtual uint64 GetInstanceHash()const;
	RHITexture* GetRenderTexture(int index = 0);
	RHITexture* GetDepthStencil();
protected:
	RHI_API virtual void HandleResize();
	void SetupFences();

	void HandleInit();
	RHI_API void PostInit();
	int CrossGPUBytes = 0;
	bool DidTransferLastFrame = false;
	int m_width = 0;
	int m_height = 0;
	RHI_API bool NeedsSFRResolve() const;
	RHIFrameBufferDesc BufferDesc;
	SFRNode* SFR_Node = nullptr;
	DeviceContext* Device = nullptr;
	RHIGPUSyncEvent* CopyFence = nullptr;
	RHIGPUSyncEvent* DeviceFence = nullptr;
	RHIGPUSyncEvent* TargetCopyFence = nullptr;
	EResourceState::Type CurrentState = EResourceState::Undefined;
};

