
#include "BTBlackboard.h"


BTValue * BTBlackboard::AddValue(EBTBBValueType::Type type)
{
	BTValue* newvalue = new BTValue();
	newvalue->ValueType = type;//todo: name and initial value
	return newvalue;
}

BTBlackboard::BTBlackboard()
{}


BTBlackboard::~BTBlackboard()
{}

bool BTValue::IsValid()
{
	switch (ValueType)
	{
	case EBTBBValueType::Vector:
		break;
	case EBTBBValueType::Float:
		break;
	case EBTBBValueType::Integer:
		break;
	case EBTBBValueType::Object:
		return ObjectPtr.IsValid();
		break;
	}
	return true;
}
bool BTValue::CheckZero()
{
	switch (ValueType)
	{
	case EBTBBValueType::Vector:
		return Vector != glm::vec3(0.0f);
		break;
	case EBTBBValueType::Float:
		return FloatValue != 0.0f;
		break;
	case EBTBBValueType::Integer:
		return IntValue != 0;
		break;
	}
	return true;
}
bool BTValue::CheckGreater(float value, bool equal)
{
	if (equal)
	{
		switch (ValueType)
		{
		case EBTBBValueType::Vector:
			return Vector.length() >= value;
			break;
		case EBTBBValueType::Float:
			return FloatValue >= value;
			break;
		case EBTBBValueType::Integer:
			return IntValue >= value;
			break;
		}
	}
	else
	{
		switch (ValueType)
		{
		case EBTBBValueType::Vector:
			return Vector.length() > value;
			break;
		case EBTBBValueType::Float:
			return FloatValue > value;
			break;
		case EBTBBValueType::Integer:
			return IntValue > value;
			break;
		}
	}
	return true;
}
bool BTValue::CheckLess(float value, bool equal)
{
	if (equal)
	{
		switch (ValueType)
		{
		case EBTBBValueType::Vector:
			return Vector.length() <= value;
			break;
		case EBTBBValueType::Float:
			return FloatValue <= value;
			break;
		case EBTBBValueType::Integer:
			return IntValue <= value;
			break;
		}
	}
	else
	{
		switch (ValueType)
		{
		case EBTBBValueType::Vector:
			return Vector.length() < value;
			break;
		case EBTBBValueType::Float:
			return FloatValue < value;
			break;
		case EBTBBValueType::Integer:
			return IntValue < value;
			break;
		}
	}
	return true;
}