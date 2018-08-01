#include "Scene.h"
#include "RHI/RHI.h"
#include "Core/Components/MeshRendererComponent.h"
#include "Core/Components/CameraComponent.h"
#include "Core/Components/LightComponent.h"
#include "Rendering/Renderers/RenderEngine.h"
#include "Core/Components/CompoenentRegistry.h"
#include "Core/Components/RigidbodyComponent.h"
#include "Core/Components/Utillity/FreeLookComponent.h"
#include <algorithm>
#include "Core/Utils/MemoryUtils.h"
#include "Core/Assets/AssetManager.h"
Scene::Scene()
{
	LightingData.SkyBox = AssetManager::DirectLoadTextureAsset("\\texture\\cube_1024_preblurred_angle3_ArstaBridge.dds", true);
}

Scene::~Scene()
{
	Lights.clear();
	MemoryUtils::DeleteVector(SceneObjects);
}

void Scene::UpdateScene(float deltatime)
{
	if (SceneObjects.size() == 0)
	{
		return;
	}
	for (int i = 0; i < SceneObjects.size(); i++)
	{
		SceneObjects[i]->Update(deltatime);
	}
}
void Scene::EditorUpdateScene()
{
	if (SceneObjects.size() == 0)
	{
		return;
	}
	for (int i = 0; i < SceneObjects.size(); i++)
	{
		SceneObjects[i]->EditorUpdate();
	}
}

void Scene::OnFrameEnd()
{
	StaticSceneNeedsUpdate = false;//clear last frames flag
}

void Scene::FixedUpdateScene(float deltatime)
{
	for (int i = 0; i < SceneObjects.size(); i++)
	{
		SceneObjects[i]->FixedUpdate(deltatime);
	}
}

