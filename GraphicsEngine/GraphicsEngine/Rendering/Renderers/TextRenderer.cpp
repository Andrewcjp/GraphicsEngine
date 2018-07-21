#include "TextRenderer.h"
#include "glm\glm.hpp"
#include "RHI/RHI.h"
#include "RHI/DeviceContext.h"
#include "Rendering/Shaders/Text_Shader.h"
#include <algorithm>
#include <cstring>
#include "Core/Engine.h"
#include "RHI/BaseTexture.h"
#include "Core/Utils/FileUtils.h"
#define MAXWIDTH 1024
TextRenderer* TextRenderer::instance = nullptr;
#include "Rendering/PostProcessing/PostProcessing.h"
#include "RHI/RenderAPIs/D3D12/D3D12RHI.h"
#include "RHI/RenderAPIs/D3D12/D3D12Framebuffer.h"
#include "RHI/RenderAPIs/D3D12/D3D12CommandList.h"
#include "RHI/RenderAPIs/D3D12/D3D12TimeManager.h"
#include "Core/Performance/PerfManager.h"
TextRenderer::TextRenderer(int width, int height)
{
	m_width = width;
	m_height = height;
	m_TextShader = new Text_Shader(RHI::GetDeviceContext(RunOnSecondDevice));
	m_TextShader->Height = m_height;
	m_TextShader->Width = m_width;
	LoadText();
	instance = this;
	coords.reserve(100 * 6);
}


TextRenderer::~TextRenderer()
{
	delete TextAtlas;
	delete m_TextShader;
	delete VertexBuffer;
	coords.empty();

	if (Renderbuffer != nullptr)
	{
		delete Renderbuffer;
	}
	delete TextCommandList;
}

void TextRenderer::RenderFromAtlas(std::string text, float x, float y, float scale, glm::vec3 color, bool reset/* = true*/)
{
	if (reset)
	{
		Reset();
	}
	m_TextShader->SetShaderActive();
	m_TextShader->Height = m_height;
	m_TextShader->Width = m_width;
	scale = scale * ScaleFactor;
	if (UseFrameBuffer)
	{
		TextCommandList->SetRenderTarget(Renderbuffer);
		if (NeedsClearRT)
		{
			TextCommandList->ClearFrameBuffer(Renderbuffer);
			NeedsClearRT = false;
		}
	}
	else
	{
		TextCommandList->SetScreenBackBufferAsRT();
	}

	m_TextShader->Update(TextCommandList);
	const uint8_t *p;
	atlas* a = TextAtlas;
	size_t TargetDatalength = 6 * text.length();
	if (coords.size() < currentsize + TargetDatalength)
	{
		coords.resize(currentsize + TargetDatalength);
		ensure(MAX_BUFFER_SIZE > (coords.size() / 6));
	}
	int c = currentsize;

	/* Loop through all characters */
	for (p = (const uint8_t *)text.c_str(); *p; p++)
	{
		/* Calculate the vertex and texture coordinates */
		float x2 = x + a->c[*p].bl * scale;
		float y2 = -y - a->c[*p].bt * scale;
		float w = a->c[*p].bw * scale;
		float h = a->c[*p].bh * scale;

		/* Advance the cursor to the start of the next character */
		x += a->c[*p].ax * scale;
		y += a->c[*p].ay * scale;

		/* Skip glyphs that have no pixels */
		if (!w || !h)
		{
			continue;
		}

		coords[c++] =
		{
			x2, -y2, a->c[*p].tx, a->c[*p].ty,color
		};
		coords[c++] =
		{
			x2 + w, -y2, a->c[*p].tx + a->c[*p].bw / a->w, a->c[*p].ty,color
		};
		coords[c++] =
		{
			x2, -y2 - h, a->c[*p].tx, a->c[*p].ty + a->c[*p].bh / a->h,color
		};
		coords[c++] =
		{
			x2 + w, -y2, a->c[*p].tx + a->c[*p].bw / a->w, a->c[*p].ty,color
		};
		coords[c++] =
		{
			x2, -y2 - h, a->c[*p].tx, a->c[*p].ty + a->c[*p].bh / a->h,color
		};
		coords[c++] =
		{
			x2 + w, -y2 - h, a->c[*p].tx + a->c[*p].bw / a->w, a->c[*p].ty + a->c[*p].bh / a->h,color
		};
		currentsize += 6;
	}
	if (reset)
	{
		VertexBuffer->UpdateVertexBuffer(coords.data(), sizeof(point)*((int)text.length() * 6));
	}
	else
	{
		VertexBuffer->UpdateVertexBuffer(coords.data(), sizeof(point)*(currentsize));
	}

	TextCommandList->SetVertexBuffer(VertexBuffer);
	TextCommandList->SetTexture(TextAtlas->Texture, 0);
	TextCommandList->DrawPrimitive(c, 1, 0, 0);

	///* Draw all the character on the screen in one go */
	if (reset)
	{
		Finish();
	}

}

