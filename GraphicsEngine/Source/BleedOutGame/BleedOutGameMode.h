#pragma once
#include "EngineHeader.h"

class BleedOutHud;
struct DifficultyPreset
{
	int WaveEnemyCount = 1;
	//when closer to 1 more high level AI will be spawned
	float SizeWeighting = 0.5f;//normal value;
	int MaxAttackingAI = 3;
	float BeedOutSpeed = 1.5f;
	float AITransferPC = 0.2f;
};

struct Door
{
	void Init(GameObject* Obj);
	WeakObjectPtr<GameObject> Door;
	glm::vec3 UpPos = glm::vec3(0, 0, 0);
	glm::vec3 DownPos = glm::vec3(0, 4, 0);
	void Down();
	void Up();
};

class BleedOutGameMode :public GameMode
{
public:
	BleedOutGameMode();
	~BleedOutGameMode();
	virtual void BeginPlay(Scene* Scene);
	void SpawnSKull(glm::vec3 Position);
	virtual void EndPlay();
	virtual void Update();
	void OnPlayerDeath();
	GameObject* GetPlayer()
	{
		return MPlayer.Get();
	};
	void SpawnPlayer(glm::vec3 Pos, Scene* scene);
	void CollectDoors();
	void SetRoomLocked();
	void UnlockNextRoom();
	const DifficultyPreset& GetDifficultyPreset();
	void CompleteGame();
	bool IsGameComplete = false;
private:
	WeakObjectPtr<GameObject> MPlayer;
	std::vector<Door> Doors;
	DifficultyPreset CurrentDifficluty;
	BleedOutHud* GameHud = nullptr;
};

