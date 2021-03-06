#pragma once
#include "Core/Utils/VectorUtils.h"
template<class T>
class WeakObjectPtr
{
public:
	bool IsValid() const
	{
		return RawPtr != nullptr;
	}
	T* Get() const
	{
		return RawPtr;
	}
	void Reset()
	{
		RawPtr = nullptr;
	}
	WeakObjectPtr()
	{}
	WeakObjectPtr(T* ptr)
	{
		Init(ptr);
	}
	~WeakObjectPtr()
	{
		if (RawPtr)
		{
			RawPtr->RemoveRef(this);
		}
		Reset();
	}
	void operator=(T* t)
	{
		Init(t);
	}
	bool operator==(T* other)
	{
		//if (!IsValid() && other == nullptr)
		//{
		//	return false;
		//}
		return other == RawPtr;
	}
	bool operator!=(T* other)
	{
		return !operator==(other);
	}
	T* operator->() const
	{
		return Get();
	}
	T operator*() const
	{
		return *Get();
	}
private:
	void Init(T* ptr)
	{
		if (RawPtr == ptr)
		{
			return;
		}
		if (RawPtr != nullptr)
		{
			//we already have one assigned so
			RawPtr->RemoveRef(this);//clear the ref to the other object
		}
		RawPtr = ptr;
		if (RawPtr != nullptr)
		{
			RawPtr->RegisterWeak(this);
		}
	}
	T* RawPtr = nullptr;
};

template<class T>
class IGarbageCollectable
{
public:
	IGarbageCollectable()
	{}
	void RegisterWeak(WeakObjectPtr<T>* newref)
	{
		Refs.push_back(newref);
	}
	virtual ~IGarbageCollectable()
	{
		Destory();
	}
	void Destory()
	{
		for (int i = 0; i < Refs.size(); i++)
		{
			Refs[i]->Reset();
		}
		Refs.clear();
	}
	void RemoveRef(WeakObjectPtr<T>* newref)
	{
		VectorUtils::Remove(Refs, newref);
	}
private:
	std::vector<WeakObjectPtr<T>*> Refs;
};