#pragma once
#include "Core/Types/FString.h"
#include "Core/IRefCount.h"
#include "Core/EngineTypes.h"




class Shader;
class FrameBuffer;
class DeviceContext;
class IRHIResourse;
class RHITexture;
struct VertexElementDESC;
#define MRT_MAX 8
#define NAME_RHI_PRIMS !BUILD_SHIPPING
enum eTextureDimension
{
	DIMENSION_UNKNOWN = 0,
	DIMENSION_BUFFER = 1,
	DIMENSION_TEXTURE1D = 2,
	DIMENSION_TEXTURE1DARRAY = 3,
	DIMENSION_TEXTURE2D = 4,
	DIMENSION_TEXTURE2DARRAY = 5,
	DIMENSION_TEXTURE2DMS = 6,
	DIMENSION_TEXTURE2DMSARRAY = 7,
	DIMENSION_TEXTURE3D = 8,
	DIMENSION_TEXTURECUBE = 9,
	DIMENSION_TEXTURECUBEARRAY = 10,
	DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE = 11,
};

namespace ECommandListType
{
	enum Type
	{
		Graphics,
		Compute,
		Copy,
		VideoEncode,
		VideoDecode,
		Limit
	};
};

struct  EResourceState
{
	enum Type
	{
		RenderTarget,
		PixelShader,
		Non_PixelShader,
		ComputeUse,
		UAV,
		CopySrc,
		CopyDst,
		Undefined,
		Common,
		ShadingRateImage,
		IndirectArg,
		Limit
	};
	static std::string ToString(EResourceState::Type state);
	static bool IsStateValidForList(ECommandListType::Type listtype, EResourceState::Type state);
};


