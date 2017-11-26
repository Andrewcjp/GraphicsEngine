#include "Shader_Main.h"
#include "RHI/RHI.h"
#include "../Rendering/Core/GPUStateCache.h"

Shader_Main::Shader_Main()
{
	//Initialise OGL shader
	m_Shader = RHI::CreateShaderProgam();

	m_Shader->CreateShaderProgram();
	//m_Shader->AttachAndCompileShaderFromFile(L"../asset/shader/glsl/PBR.vert", SHADER_VERTEX);
	//m_Shader->AttachAndCompileShaderFromFile(L"../asset/shader/glsl/PBR.frag", SHADER_FRAGMENT);
	m_Shader->AttachAndCompileShaderFromFile("Main_vs", SHADER_VERTEX);
	m_Shader->AttachAndCompileShaderFromFile("Main_fs", SHADER_FRAGMENT);

	m_Shader->BindAttributeLocation(0, "pos");
	m_Shader->BindAttributeLocation(1, "Normal");
	m_Shader->BindAttributeLocation(2, "texCoords");

	m_Shader->BuildShaderProgram();
	m_Shader->ActivateShaderProgram();
	if (RHI::GetType() == RenderSystemOGL)
	{
		m_uniform_model = glGetUniformLocation(m_Shader->GetProgramHandle(), "model");
		m_uniform_View = glGetUniformLocation(m_Shader->GetProgramHandle(), "view");
		m_UniformMVP = glGetUniformLocation(m_Shader->GetProgramHandle(), "projection");

		Uniform_Cam_Pos = glGetUniformLocation(m_Shader->GetProgramHandle(), "viewPos");
		m_uniform_texture = glGetUniformLocation(m_Shader->GetProgramHandle(), "albedoMap");
		m_uniform_LightNumber = glGetUniformLocation(m_Shader->GetProgramHandle(), "numLights");
		glUniform1i(m_uniform_texture, ALBEDOMAP);
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "NormalMap"), NORMALMAP);
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "shadowcubemap2"), SHADOWCUBEMAP2);
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "shadowcubemap"), SHADOWCUBEMAP);
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "shadowdirmap"), SHADOWDIRMAP1);
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "Displacementmap"), DISPMAP);
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "Reflectmap"), 10);
		m_IsMapUniform = glGetUniformLocation(m_Shader->GetProgramHandle(), "isMap");
		m_FarPlane = glGetUniformLocation(m_Shader->GetProgramHandle(), "far_plane");
		m_MV33 = glGetUniformLocation(m_Shader->GetProgramHandle(), "MV3x3");
		IsReflect = glGetUniformLocation(m_Shader->GetProgramHandle(), "IsReft");

		unsigned int block_index = glGetUniformBlockIndex(m_Shader->GetProgramHandle(), "UBuffer");
		GLuint binding_point_index = 1;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, ubo);
		//glNamedBufferData(ubo, sizeof(ConstBuffer), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		//block_index = glGetUniformBlockIndex(m_Shader->GetProgramHandle(), "UBuffer");
		//GLuint binding_point_index = 1;
		//glGenBuffers(1, &ubo);
		//glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		//glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, ubo);
		////glNamedBufferData(ubo, sizeof(ConstBuffer), nullptr, GL_DYNAMIC_DRAW);
		//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	LightBuffers = new LightUniformBuffer[CurrentLightcount];
}
//NormalMap

Shader_Main::~Shader_Main()
{

}
void Shader_Main::SetNormalVis()
{
	//	std::cout << "hi "<<state << std::endl;
	if (vistate)
	{
		vistate = false;
	}
	else
	{
		vistate = true;
	}
}
void Shader_Main::SetFullBright()
{
	if (enabledFullBright)
	{
		enabledFullBright = false;
	}
	else
	{
		enabledFullBright = true;
	}
}
void Shader_Main::SetShadowVis()
{
	if (shadowvisstate)
	{
		shadowvisstate = false;
	}
	else
	{
		shadowvisstate = true;
	}
}
void Shader_Main::SetNormalState(bool t, bool dispstate, bool refelction)
{
	if (RHI::GetType() == RenderSystemOGL)
	{
		if (t)
		{
			glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "HasNormal"), 1);
		}
		else
		{
			glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "HasNormal"), 0);
		}
		//default value IS ALWAYS zero!HasDisp
		if (dispstate)
		{
			glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "HasDisp"), 1);
		}
		else
		{
			glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "HasDisp"), 0);
		}
		if (refelction)
		{
			glUniform1i(IsReflect, 1);
		}
		else
		{
			glUniform1i(IsReflect, 0);
		}
	}
}

