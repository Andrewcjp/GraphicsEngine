#include "GPUTextureStreamer.h"
#include "TextureStreamingEngine.h"


GPUTextureStreamer::GPUTextureStreamer()
{}


GPUTextureStreamer::~GPUTextureStreamer()
{}

void GPUTextureStreamer::Init(DeviceContext * Con)
{
	CmdList = RHI::CreateCommandList(ECommandListType::Graphics, Con);
	if (TextureStreamingEngine::Get() != nullptr)
	{
		TextureStreamingEngine::Get()->RegisterGPUStreamer(this);
	}
	Context = Con;
	OnInit(Con);
	SetStreamingMode(TextureStreamingEngine::Get()->GetStreamingMode());
}

void GPUTextureStreamer::Update()
{
	Tick(CmdList);
}

void GPUTextureStreamer::RealiseHandle(TextureHandle* handle)
{
	ensure(handle->IsCPULoaded)
	Handles.push_back(handle);
	OnRealiseTexture(handle);
	handle->SetupForSFS(Context);
}

void GPUTextureStreamer::SetTargetSize(uint64 size)
{
	TargetPoolSize = size;
}

void GPUTextureStreamer::SetStreamingMode(EGPUSteamMode::Type mode)
{
	m_StreamingMode = mode;
}

void GPUTextureStreamer::UpdateMappingsFromFeedback(RHICommandList* list, TextureHandle* handle)
{

}

void GPUTextureStreamer::Tick(RHICommandList* list)
{}

void GPUTextureStreamer::OnInit(DeviceContext* Con)
{

}