enum ETextureFormat
{
	FORMAT_UNKNOWN = 0,
	FORMAT_R32G32B32A32_TYPELESS = 1,
	R32G32B32A32_FLOAT = 2,
	R32G32B32A32_UINT = 3,
	R32G32B32A32_SINT = 4,
	R32G32B32_TYPELESS = 5,
	R32G32B32_FLOAT = 6,
	R32G32B32_UINT = 7,
	R32G32B32_SINT = 8,
	FORMAT_R16G16B16A16_TYPELESS = 9,
	FORMAT_R16G16B16A16_FLOAT = 10,
	FORMAT_R16G16B16A16_UNORM = 11,
	FORMAT_R16G16B16A16_UINT = 12,
	FORMAT_R16G16B16A16_SNORM = 13,
	FORMAT_R16G16B16A16_SINT = 14,
	FORMAT_R32G32_TYPELESS = 15,
	R32G32_FLOAT = 16,
	FORMAT_R32G32_UINT = 17,
	FORMAT_R32G32_SINT = 18,
	FORMAT_R32G8X24_TYPELESS = 19,
	FORMAT_D32_FLOAT_S8X24_UINT = 20,
	FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	FORMAT_R10G10B10A2_TYPELESS = 23,
	FORMAT_R10G10B10A2_UNORM = 24,
	FORMAT_R10G10B10A2_UINT = 25,
	FORMAT_R11G11B10_FLOAT = 26,
	FORMAT_R8G8B8A8_TYPELESS = 27,
	FORMAT_R8G8B8A8_UNORM = 28,
	FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	FORMAT_R8G8B8A8_UINT = 30,
	FORMAT_R8G8B8A8_SNORM = 31,
	FORMAT_R8G8B8A8_SINT = 32,
	FORMAT_R16G16_TYPELESS = 33,
	FORMAT_R16G16_FLOAT = 34,
	FORMAT_R16G16_UNORM = 35,
	FORMAT_R16G16_UINT = 36,
	FORMAT_R16G16_SNORM = 37,
	FORMAT_R16G16_SINT = 38,
	FORMAT_R32_TYPELESS = 39,
	FORMAT_D32_FLOAT = 40,
	R32_FLOAT = 41,
	FORMAT_R32_UINT = 42,
	FORMAT_R32_SINT = 43,
	FORMAT_R24G8_TYPELESS = 44,
	FORMAT_D24_UNORM_S8_UINT = 45,
	FORMAT_R24_UNORM_X8_TYPELESS = 46,
	FORMAT_X24_TYPELESS_G8_UINT = 47,
	FORMAT_R8G8_TYPELESS = 48,
	FORMAT_R8G8_UNORM = 49,
	FORMAT_R8G8_UINT = 50,
	FORMAT_R8G8_SNORM = 51,
	FORMAT_R8G8_SINT = 52,
	FORMAT_R16_TYPELESS = 53,
	R16_FLOAT = 54,
	FORMAT_D16_UNORM = 55,
	FORMAT_R16_UNORM = 56,
	R16_UINT = 57,
	FORMAT_R16_SNORM = 58,
	FORMAT_R16_SINT = 59,
	FORMAT_R8_TYPELESS = 60,
	FORMAT_R8_UNORM = 61,
	R8_UINT = 62,
	FORMAT_R8_SNORM = 63,
	FORMAT_R8_SINT = 64,
	FORMAT_A8_UNORM = 65,
	FORMAT_R1_UNORM = 66,
	FORMAT_R9G9B9E5_SHAREDEXP = 67,
	FORMAT_R8G8_B8G8_UNORM = 68,
	FORMAT_G8R8_G8B8_UNORM = 69,
	FORMAT_BC1_TYPELESS = 70,
	FORMAT_BC1_UNORM = 71,
	FORMAT_BC1_UNORM_SRGB = 72,
	FORMAT_BC2_TYPELESS = 73,
	FORMAT_BC2_UNORM = 74,
	FORMAT_BC2_UNORM_SRGB = 75,
	FORMAT_BC3_TYPELESS = 76,
	FORMAT_BC3_UNORM = 77,
	FORMAT_BC3_UNORM_SRGB = 78,
	FORMAT_BC4_TYPELESS = 79,
	FORMAT_BC4_UNORM = 80,
	FORMAT_BC4_SNORM = 81,
	FORMAT_BC5_TYPELESS = 82,
	FORMAT_BC5_UNORM = 83,
	FORMAT_BC5_SNORM = 84,
	FORMAT_B5G6R5_UNORM = 85,
	FORMAT_B5G5R5A1_UNORM = 86,
	FORMAT_B8G8R8A8_UNORM = 87,
	FORMAT_B8G8R8X8_UNORM = 88,
	FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	FORMAT_B8G8R8A8_TYPELESS = 90,
	FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	FORMAT_B8G8R8X8_TYPELESS = 92,
	FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	FORMAT_BC6H_TYPELESS = 94,
	FORMAT_BC6H_UF16 = 95,
	FORMAT_BC6H_SF16 = 96,
	FORMAT_BC7_TYPELESS = 97,
	FORMAT_BC7_UNORM = 98,
	FORMAT_BC7_UNORM_SRGB = 99,
	FORMAT_AYUV = 100,
	FORMAT_Y410 = 101,
	FORMAT_Y416 = 102,
	FORMAT_NV12 = 103,
	FORMAT_P010 = 104,
	FORMAT_P016 = 105,
	FORMAT_420_OPAQUE = 106,
	FORMAT_YUY2 = 107,
	FORMAT_Y210 = 108,
	FORMAT_Y216 = 109,
	FORMAT_NV11 = 110,
	FORMAT_AI44 = 111,
	FORMAT_IA44 = 112,
	FORMAT_P8 = 113,
	FORMAT_A8P8 = 114,
	FORMAT_B4G4R4A4_UNORM = 115,
	FORMAT_FORCE_UINT = 0xffffffff
};

enum PRIMITIVE_TOPOLOGY_TYPE
{
	PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED = 0,
	PRIMITIVE_TOPOLOGY_TYPE_POINT = 1,
	PRIMITIVE_TOPOLOGY_TYPE_LINE = 2,
	PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE = 3,
	PRIMITIVE_TOPOLOGY_TYPE_PATCH = 4
};

namespace EInputClassification
{
	enum Type
	{
		PER_VERTEX = 0,
		PER_INSTANCE = 1
	};
}

struct VertexElementDESC
{
	char* SemanticName;
	unsigned int SemanticIndex;
	ETextureFormat Format;
	uint InputSlot;
	uint AlignedByteOffset;
	EInputClassification::Type InputSlotClass;
	uint InstanceDataStepRate;
	int Stride = 0;
};