void Shader_Main::UpdateOGLUniforms(Transform* t, Camera* c, std::vector<Light*> lights)
{


	if (shadowvisstate)
	{
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "VisShadow"), 1);
	}
	else
	{
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "VisShadow"), 0);
	}
	if (enabledFullBright)
	{
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "FULLBRIGHT"), 1);
	}
	else
	{
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "FULLBRIGHT"), 0);
	}

	if (ISWATER)
	{
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "IsWater"), 1);
		glUniform1f(glGetUniformLocation(m_Shader->GetProgramHandle(), "scroll"), currentnumber);
	}
	else
	{
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "IsWater"), 0);
	}
	if (IsPhysics)
	{
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "isMap"), 1);
	}
	else
	{
		glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), "isMap"), 0);
	}

	glUniform1i(m_uniform_LightNumber, static_cast<GLuint>(lights.size()));
	glUniform1f(m_FarPlane, static_cast<GLfloat>(ShadowFarPlane));
	glUniform1i(m_IsMapUniform, 0);
	if (!Once)
	{
		for (int i = 0; i < lights.size(); i++)
		{
			//todo: improve this!
			glUniform3f(glGetUniformLocation(m_Shader->GetProgramHandle(), ("allLights[" + std::to_string(i) + "].position").c_str()), lights[i]->GetPosition().x, lights[i]->GetPosition().y, lights[i]->GetPosition().z);
			glUniform3f(glGetUniformLocation(m_Shader->GetProgramHandle(), ("allLights[" + std::to_string(i) + "].Color").c_str()), lights[i]->GetColor().x, lights[i]->GetColor().y, lights[i]->GetColor().z);
			glUniform3fv(glGetUniformLocation(m_Shader->GetProgramHandle(), ("allLights[" + std::to_string(i) + "].Direction").c_str()), 1, glm::value_ptr(lights[i]->GetDirection()));
			glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), ("allLights[" + std::to_string(i) + "].type").c_str()), static_cast<int>(lights[i]->GetType()));
			glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), ("allLights[" + std::to_string(i) + "].ShadowID").c_str()), (lights[i]->GetShadowId()));
			glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), ("allLights[" + std::to_string(i) + "].DirShadowID").c_str()), (lights[i]->DirectionalShadowid));
			glUniform1i(glGetUniformLocation(m_Shader->GetProgramHandle(), ("allLights[" + std::to_string(i) + "].HasShadow").c_str()), (lights[i]->GetDoesShadow()));
		}
		Once = true;
	}
	UBuffer.M = t->GetModel();
	UBuffer.V = c->GetView();
	UBuffer.P = c->GetProjection();
	if (!GPUStateCache::CheckCurrentUniformBuffer(ubo))
	{
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		GPUStateCache::UpdateCurrentUniformBuffer(ubo);
	}
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MVPStruct), &UBuffer, GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
	//glNamedBufferStorage(ubo, sizeof(ConstBuffer), &UBuffer, GL_MAP_READ_BIT);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	//GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);
	//ConstBuffer Testptr;
	//memcpy(&Testptr, p, sizeof(ConstBuffer));
	////ConstBuffer test = reinterpret_cast<ConstBuffer>(Testptr);
	//glUnmapBuffer(GL_UNIFORM_BUFFER);

	/*GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << glewGetErrorString(error) << std::endl;
	}*/

}

void Shader_Main::UpdateD3D11Uniforms(Transform * t, Camera * c, std::vector<Light*> lights)
{
	//UBuffer
	UBuffer.M = t->GetModel();
	UBuffer.V = c->GetView();
	UBuffer.P = c->GetProjection();
	RHI::GetD3DContext()->UpdateSubresource(RHI::instance->m_constantBuffer, 0, NULL, &UBuffer, 0, 0);
	RHI::GetD3DContext()->VSSetConstantBuffers(0, 1, &RHI::instance->m_constantBuffer);
}
