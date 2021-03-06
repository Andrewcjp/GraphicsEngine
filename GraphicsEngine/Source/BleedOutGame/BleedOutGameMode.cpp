#include "BleedOutGameMode.h"
#include "AI/Core/AIController.h"
#include "AI/Core/AISystem.h"
#include "AI/SkullChaser.h"
#include "AI/BleedOut_Director.h"
#include "Components/Health.h"
#include "Components/MeleeWeapon.h"
#include "Components/Pickup.h"
#include "Components/Projectile.h"
#include "Components/Railgun.h"
#include "Components/SpawningPool.h"
#include "Components/BleedOutPlayer.h"
#include "Components/Weapon.h"
#include "Components/WeaponManager.h"
#include "Core/Components/Core_Components_inc.h"
#include "Physics/GenericConstraint.h"
#include "Audio/AudioEngine.h"
#include "BleedOutHud.h"
#include "Benching/LevelBenchMarker.h"

BleedOutGameMode::BleedOutGameMode()
{}

BleedOutGameMode::~BleedOutGameMode()
{}

GameObject* MakeTestSphere(Scene* Scene, bool Static = false)
{
	GameObject* go = Scene::CreateDebugSphere(nullptr);
	if (!Static)
	{
		go->AttachComponent(new RigidbodyComponent());
	}
	else
	{
		go->SetPosition(glm::vec3(10, 20, 0));
	}
	ColliderComponent* cc = go->AttachComponent(new ColliderComponent());
	cc->SetCollisonShape(EShapeType::eSPHERE);
	Scene->AddGameobjectToScene(go);
	return go;
}
void BleedOutGameMode::SpawnSpectator(glm::vec3 pos)
{
	GameObject* Cam = new GameObject("PlayerCamera");
	AudioEngine::Get()->MakeDefaultListener(Cam);
	CameraComponent* c = Cam->AttachComponent(new CameraComponent());
	c->AllowRotSync = true;
	MPlayer = Cam;
	CurrentScene->AddGameobjectToScene(Cam);
}

void BleedOutGameMode::BeginPlay(Scene* Scene)
{
	GameMode::BeginPlay(Scene);
#if !RUN_ONLY_BENCHER
	AISystem::Get()->GetDirector<BleedOut_Director>()->GameMode = this;
	GameHud = (BleedOutHud*)Hud;
#if 1
	GameObject* A = MakeTestSphere(Scene, true);
	GameObject* B = MakeTestSphere(Scene);
	B->SetPosition(glm::vec3(10, 10, 0));
	ConstaintSetup data;
	//data.Desc.Dampening = 0.5f;
	//Engine::GetPhysEngineInstance()->CreateConstraint(A->GetComponent<ColliderComponent>()->GetActor(), B->GetComponent<RigidbodyComponent>()->GetActor(), data);
#endif

	SpawnPlayer(glm::vec3(0, 5, 35), Scene);

	GameObject* AiTest = new GameObject();
	Material* mat = Material::CreateDefaultMaterialInstance();
	mat->SetDiffusetexture(AssetManager::DirectLoadTextureAsset("\\texture\\Spawner.png"));
	MeshLoader::FMeshLoadingSettings set;
	set.FlipUVs = true;
	AiTest->AttachComponent(new MeshRendererComponent(RHI::CreateMesh("Models\\SpawningPool.obj", set), mat));
	AiTest->AttachComponent(new SpawningPool());
	AiTest->SetPosition(glm::vec3(0, 0, 10));
	Scene->AddGameobjectToScene(AiTest);
	Pickup::SpawnPickup(glm::vec3(0, 1, -20), PickupType::Rifle_Ammo, 100);
	Pickup::SpawnPickup(glm::vec3(0, 1, -22), PickupType::Gauss_Ammo, 100);
	CollectDoors();
	CurrentDifficluty.Init(CurrnetLevel);
#else
	BenchMarker = new LevelBenchMarker();
	SpawnSpectator(glm::vec3(0, 0, 0));
	BenchMarker->CameraObject = MPlayer.Get();
	BenchMarker->Init();
	TestObject = Scene->SpawnBox(glm::vec3(1, 1, 1));
#endif

}