namespace EDeviceContextQueue
{
	enum Type
	{
		Graphics,
		Compute,
		Copy,//used to move resources from Host to this GPU
		InterCopy,//Used to Transfer Resources From other GPUS Via HOST
		LIMIT
	};
	EDeviceContextQueue::Type GetFromCommandListType(ECommandListType::Type listType);
}

//todo: remove this
namespace EBufferResourceState
{
	enum Type
	{
		UnorderedAccess,
		Read,
		IndirectArgs,
		Non_PixelShader,
		CopySrc,
		CopyDST,
		Limit
	};
}


enum COMPARISON_FUNC
{
	COMPARISON_FUNC_NEVER = 1,
	COMPARISON_FUNC_LESS = 2,
	COMPARISON_FUNC_EQUAL = 3,
	COMPARISON_FUNC_LESS_EQUAL = 4,
	COMPARISON_FUNC_GREATER = 5,
	COMPARISON_FUNC_NOT_EQUAL = 6,
	COMPARISON_FUNC_GREATER_EQUAL = 7,
	COMPARISON_FUNC_ALWAYS = 8
};
namespace EGPUTIMERS
{
	enum Type
	{
		Total,
		GPU0WaitOnGPU1,
		Text,
		LIMIT
	};
}
namespace EGPUCOPYTIMERS
{
	enum Type
	{
		Total = EGPUTIMERS::LIMIT,
		MGPUCopy,
		ShadowCopy,
		ShadowCopy2,
		SFRMerge,
		LIMIT
	};
}
namespace ERHIBufferType
{
	enum Type
	{
		Vertex,
		Index,
		Constant,
		GPU,
		ReadBack
	};
}

//Render passes
struct ERenderPassStoreOp
{
	enum Type
	{
		Store,
		DontCare,
		Limit
	};
};

struct ERenderPassLoadOp
{
	enum Type
	{
		Load,
		Clear,
		DontCare,
		Limit
	};
};


struct RHI_API RHIPipeRenderTargetDesc
{
	ETextureFormat RTVFormats[8] = { ETextureFormat::FORMAT_UNKNOWN };
	ETextureFormat DSVFormat = ETextureFormat::FORMAT_UNKNOWN;
	int NumRenderTargets = 0;
	bool operator==(const RHIPipeRenderTargetDesc other) const;
	static RHIPipeRenderTargetDesc GetDefault();
};

enum TMP_BlendMode
{
	Text,
	Full
};

struct RHIRenderOutputSet
{
	RHITexture* Targets[MRT_MAX] = { nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };
	RHITexture* DepthTarget = nullptr;
	RHIPipeRenderTargetDesc GetRTDesc();
};

//defines a render pass pointers to framebuffer optional
struct RHIRenderPassDesc
{
	RHIRenderPassDesc() {}
	RHIRenderPassDesc(FrameBuffer* buffer, ERenderPassLoadOp::Type LoadOp = ERenderPassLoadOp::Load);
	FrameBuffer* TargetBuffer = nullptr;
	FrameBuffer* DepthSourceBuffer = nullptr;

	ERenderPassLoadOp::Type LoadOp = ERenderPassLoadOp::Clear;
	ERenderPassStoreOp::Type StoreOp = ERenderPassStoreOp::Store;

	ERenderPassLoadOp::Type StencilLoadOp = ERenderPassLoadOp::DontCare;
	ERenderPassStoreOp::Type StencilStoreOp = ERenderPassStoreOp::DontCare;

	EResourceState::Type InitalState = EResourceState::RenderTarget;
	EResourceState::Type FinalState = EResourceState::RenderTarget;
	RHIPipeRenderTargetDesc RenderDesc;
	int SubResourceIndex = 0;
	RHI_API void Build();
	RHI_API bool operator==(const RHIRenderPassDesc other)const;

	bool TargetSwapChain = false;
	size_t GetHash();
};
namespace RHIBlendMode
{
	enum Type
	{
		BLEND_ZERO = 1,
		BLEND_ONE = 2,
		BLEND_SRC_COLOR = 3,
		BLEND_INV_SRC_COLOR = 4,
		BLEND_SRC_ALPHA = 5,
		BLEND_INV_SRC_ALPHA = 6,
		BLEND_DEST_ALPHA = 7,
		BLEND_INV_DEST_ALPHA = 8,
		BLEND_DEST_COLOR = 9,
		BLEND_INV_DEST_COLOR = 10,
		BLEND_SRC_ALPHA_SAT = 11,
		BLEND_BLEND_FACTOR = 14,
		BLEND_INV_BLEND_FACTOR = 15,
		BLEND_SRC1_COLOR = 16,
		BLEND_INV_SRC1_COLOR = 17,
		BLEND_SRC1_ALPHA = 18,
		BLEND_INV_SRC1_ALPHA = 19
	};
};

