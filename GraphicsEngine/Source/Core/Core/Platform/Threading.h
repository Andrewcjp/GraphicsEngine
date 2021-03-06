#pragma once
#ifdef PLATFORM_WINDOWS
#include "../MinWindows.h"
#endif
#include <thread>

namespace Threading
{
	class Event
	{
	private:
		void *Handle;
	public:
		Event();
		~Event();
		void Signal();

		// use -1 for an infinite wait
		bool WaitForSignal(int milliseconds);
	};
	class Thread
	{
	public:
		Thread(int index)
		{
#ifdef PLATFORM_WINDOWS
			DWORD id;
			Handle = CreateThread(NULL, 0, &Thread::ThreadMain, this, 0, &id);
			std::wstring Name = L"TaskGraph Worker " + std::to_wstring(index);
			SetThreadDescription(Handle, Name.c_str());
#endif
			ThreadIndex = index;
		}

		~Thread();

		void RequestToExit()
		{
			/// Signal Main() to finish
			QuitRequested = true;
		}

		bool IsRequestedToExit() const
		{
			return QuitRequested;
		}

		bool WaitForThreadToFinish(int milliseconds)
		{
			return JobDone.WaitForSignal(milliseconds);
		}

		Event JobReady; /// waiting for jobs
		Event JobDone;  /// wait for workers to complete their work
		void StartFunction(std::function <void(int)> functionToRun)
		{
			FunctionToRun = functionToRun;
			JobReady.Signal();
		}

		void WaitForFunctionCompletion()
		{
			JobDone.WaitForSignal(-1);
		}
		int ThreadIndex = 0;
	private:
		void *Handle;

		volatile bool QuitRequested = false;
		std::function <void(int)> FunctionToRun;
#ifdef PLATFORM_WINDOWS
		static DWORD WINAPI ThreadMain(void *threadAsVoidPtr);
#endif
	};

	class TaskGraph
	{
	public:
		TaskGraph(int ThreadCount);
		void Shutdown();
		/**
		*\param function to run with thread index as parameter
		*\param threadstouse if 0 all threads will be used
		*/
		void RunTaskOnGraph(std::function<void(int)> function, int threadstouse = 0);
		int GetThreadCount() const;
	private:
		Thread** Threads;
		int ThreadCount = 0;
	};
}
