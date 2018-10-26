#pragma once
namespace TD
{
	class TDScene;
	class TDSolver;
	class TDSimConfig;
	class TDPhysics
	{
	public:
		/**
		*\brief Creates a Physics Engine Object for use
		*\param BuildID provided by TD_VERSION_NUMBER in TDPhysicsAPI.h
		*\return a valid TDPhysics* instance unless the BuildID is invalid In which case nullptr is returned;
		*/
		TD_API static TDPhysics * CreatePhysics(unsigned int BuildID, TDSimConfig* SimConfig = nullptr);
		/**
		*\brief Initializes the Simulator ready for work
		*/
		TD_API void StartUp();
		/**
		*\brief Starts the step of the Simulator 
		*\param TimeStep for the Simulator to use
		*/
		TD_API void StartStep(float TimeStep);
		/**
		*\brief Destroy the Simulator
		*After this call any TDPhysics pointers are invalid for use.
		*/
		TD_API void ShutDown();
		/**
		*\brief Returns the current TDPhysics instance, nullptr if the Simulator has not been initialized 
		*/
		TD_API static TDPhysics* Get();
		/**
		*\brief Creates a new TDScene for Use.
		*/
		TD_API TDScene* CreateScene();

		///Returns the current Config for the simulator of which some properties can be changed at runtime
		static TDSimConfig* GetCurrentSimConfig();
	private:
		TDPhysics();
		~TDPhysics();

		TDSolver* Solver = nullptr;
		std::vector<TDScene*> Scenes;
		static TDPhysics* Instance;
		TDSimConfig* CurrentSimConfig = nullptr;
		
	};

}