namespace RHIBlendOp
{
	enum Type
	{
		BLEND_OP_ADD = 1,
		BLEND_OP_SUBTRACT = 2,
		BLEND_OP_REV_SUBTRACT = 3,
		BLEND_OP_MIN = 4,
		BLEND_OP_MAX = 5
	};
};


namespace COLOR_MASK
{
	enum Type
	{
		COLOR_WRITE_ENABLE_RED = 1,
		COLOR_WRITE_ENABLE_GREEN = 2,
		COLOR_WRITE_ENABLE_BLUE = 4,
		COLOR_WRITE_ENABLE_ALPHA = 8,
		COLOR_WRITE_ENABLE_ALL = (((COLOR_WRITE_ENABLE_RED | COLOR_WRITE_ENABLE_GREEN) | COLOR_WRITE_ENABLE_BLUE) | COLOR_WRITE_ENABLE_ALPHA)
	};
};

struct RHIRender_Target_Blend_Desc
{
	bool BlendEnable = false;
	bool LogicOpEnable = false;
	RHIBlendMode::Type SrcBlend = RHIBlendMode::BLEND_ONE;
	RHIBlendMode::Type DestBlend = RHIBlendMode::BLEND_ZERO;
	RHIBlendOp::Type BlendOp = RHIBlendOp::BLEND_OP_ADD;
	RHIBlendMode::Type SrcBlendAlpha = RHIBlendMode::BLEND_ONE;
	RHIBlendMode::Type DestBlendAlpha = RHIBlendMode::BLEND_ZERO;
	RHIBlendOp::Type BlendOpAlpha = RHIBlendOp::BLEND_OP_ADD;
	//LOGIC_OP LogicOp;
	uint8_t RenderTargetWriteMask = COLOR_MASK::COLOR_WRITE_ENABLE_ALL;
};

struct RHIBlendState
{
	bool AlphaToCoverageEnable = false;
	bool IndependentBlendEnable = false;
	RHIRender_Target_Blend_Desc RenderTargetDescs[MRT_MAX] = {};
	static RHIBlendState CreateText();
	static RHIBlendState CreateBlendDefault();
};
struct RHIRasterizerDesc
{
	bool Cull = false;
	bool ConservativeRaster = false;
};

typedef
enum DEPTH_WRITE_MASK
{
	DEPTH_WRITE_MASK_ZERO = 0,
	DEPTH_WRITE_MASK_ALL = 1
} 	DEPTH_WRITE_MASK;

typedef
enum STENCIL_OP
{
	STENCIL_OP_KEEP = 1,
	STENCIL_OP_ZERO = 2,
	STENCIL_OP_REPLACE = 3,
	STENCIL_OP_INCR_SAT = 4,
	STENCIL_OP_DECR_SAT = 5,
	STENCIL_OP_INVERT = 6,
	STENCIL_OP_INCR = 7,
	STENCIL_OP_DECR = 8
} 	STENCIL_OP;

typedef struct DEPTH_STENCILOP_DESC
{
	STENCIL_OP StencilFailOp = STENCIL_OP_KEEP;
	STENCIL_OP StencilDepthFailOp = STENCIL_OP_KEEP;
	STENCIL_OP StencilPassOp = STENCIL_OP_KEEP;
	COMPARISON_FUNC StencilFunc = COMPARISON_FUNC_ALWAYS;
} 	DEPTH_STENCILOP_DESC;

struct RHIDepthStencilDesc
{
	bool DepthEnable = true;
	bool DepthWrite = true;
	bool StencilEnable = false;
	//DEPTH_WRITE_MASK DepthWriteMask;
	COMPARISON_FUNC DepthCompareFunction = COMPARISON_FUNC::COMPARISON_FUNC_LESS_EQUAL;
	//UINT8 StencilReadMask;
	//UINT8 StencilWriteMask;
	DEPTH_STENCILOP_DESC FrontFace;
	DEPTH_STENCILOP_DESC BackFace;
};