void Scene::StartScene()
{
	for (int i = 0; i < SceneObjects.size(); i++)
	{
		SceneObjects[i]->BeginPlay();
	}
}
void Scene::LoadDefault()
{
	GameObject* go = new GameObject("Main Camera");
	go->GetTransform()->SetPos(glm::vec3(0, 10, 0));
	go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
	go->GetTransform()->SetScale(glm::vec3(2));
	go->AttachComponent(new CameraComponent());
	AddGameobjectToScene(go);
}
//load an example scene
void Scene::LoadExampleScene(RenderEngine* Renderer, bool IsDeferredMode)
{
	GameObject* go = new GameObject("House");
	LightComponent* lc = nullptr;
	Material* newmat = new Material(AssetManager::DirectLoadTextureAsset("texture\\house_diffuse.tga"));
	go->AttachComponent(new MeshRendererComponent(RHI::CreateMesh("house.obj"), newmat));
	go->GetTransform()->SetPos(glm::vec3(7, 0, 0));
	go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
	AddGameobjectToScene(go);

	go = new GameObject("Terrain");
	Material* mat = new Material(AssetManager::DirectLoadTextureAsset("texture\\grasshillalbedo.png"));
	go->AttachComponent(new MeshRendererComponent(RHI::CreateMesh("DefaultTerrain.obj"), mat));
	go->GetTransform()->SetPos(glm::vec3(0, 0, 0));
	go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
	go->GetTransform()->SetScale(glm::vec3(2));
	//slow
	AddGameobjectToScene(go);

	go = new GameObject("Camera");
	go->GetTransform()->SetPos(glm::vec3(0, 10, 0));
	go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
	go->GetTransform()->SetScale(glm::vec3(2));
	go->AttachComponent(new CameraComponent());
#if !WITH_EDITOR
	go->AttachComponent(new FreeLookComponent());
#endif
	AddGameobjectToScene(go);

#if 0
	go = new GameObject("Dir Light");
	go->GetTransform()->SetPos(glm::vec3(0, 5, 1));
	go->GetTransform()->SetEulerRot(glm::vec3(45, 0, 0));
	lc = (LightComponent*)go->AttachComponent(new LightComponent());
	lc->SetShadow(true);
	lc->SetLightType(Light::Directional);
	lc->SetIntensity(1.3f);
	AddGameobjectToScene(go);

	//go = new GameObject("Dir Light");
	//go->GetTransform()->SetPos(glm::vec3(0, 5, 1));
	//go->GetTransform()->SetEulerRot(glm::vec3(50, 0, 0));
	//lc = (LightComponent*)go->AttachComponent(new LightComponent());
	//lc->SetShadow(true);
	//lc->SetLightType(Light::Directional);
	//lc->SetIntensity(1.3f);
	//AddGameobjectToScene(go);
#endif

#if 1
	go = new GameObject("Point Light");
	go->GetTransform()->SetPos(glm::vec3(38, 10, 20));
	go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
	lc = (LightComponent*)go->AttachComponent(new LightComponent());
	lc->SetShadow(true);
	lc->SetLightType(Light::Point);
	lc->SetIntensity(500.0f);
	AddGameobjectToScene(go);

	go = new GameObject("Point Light");
	go->GetTransform()->SetPos(glm::vec3(10, 20, 20));
	go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
	lc = (LightComponent*)go->AttachComponent(new LightComponent());
	lc->SetShadow(true);
	lc->SetLightType(Light::Point);
	lc->SetIntensity(500.0f);
	AddGameobjectToScene(go);



	go = new GameObject("Point Light");
	go->GetTransform()->SetPos(glm::vec3(3, 15, 20));
	go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
	lc = (LightComponent*)go->AttachComponent(new LightComponent());
	lc->SetShadow(true);
	lc->SetLightType(Light::Point);
	lc->SetIntensity(500.0f);
	AddGameobjectToScene(go);
	for (int i = 0; i < 1; i++)
	{
		go = new GameObject("Point Light");
		go->GetTransform()->SetPos(glm::vec3(20 + (10 * i), 5, 20));
		go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
		lc = (LightComponent*)go->AttachComponent(new LightComponent());
		lc->SetShadow(true);
		lc->SetLightType(Light::Point);
		lc->SetIntensity(500.0f);
		AddGameobjectToScene(go);
	}
#endif

	Material::MaterialProperties props = Material::MaterialProperties();
	go = new GameObject("Test");
	mat = new Material(AssetManager::DirectLoadTextureAsset("\\texture\\bricks2.jpg"), props);
	go->AttachComponent(new MeshRendererComponent(RHI::CreateMesh("Sphere.obj"), mat));
	go->GetTransform()->SetPos(glm::vec3(10, 10, 0));
	go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
	go->GetTransform()->SetScale(glm::vec3(1));
	AddGameobjectToScene(go);


	go = new GameObject("Plane");
	mat = new Material(AssetManager::DirectLoadTextureAsset("\\texture\\bricks2.jpg"));
	go->AttachComponent(new MeshRendererComponent(RHI::CreateMesh("Plane.obj"), mat));
	go->GetTransform()->SetPos(glm::vec3(0, 20, 20));
	go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
	go->GetTransform()->SetScale(glm::vec3(0.5));
	go->AttachComponent(new RigidbodyComponent());

	AddGameobjectToScene(go);

	go = new GameObject("Plane");
	mat = new Material(AssetManager::DirectLoadTextureAsset("\\texture\\bricks2.jpg"));
	mat->SetNormalMap(AssetManager::DirectLoadTextureAsset("\\texture\\bricks2_normal.jpg", true));
	mat->SetDisplacementMap(AssetManager::DirectLoadTextureAsset("\\texture\\bricks2_disp.jpg"));

	go->AttachComponent(new MeshRendererComponent(RHI::CreateMesh("Plane.obj"), mat));
	go->GetTransform()->SetPos(glm::vec3(-24, 2, -6));
	go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
	go->GetTransform()->SetScale(glm::vec3(0.5));

	AddGameobjectToScene(go);

	/*go = new GameObject("Fence");
	mat = new Material(AssetManager::DirectLoadTextureAsset("\\texture\\fence.png"));
	mat->SetShadow(false);
	go->AttachComponent(new MeshRendererComponent(RHI::CreateMesh("Plane.obj"), mat));
	go->GetTransform()->SetPos(glm::vec3(-10, 1, -6));
	go->GetTransform()->SetEulerRot(glm::vec3(90, -90, 0));
	go->GetTransform()->SetScale(glm::vec3(0.1f));

	AddGameobjectToScene(go);*/

	go = new GameObject("Static Water");
	mat = new Material(AssetManager::DirectLoadTextureAsset("texture\\Water fallback.jpg"));
	mat->SetNormalMap(AssetManager::DirectLoadTextureAsset("texture\\IKT4l.jpg"));
	go->AttachComponent(new MeshRendererComponent(RHI::CreateMesh("Plane.obj"), mat));
	go->GetTransform()->SetPos(glm::vec3(-37, -2, -20));
	go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
	go->GetTransform()->SetScale(glm::vec3(2));

	AddGameobjectToScene(go);
	if (IsDeferredMode == false)
	{
		/*go = new GameObject("Water");

		Material::MaterialProperties props;
		props.DoesShadow = false;
		props.IsReflective = true;
		mat = new Material(Renderer->GetReflectionBuffer()->GetRenderTexture(), props);
		mat->NormalMap = AssetManager::DirectLoadTextureAsset("\\texture\\IKT4l.jpg");
		go->AttachComponent(new MeshRendererComponent(RHI::CreateMesh("Plane.obj", Renderer->GetMainShader()->GetShaderProgram()), mat));
		go->GetTransform()->SetPos(glm::vec3(-37, -1, -21));
		go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
		go->GetTransform()->SetScale(glm::vec3(2));
		Camera* c = new Camera(go->GetTransform()->GetPos(), 90.0f, static_cast<float>(512 / 512), 0.1f, 100.0f);
		c->Pitch(-90);
		c->SetUpAndForward(glm::vec3(0, 1.0, 0), glm::vec3(0, 0, 1.0));
		Renderer->SetReflectionCamera(c);
		AddGameobjectToScene(go);*/
	}
	StaticSceneNeedsUpdate = true;

	int size = 5;
	glm::vec3 startPos = glm::vec3(10, 5, 30);
	float stride = 5.0f;
	props;
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			go = new GameObject("Water");
			props.Roughness = x * (1.0f / size);
			props.Metallic = y * (1.0f / size);

			mat = new Material(AssetManager::DirectLoadTextureAsset("\\texture\\bricks2.jpg"), props);
			go->AttachComponent(new MeshRendererComponent(RHI::CreateMesh("Sphere.obj"), mat));
			go->GetTransform()->SetPos(startPos + glm::vec3(x*stride, y*stride, 0));
			go->GetTransform()->SetEulerRot(glm::vec3(0, 0, 0));
			go->GetTransform()->SetScale(glm::vec3(1));
			AddGameobjectToScene(go);
		}
	}
}