void TextRenderer::Finish()
{
	TextCommandList->SetRenderTarget(nullptr);
	TextCommandList->GetDevice()->GetTimeManager()->EndTimer(TextCommandList, D3D12TimeManager::eGPUTIMERS::Text);
	TextCommandList->GetDevice()->GetTimeManager()->EndTotalGPUTimer(TextCommandList);
	TextCommandList->Execute();
	TextCommandList->GetDevice()->InsertGPUWait(DeviceContextQueue::InterCopy, DeviceContextQueue::Graphics);
	currentsize = 0;

	if (RunOnSecondDevice)
	{
#if 0
		PerfManager::StartTimer("RunOnSecondDevice");
		DeviceContext* HostDevice = RHI::GetDeviceContext(1);
		DeviceContext* TargetDevice = RHI::GetDeviceContext(0);
		
		RHICommandList* CopyList = HostDevice->GetInterGPUCopyList();
		CopyList->ResetList();
		CopyList->CopyResourceToSharedMemory(Renderbuffer);
		CopyList->Execute(DeviceContextQueue::InterCopy);
		HostDevice->InsertGPUWait(DeviceContextQueue::Graphics, DeviceContextQueue::InterCopy);
		RHI::GetDeviceContext(1)->GPUWaitForOtherGPU(RHI::GetDeviceContext(0), DeviceContextQueue::Graphics, DeviceContextQueue::Graphics);
		TargetDevice->InsertGPUWait(DeviceContextQueue::InterCopy, DeviceContextQueue::Graphics);
		
		CopyList = TargetDevice->GetInterGPUCopyList();
		CopyList->ResetList();
		CopyList->CopyResourceFromSharedMemory(Renderbuffer);
		CopyList->Execute(DeviceContextQueue::InterCopy);
		TargetDevice->InsertGPUWait(DeviceContextQueue::Graphics, DeviceContextQueue::InterCopy);
		PerfManager::EndTimer("RunOnSecondDevice");
#else
		FrameBuffer::CopyHelper(Renderbuffer, RHI::GetDeviceContext(0));
#endif
	}
}
void TextRenderer::Reset()
{
	TextCommandList->ResetList();
	TextCommandList->GetDevice()->GetTimeManager()->StartTimer(TextCommandList, D3D12TimeManager::eGPUTIMERS::Text);
	if (RunOnSecondDevice)
	{
		TextCommandList->GetDevice()->GetTimeManager()->StartTotalGPUTimer(TextCommandList);
	}
	currentsize = 0;
}

void TextRenderer::LoadText()
{
	VertexBuffer = RHI::CreateRHIBuffer(RHIBuffer::BufferType::Vertex, RHI::GetDeviceContext(RunOnSecondDevice));
	VertexBuffer->CreateVertexBuffer(sizeof(point), (sizeof(point) * 6) * MAX_BUFFER_SIZE, RHIBuffer::BufferAccessType::Dynamic);//max text length?

	TextCommandList = RHI::CreateCommandList(ECommandListType::Graphics, RHI::GetDeviceContext(RunOnSecondDevice));
	TextCommandList->SetPipelineState(PipeLineState{ false,false ,true });
	TextCommandList->CreatePipelineState(m_TextShader);
	if (UseFrameBuffer)
	{
		RHIFrameBufferDesc desc = RHIFrameBufferDesc::CreateColour(1280, 720);
		desc.clearcolour = glm::vec4(0.0f, 0.2f, 0.4f, 0.0f);
		if (RunOnSecondDevice)
		{
			desc.IsShared = true;
			desc.DeviceToCopyTo = RHI::GetDeviceContext(0);
		}
		Renderbuffer = RHI::CreateFrameBuffer(RHI::GetDeviceContext(RunOnSecondDevice), desc);
		PostProcessing::Instance->AddCompostPass(Renderbuffer);
		if (D3D12RHI::Instance)
		{
			D3D12RHI::Instance->AddLinkedFrameBuffer(Renderbuffer);
			if (RunOnSecondDevice)
			{
				Renderbuffer->SetupCopyToDevice(RHI::GetDeviceContext(0));
			}
		}
	}

	if (FT_Init_FreeType(&ft))
	{
		Log::OutS  << "ERROR::FREETYPE: Could not init FreeType Library" << Log::OutS;
	}
	std::string fontpath = Engine::GetRootDir();
	fontpath.append("\\asset\\fonts\\arial.ttf");
	if (FT_New_Face(ft, fontpath.c_str(), 0, &face))
	{
		Log::OutS  << "ERROR::FREETYPE: Failed to load font" << Log::OutS;
	}
	float scale = 1;
	int facesize = 48 * std::lround(scale);//48
	ScaleFactor = (float)1 / scale;
	FT_Set_Pixel_Sizes(face, 0, facesize);

	// Load first 128 characters of ASCII set
	//Freetype is awesome!
	TextAtlas = new atlas(face, facesize, RunOnSecondDevice);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	//Say thanks you to Free type for being very useful

}