struct ViewInstancingMode
{
	bool Active = false;
	uint32 Instances = 6;
	bool NV_UseSMP = false;
};
struct RHIInputLayout
{
	std::vector<VertexElementDESC> Elements;
};
struct  RHIPipeLineStateDesc
{
	//Hold both root signature and shader blobs
	Shader* ShaderInUse = nullptr;
	void InitOLD(bool Depth, bool shouldcull, bool Blend);
	bool Cull = true;
	RHIBlendState BlendState;
	RHIRasterizerDesc RasterizerState;
	PRIMITIVE_TOPOLOGY_TYPE RasterMode = PRIMITIVE_TOPOLOGY_TYPE::PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	RHIPipeRenderTargetDesc RenderTargetDesc = RHIPipeRenderTargetDesc();
	RHIDepthStencilDesc DepthStencilState;
	int SampleCount = 1;

	//Validation in cases without a validation layer or engine limitations (e.g. multi-GPU)
	RHI_API bool Validate();
	DeviceContext* OwningDevice = nullptr;
	RHI_API size_t GetHash();
	RHI_API void CalulateHash();
	RHI_API bool operator==(const RHIPipeLineStateDesc other)const;
	static RHIPipeLineStateDesc CreateDefault(Shader* shader, FrameBuffer* FB = nullptr);
	RHI_API void Build();
	RHI_API std::string GetString();
	class RHIRenderPass* RenderPass = nullptr;
	ViewInstancingMode ViewInstancing;
	RHIRenderPassDesc RenderPassDesc;
	bool EnableDepthBoundsTest = false;
	RHIInputLayout InputLayout = RHIInputLayout();
private:
	uint64 UniqueHash = 0;
	std::string StringPreHash;
};

class  RHIPipeLineStateObject
{
public:
	RHI_API RHIPipeLineStateObject(const RHIPipeLineStateDesc& desc);
	RHI_API virtual ~RHIPipeLineStateObject();
	RHI_API size_t GetDescHash();
	RHI_API std::string GetDescString();
	RHI_API virtual void Complie();
	RHI_API bool IsReady() const;
	RHI_API const RHIPipeLineStateDesc& GetDesc();
	RHI_API DeviceContext* GetDevice()const;
	RHI_API virtual void Release();
protected:
	bool IsComplied = false;
	RHIPipeLineStateDesc Desc;
protected:
	DeviceContext* Device = nullptr;
};

namespace EFrameBufferSizeMode
{
	enum Type
	{
		Fixed,//Resize is handled explicitly 
		LinkedToRenderScale, //the Scale of the renderer
		LinkedToScreenSize, //Linked to the screen size
		LinkedToRenderScale_TileSize,//size based on the render scale split into VRS tiles
		Limit
	};
};

namespace EViewType
{
	enum Type
	{
		UAV,
		SRV,
		Limit
	};
}
struct RHIViewDesc
{
	int ArraySlice = 0;
	int Mip = 0;
	int MipLevels = -1;
	int ResourceIndex = 0;
	int Offset = 0;
	uint FirstElement = 0;
	EViewType::Type ViewType = EViewType::Limit;
	eTextureDimension Dimension = DIMENSION_UNKNOWN;
	bool UseResourceFormat = true;
	ETextureFormat Format = ETextureFormat::FORMAT_UNKNOWN;
	static RHIViewDesc CreateUAV(int Resource)
	{
		RHIViewDesc D;
		D.ViewType = EViewType::UAV;
		D.ResourceIndex = Resource;
		return D;
	}
	static RHIViewDesc DefaultUAV(eTextureDimension dimention = DIMENSION_UNKNOWN)
	{
		RHIViewDesc D;
		D.ViewType = EViewType::UAV;
		D.Dimension = dimention;
		return D;
	}
	static RHIViewDesc DefaultSRV(eTextureDimension dimention = DIMENSION_UNKNOWN)
	{
		RHIViewDesc D;
		D.ViewType = EViewType::SRV;
		D.Dimension = dimention;
		return D;
	}
	RHI_API bool operator==(const RHIViewDesc other)const;
	uint32 OffsetInDescriptor = 0;
};