void Scene::RemoveCamera(Camera * Cam)
{
	if (Cameras.size() > 1)
	{
		Cameras.erase(std::remove(Cameras.begin(), Cameras.end(), Cam));
	}
	else
	{
		Cameras.empty();
	}
	if (CurrentCamera == Cam)
	{
		if (Cameras.size())
		{
			CurrentCamera = Cameras[0];
		}
		else
		{
			CurrentCamera = nullptr;
		}
	}
}

void Scene::RemoveLight(Light * Light)
{
	if (Lights.size() > 1)
	{
		//todo error!
		Lights.erase(std::remove(Lights.begin(), Lights.end(), Light));
	}
	else
	{
		Lights.empty();
	}
}
void Scene::RemoveGameObject(GameObject* object)
{
	SceneObjects.erase(std::remove(SceneObjects.begin(), SceneObjects.end(), object));
	object->Internal_SetScene(nullptr);
}
void Scene::AddGameobjectToScene(GameObject* gameobject)
{
	SceneObjects.push_back(gameobject);
	gameobject->Internal_SetScene(this);
}

void Scene::CopyScene(Scene* newscene)
{
	for (int i = 0; i < SceneObjects.size(); i++)
	{
		GameObject* go = new GameObject(*SceneObjects[i]);
		newscene->AddGameobjectToScene(go);
	}

}