void BleedOutGameMode::SpawnSKull(glm::vec3 Position)
{
	GameObject* skull = GameObject::Instantiate(Position);
	SkullChaser* c = skull->AttachComponent(new SkullChaser());
	c->Player = MPlayer;
	Material* mat = Material::CreateDefaultMaterialInstance();
	mat->SetDiffusetexture(AssetManager::DirectLoadTextureAsset("\\texture\\bricks2.jpg"));
	skull->AttachComponent(new MeshRendererComponent(RHI::CreateMesh("models\\Munkey.obj"), mat));
	skull->AttachComponent(new ColliderComponent());
	RigidbodyComponent* rb = skull->AttachComponent(new RigidbodyComponent());
	rb->SetGravity(false);
	Health* h = skull->AttachComponent(new Health());
	h->MaxHealth = 10.0f;
	Projectile* p = skull->AttachComponent(new Projectile());
	p->SetDamage(50);
	CurrentScene->AddGameobjectToScene(skull);
}

void BleedOutGameMode::EndPlay()
{
	GameMode::EndPlay();
}

void BleedOutGameMode::Update()
{
	GameMode::Update();
	if (BenchMarker != nullptr)
	{
		BenchMarker->Update();
	}
#if 0
	if (GetPlayer() != nullptr)
	{
		float distance = glm::length(GetPlayer()->GetPosition() - glm::vec3(0, 1, -45));
		if (distance < 5)
		{
			CompleteGame();
			GetPlayer()->Destory();
		}
	}
	else
	{
		Input::SetCursorState(false, true);
	}
#endif
	if (TestObject != nullptr)
	{
		const float Flip = 20;
		float spd = 10.0f;
		if (FWD)
		{
			CurrentPos += spd*Engine::GetDeltaTime();
		}
		else
		{
			CurrentPos -= spd*Engine::GetDeltaTime();
		}
		TestObject->SetPosition(glm::vec3(CurrentPos, 10, 0));
		if (CurrentPos > Flip)
		{
			FWD = false;
		}
		else if(CurrentPos < -Flip)
		{
			FWD = true;
		}

	}

}

void BleedOutGameMode::OnPlayerDeath()
{
	GetPlayer()->Destory();
	GameHud->ShowRestart();
	GameHud->DisplayText("You Died", 10.0f, 0.15f);
}

void BleedOutGameMode::SpawnPlayer(glm::vec3 Pos, Scene* Scene)
{
	GameObject* go = new GameObject("Player Test");
	MPlayer = go;
	MPlayer->Tags.Add("player");
#if 0
	Material* mat = Material::GetDefaultMaterial();
	mat->SetDiffusetexture(AssetManager::DirectLoadTextureAsset("\\texture\\bricks2.jpg"));
	mat->GetProperties()->Roughness = 0.0f;
	mat->GetProperties()->Metallic = 1.0f;
	go->AttachComponent(new MeshRendererComponent(RHI::CreateMesh("models\\Sphere.obj"), mat));
#endif
	go->SetPosition(Pos);
	go->GetTransform()->SetEulerRot(glm::vec3(0, -90, 0));
	go->GetTransform()->SetScale(glm::vec3(1));
	go->AttachComponent(new RigidbodyComponent());
	ColliderComponent* cc = go->AttachComponent(new ColliderComponent());
	cc->SetCollisonShape(EShapeType::eCAPSULE);
	cc->LocalOffset = glm::vec3(0, 0, 0);
	Health* H = go->AttachComponent(new Health());
	H->BindDeathCallback(std::bind(&BleedOutGameMode::OnPlayerDeath, this));
	BleedOutPlayer* player = go->AttachComponent(new BleedOutPlayer());
	player->BleedOutRate = GetDifficultyPreset()->BeedOutSpeed;
	BodyInstanceData lock;
	lock.LockXRot = true;
	lock.LockZRot = true;
	lock.Mat = new PhysicalMaterial();
	lock.Mat->Bouncyness = 0.0f;
	lock.Mat->StaticFriction = 0.0f;
	go->GetComponent<RigidbodyComponent>()->SetBodyData(lock);

	GameObject* Cam = new GameObject("PlayerCamera");
	AudioEngine::Get()->MakeDefaultListener(Cam);
	Cam->SetParent(go);
	Cam->DisableCulling = true;
	player->CameraObject = Cam;
	WeaponManager* manager = go->AttachComponent(new WeaponManager());
	manager->Weapons[Weapon::WeaponType::Rifle] = go->AttachComponent(new Weapon(Weapon::WeaponType::Rifle, Scene, player));
	manager->Weapons[Weapon::WeaponType::ShotGun] = go->AttachComponent(new Weapon(Weapon::WeaponType::ShotGun, Scene, player));
	manager->Weapons[Weapon::WeaponType::RailGun] = go->AttachComponent(new Railgun(Scene, player));
	manager->Melee = go->AttachComponent(new MeleeWeapon());
	cc = go->AttachComponent(new ColliderComponent());
	cc->SetCollisonShape(EShapeType::eSPHERE);
	cc->Radius = 2.0f;
	cc->IsTrigger = true;
	manager->Melee->Collider = cc;
	Scene->AddGameobjectToScene(Cam);
	Scene->AddGameobjectToScene(go);
	AISystem::Get()->GetDirector<BleedOut_Director>()->SetPlayer(player->GetOwner());
}