struct FrameBufferVariableRateSettings
{
	enum VRateMode
	{
		None,
		VRR,//Raster at coarse rate
		Limit
	};
	FrameBufferVariableRateSettings::VRateMode BufferMode = FrameBufferVariableRateSettings::None;
	bool UsesVRR()const;
};
struct RHIFrameBufferDesc
{
public:
	static RHIFrameBufferDesc CreateColour(int width, int height);
	static RHIFrameBufferDesc CreateDepth(int width, int height);
	static RHIFrameBufferDesc CreateCubeDepth(int width, int height);
	static RHIFrameBufferDesc CreateCubeColourDepth(int width, int height);
	TEMP_API static RHIFrameBufferDesc CreateColourDepth(int width, int height);
	static RHIFrameBufferDesc CreateGBuffer(int width, int height);
	RHIFrameBufferDesc()
	{
	};
	RHIFrameBufferDesc(int width, int height, ETextureFormat format, eTextureDimension dimension)
	{
		RTFormats[0] = format;
		Width = width;
		Height = height;
		Dimension = dimension;
		MaxSize.x = width;
		MaxSize.y = height;
	}
	ETextureFormat RTFormats[MRT_MAX] = {};
	ETextureFormat DepthFormat = ETextureFormat::FORMAT_D32_FLOAT;
	ETextureFormat DepthReadFormat = ETextureFormat::R32_FLOAT;
	int Width = 0;
	int Height = 0;
	int TextureDepth = 1;
	int RenderTargetCount = 1;
	bool NeedsDepthStencil = false;
	bool AllowUnorderedAccess = false;
	//If set to 0 the resource will be auto mipped
	int MipsToGenerate = 1;
	eTextureDimension Dimension = eTextureDimension::DIMENSION_TEXTURE2D;
	glm::vec4 clearcolour = glm::vec4(0.0f, 0.2f, 0.4f, 1.0f);
	float DepthClearValue = 1.0f;
	int Samples = 0;
	int MipCount = 1;
	int DepthMipCount = 1;
	bool CubeMapAddressAsOne = true;
	EResourceState::Type SimpleStartingState = EResourceState::Common;
	glm::vec4 ViewPort = glm::vec4();
	glm::vec4 ScissorRect = glm::vec4();
	bool IncludedInSFR = false;
	float LinkToBackBufferScaleFactor = 1.0f;
	int SFR_FullWidth = 0;

	RHIPipeRenderTargetDesc GetRTDesc();
	bool AllowDynamicResize = false;
	glm::ivec2 MaxSize = glm::ivec2(0, 0);
	FrameBuffer* SharedDepthStencilSource = nullptr;
	EFrameBufferSizeMode::Type SizeMode = EFrameBufferSizeMode::Fixed;
	FrameBufferVariableRateSettings VarRateSettings;

	RHITexture* DepthStencil = nullptr;
	RHITexture* RenderTargets[MRT_MAX] = {};
	bool HasStencil()const;
};

class  IRHIResourse : public IRefCount
{
public:
	RHI_API virtual ~IRHIResourse();
	RHI_API virtual void Release();
	RHI_API bool IsPendingKill() const
	{
		return PendingKill;
	}
	bool IsReleased = false;
	RHI_API void SetDebugName(std::string Name);
	const char* ObjectSufix = "";
#if NAME_RHI_PRIMS	
	RHI_API const char* GetDebugName();
#endif

private:
#if NAME_RHI_PRIMS
	const char* FinalName = "";
	const char* DebugName = "";
#endif
	bool PendingKill = false;
	friend class RHI;
	uint Karma = 253;
};
//Releases the GPU side and deletes the CPU object
#define SafeRHIRelease(Target) if(Target != nullptr){Target->Release(); delete Target; Target= nullptr;}
#define EnqueueSafeRHIRelease(Target) if(Target != nullptr){RHI::AddToDeferredDeleteQueue(Target); Target = nullptr;}
#define EnqueueSafeArrayRelease(x,length) for(int i= 0; i < length;i++){EnqueueSafeRHIRelease(x[i]);}
#if NAME_RHI_PRIMS
#define NAME_RHI_OBJECT(x) x->SetDebugName(#x);
#else
#define NAME_RHI_OBJECT(x)
#endif
struct IndirectDrawArgs
{
	int VertexCountPerInstance;
	int InstanceCount;
	int StartVertexLocation;
	int StartInstanceLocation;
};