void TextRenderer::UpdateSize(int width, int height)
{
	m_width = width;
	m_height = height;
	if (m_TextShader != nullptr)
	{
		m_TextShader->Height = m_height;
		m_TextShader->Width = m_width;
	}
}

void TextRenderer::NotifyFrameEnd()
{
	NeedsClearRT = true;
}

TextRenderer::atlas::atlas(FT_Face face, int height, bool RunOnSecondDevice)
{

	FT_Set_Pixel_Sizes(face, 0, height);
	FT_GlyphSlot g = face->glyph;

	unsigned int roww = 0;
	unsigned int rowh = 0;
	w = 0;
	h = 0;

	memset(c, 0, sizeof c);

	/* Find minimum size for a texture holding all visible ASCII characters */
	for (int i = 0; i < 128; i++)
	{
		if (FT_Load_Char(face, i, FT_LOAD_RENDER))
		{
			fprintf(stderr, "Loading character %c failed!\n", i);
			continue;
		}
		if (roww + g->bitmap.width + 1 >= MAXWIDTH)
		{
			w = std::max(w, roww);
			h += rowh;
			roww = 0;
			rowh = 0;
		}
		roww += g->bitmap.width + 1;
		rowh = std::max(rowh, (unsigned int)g->bitmap.rows);
	}

	w = std::max(w, roww);
	h += rowh;

	/* Paste all glyph bitmaps into the texture, remembering the offset */
	int ox = 0;
	int oy = 0;

	rowh = 0;
	unsigned char* FinalData = new unsigned char[w*h];
	for (unsigned int i = 0; i < w*h; i++)
	{
		FinalData[i] = '\0';
	}
	int soff = 0;
	for (int i = 0; i < 128; i++)
	{
		if (FT_Load_Char(face, i, FT_LOAD_RENDER))
		{
			fprintf(stderr, "Loading character %c failed!\n", i);
			continue;
		}

		if (ox + g->bitmap.width + 1 >= MAXWIDTH)
		{
			oy += rowh;
			rowh = 0;
			ox = 0;
		}

		int offset = ((ox + oy * h));
		//scan into texture
		int lastoff = 0;
		for (int suby = 0; suby < g->bitmap.rows; suby++)
		{
			int neo = ((ox + (oy + suby)*w));
			memcpy((FinalData + neo), g->bitmap.buffer + (suby*g->bitmap.width), (g->bitmap.width));
			lastoff = neo;
		}

		c[i].ax = (float)(g->advance.x >> 6);
		c[i].ay = (float)(g->advance.y >> 6);

		c[i].bw = (float)g->bitmap.width;
		c[i].bh = (float)g->bitmap.rows;

		c[i].bl = (float)g->bitmap_left;
		c[i].bt = (float)g->bitmap_top;

		c[i].tx = ox / (float)w;
		c[i].ty = oy / (float)h;

		rowh = std::max(rowh, (unsigned int)g->bitmap.rows);
		ox += g->bitmap.width + 1;
	}
	if (RunOnSecondDevice)
	{
		Texture = RHI::CreateTextureWithData(w, h, 1, NULL, RHI::GetDeviceContext(1));
	}
	else
	{
		Texture = RHI::CreateTextureWithData(w, h, 1, NULL, RHI::GetDeviceContext(0));
	}
	Texture->CreateTextureFromData(FinalData, RHI::TextureType::Text, w, h, 1);

	//printf("Generated a %d x %d (%d kb) texture atlas\n", w, h, w * h / 1024);
}

TextRenderer::atlas::~atlas()
{
	delete Texture;
}