void BleedOutGameMode::CollectDoors()
{
	Door New = Door();
	New.Init(CurrentScene->FindByName("EntryDoor"));
	New.Down();
	Doors.push_back(New);
	New = Door();
	New.Init(CurrentScene->FindByName("ExitDoor"));
	Doors.push_back(New);
}

void BleedOutGameMode::SetRoomLocked()
{
	for (int i = 0; i < Doors.size(); i++)
	{
		Doors[i].Up();
	}
}

void BleedOutGameMode::UnlockNextRoom()
{
	Doors[0].Down();
	Doors[1].Down();//todo: next room 
}

const DifficultyPreset* BleedOutGameMode::GetDifficultyPreset()
{
	return &CurrentDifficluty;
}

void BleedOutGameMode::CompleteGame()
{
	IsGameComplete = true;
	GameHud->DisplayText("Level Complete", 10.0f, 0.15f);
	GameHud->ShowRestart();
}

void Door::Init(GameObject * Obj)
{
	Door = Obj;
	UpPos = Obj->GetPosition();
	DownPos = UpPos - glm::vec3(0, 10, 0);
}

void Door::Down()
{
	Door->SetPosition(DownPos);
}

void Door::Up()
{
	Door->SetPosition(UpPos);
}

const DifficultyPreset* DifficultyPreset::Get()
{
	if (AISystem::Get() != nullptr)
	{
		return AISystem::Get()->GetDirector<BleedOut_Director>()->GetDifficultyPreset();
	}
	return nullptr;
}

void DifficultyPreset::Init(EGameDifficulty::Type Level)
{
	if (Level == EGameDifficulty::Easy)
	{
		WaveEnemyCount = 1;
		MaxAttackingAI = 2;
		BeedOutSpeed = 1.5f;
		AITransferPC = 0.4f;
		MeleeDamageMulti = 0.5f;
		RifleDamageMulti = 0.5f;
	}
	else if (Level == EGameDifficulty::Normal)
	{
		WaveEnemyCount = 1;
		MaxAttackingAI = 3;
		BeedOutSpeed = 1.5f;
		AITransferPC = 0.2f;
		MeleeDamageMulti = 0.7f;
		RifleDamageMulti = 1.0f;
	}
	else
	{
		WaveEnemyCount = 2;
		MaxAttackingAI = 4;
		BeedOutSpeed = 2.0f;
		AITransferPC = 0.2f;
		MeleeDamageMulti = 1.0f;
		RifleDamageMulti = 2.0f;
	}
}