struct IndirectDispatchArgs
{
	int ThreadGroupCountX;
	int ThreadGroupCountY;
	int ThreadGroupCountZ;
};

struct SFRNode
{
	float SFR_PercentSize = 0.5f;
	float SFR_VerticalPercentSize = 1.0f;
	float SFR_Offset = 0.5f;
	float SFR_VerticalOffset = 1.0f;
	struct TimerData* SFRDriveTimer = nullptr;
	std::vector<FrameBuffer*> NodesBuffers;
	void AddBuffer(FrameBuffer* b);
	//Link to framebuffers - then call resize!
};

struct RHITextureDesc
{
	bool InitOnALLDevices = true;
	bool IsCubeMap = false;
};


namespace EBufferAccessType
{
	enum Type
	{
		Static,
		Dynamic,
		GPUOnly,
		ReadBack
	};
};

class RHI_API RHIUtils
{
public:
	static size_t BitsPerPixel(ETextureFormat fmt);
	static size_t GetPixelSize(ETextureFormat format);
	static size_t GetComponentCount(ETextureFormat fmt);
};


namespace ESamplerWrapMode
{
	enum Type
	{
		TEXTURE_ADDRESS_MODE_WRAP = 1,
		TEXTURE_ADDRESS_MODE_MIRROR = 2,
		TEXTURE_ADDRESS_MODE_CLAMP = 3,
		TEXTURE_ADDRESS_MODE_BORDER = 4,
		TEXTURE_ADDRESS_MODE_MIRROR_ONCE = 5
	};
};

namespace ESamplerFilterMode
{
	enum Type
	{
		FILTER_MIN_MAG_MIP_POINT = 0,
		FILTER_MIN_MAG_POINT_MIP_LINEAR = 0x1,
		FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
		FILTER_MIN_POINT_MAG_MIP_LINEAR = 0x5,
		FILTER_MIN_LINEAR_MAG_MIP_POINT = 0x10,
		FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
		FILTER_MIN_MAG_LINEAR_MIP_POINT = 0x14,
		FILTER_MIN_MAG_MIP_LINEAR = 0x15,
		FILTER_ANISOTROPIC = 0x55,
		FILTER_COMPARISON_MIN_MAG_MIP_POINT = 0x80,
		FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
		FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
		FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
		FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
		FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
		FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
		FILTER_COMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
		FILTER_COMPARISON_ANISOTROPIC = 0xd5,
		FILTER_MINIMUM_MIN_MAG_MIP_POINT = 0x100,
		FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x101,
		FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x104,
		FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x105,
		FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x110,
		FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x111,
		FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x114,
		FILTER_MINIMUM_MIN_MAG_MIP_LINEAR = 0x115,
		FILTER_MINIMUM_ANISOTROPIC = 0x155,
		FILTER_MAXIMUM_MIN_MAG_MIP_POINT = 0x180,
		FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x181,
		FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x184,
		FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x185,
		FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x190,
		FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x191,
		FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x194,
		FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR = 0x195,
		FILTER_MAXIMUM_ANISOTROPIC = 0x1d5
	};
};


struct RHISamplerDesc
{
	ESamplerFilterMode::Type FilterMode = ESamplerFilterMode::FILTER_MIN_MAG_MIP_LINEAR;
	ESamplerWrapMode::Type UAddressMode = ESamplerWrapMode::TEXTURE_ADDRESS_MODE_WRAP;
	ESamplerWrapMode::Type VAddressMode = ESamplerWrapMode::TEXTURE_ADDRESS_MODE_WRAP;
	ESamplerWrapMode::Type WAddressMode = ESamplerWrapMode::TEXTURE_ADDRESS_MODE_WRAP;
	int ShaderRegister = 0;
	int MaxAnisotropy = 0;
	RHISamplerDesc()
	{
	}
	RHISamplerDesc(ESamplerFilterMode::Type filter, ESamplerWrapMode::Type WrapMode, int Reg);
	RHI_API static std::vector<RHISamplerDesc> GetDefault();
};


