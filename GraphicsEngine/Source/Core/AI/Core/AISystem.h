#pragma once
#include "Navigation/NavigationManager.h"
class NavigationManager;
class AIDirector;
class Scene;
class BehaviourTreeManager;
class NavMeshGenerator;
namespace EAINavigationMode
{
	enum Type
	{
		AStar,
		DStarLTE,
		DStarBoardPhase,
		Limit
	};
}
namespace EAIDebugMode
{
	enum Type
	{
		None,
		NavMesh,
		PathOnly,
		BT,
		All,
		Limit
	};
}
/*!  \addtogroup AI
* @{ */
struct NavigationPath;
class AISystem
{
public:
	AISystem();
	~AISystem();
	void SetupForScene(Scene * newscene);
	void SceneStart();
	void SceneEnd();
	static void StartUp();
	static void ShutDown();
	void Tick(float dt);
	void EditorTick();
	CORE_API static AISystem* Get();
	NavigationManager* mesh = nullptr;
	///\returns the current pathfinding mode
	static EAINavigationMode::Type GetPathMode();
	///Adds a path request to the system - the path will not be ready until at best a frame later
	ENavRequestStatus::Type CalculatePath(glm::vec3 Startpoint, glm::vec3 EndPos, NavigationPath * outpath);
	template<class T>
	static T* GetDirector()
	{
		return (T*)Get()->Director;
	}
	BehaviourTreeManager* GetBTManager()
	{
		return BTManager;
	};
	void GenerateMesh();
	static EAIDebugMode::Type GetDebugMode();
	static void SetDebugMode(EAIDebugMode::Type mode);
private:
	NavMeshGenerator* n = nullptr;
	BehaviourTreeManager* BTManager = nullptr;
	AIDirector* Director = nullptr;
	static AISystem* Instance;
	EAINavigationMode::Type CurrentMode = EAINavigationMode::Limit;
	EAIDebugMode::Type DebugMode = EAIDebugMode::Limit;
};