struct RayArgs
{
	uint RayFlags = 0;
	uint RayMask = 0xFF;
};
struct RHIRayDispatchDesc
{
	RHIRayDispatchDesc() {};
	RHIRayDispatchDesc(FrameBuffer* RB);
	int Width = 0;
	int Height = 0;
	int Depth = 1;
	FrameBuffer* Target = nullptr;
	RayArgs RayArguments;
	bool PushRayArgs = false;
};

namespace EDeviceIndex
{
	enum Type
	{
		Device_0,
		Device_1,
		Device_2,
		Limit
	};
};

namespace VRX_SHADING_RATE
{
	enum type
	{
		SHADING_RATE_1X1 = 0,
		SHADING_RATE_1X2 = 0x1,
		SHADING_RATE_2X1 = 0x4,
		SHADING_RATE_2X2 = 0x5,
		SHADING_RATE_2X4 = 0x6,
		SHADING_RATE_4X2 = 0x9,
		SHADING_RATE_4X4 = 0xa
	};
}
namespace AS_BUILD_FLAGS
{
	enum Flags
	{
		Fast_Trace = 1 << 1,
		Fast_Build = 1 << 2,
		AllowUpdate = 1 << 3,
		Limit
	};
}

struct AccelerationStructureDesc
{
	AccelerationStructureDesc();
	int MaxSize = 0;
	int BuildFlags = AS_BUILD_FLAGS::Fast_Trace;
};



enum RAY_FLAGS
{
	RAY_FLAG_NONE = 0,
	RAY_FLAG_FORCE_OPAQUE = 0x1,
	RAY_FLAG_FORCE_NON_OPAQUE = 0x2,
	RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH = 0x4,
	RAY_FLAG_SKIP_CLOSEST_HIT_SHADER = 0x8,
	RAY_FLAG_CULL_BACK_FACING_TRIANGLES = 0x10,
	RAY_FLAG_CULL_FRONT_FACING_TRIANGLES = 0x20,
	RAY_FLAG_CULL_OPAQUE = 0x40,
	RAY_FLAG_CULL_NON_OPAQUE = 0x80
};

namespace INDIRECT_ARGUMENT_TYPE
{
	enum Type
	{
		INDIRECT_ARGUMENT_TYPE_DRAW = 0,
		INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED,
		INDIRECT_ARGUMENT_TYPE_DISPATCH,
		INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW,
		INDIRECT_ARGUMENT_TYPE_INDEX_BUFFER_VIEW,
		INDIRECT_ARGUMENT_TYPE_CONSTANT,
		INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW,
		INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW,
		INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW,
		LIMIT
	};
};

struct INDIRECT_ARGUMENT_DESC
{
	INDIRECT_ARGUMENT_TYPE::Type Type;
	union
	{
		struct
		{
			uint Slot;
		} 	VertexBuffer;
		struct
		{
			uint RootParameterIndex;
			uint DestOffsetIn32BitValues;
			uint Num32BitValuesToSet;
		} 	Constant;
		struct
		{
			uint RootParameterIndex;
		} 	ConstantBufferView;
		struct
		{
			uint RootParameterIndex;
		} 	ShaderResourceView;
		struct
		{
			uint RootParameterIndex;
		} 	UnorderedAccessView;
	};
};
struct RHICommandSignitureDescription
{
	RHIPipeLineStateObject* PSO;
	std::vector<INDIRECT_ARGUMENT_DESC> ArgumentDescs;
	uint CommandBufferStide = 0;
	bool IsCompute = false;

};

class RHICommandSigniture : public IRHIResourse
{
public:
	RHI_API RHICommandSigniture(DeviceContext* context, RHICommandSignitureDescription desc = RHICommandSignitureDescription());
	RHI_API virtual ~RHICommandSigniture() {}
	RHI_API virtual void Build() {};
	const RHICommandSignitureDescription& GetDesc()const;
protected:
	DeviceContext* Context = nullptr;
	RHICommandSignitureDescription RHIdesc;
};

struct EResourceTransitionMode
{
	enum Type
	{
		Direct,
		Start,
		End,
		Limit
	};
};


struct EShadingImageSetMode
{
	enum Type
	{
		SetOnly,
		SetForResolve,
		SetForRender,
		Limit
	};

};