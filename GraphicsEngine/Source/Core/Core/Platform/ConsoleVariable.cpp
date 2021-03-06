
#include "ConsoleVariable.h"
#include "Core/Utils/StringUtil.h"
#include "Logger.h"
#include "../Utils/VectorUtils.h"
#include "Core/Reflection/ClassReflectionNode.h"
ConsoleVariableManager* ConsoleVariableManager::Instance = nullptr;

ConsoleVariable::ConsoleVariable(std::string name, bool defaultValue, ECVarType::Type cvartype, bool NeedsValue) :ConsoleVariable(name, cvartype, NeedsValue)
{
	CurrentValue.Int_Value = defaultValue;
	DefaultValue.Int_Value = defaultValue;
	ReflectionNode = new ClassReflectionNode(name, MemberValueType::Bool, &CurrentValue.Int_Value);
}

ConsoleVariable::ConsoleVariable(std::string name, int defaultValue, ECVarType::Type cvartype, bool NeedsValue) :ConsoleVariable(name, cvartype, NeedsValue)
{
	CurrentValue.Int_Value = defaultValue;
	DefaultValue.Int_Value = defaultValue;
	ReflectionNode = new ClassReflectionNode(name, MemberValueType::Int, &CurrentValue.Int_Value);
}

ConsoleVariable::ConsoleVariable(std::string name, float defaultValue, ECVarType::Type cvartype, bool NeedsValue) : ConsoleVariable(name, cvartype, NeedsValue)
{
	CurrentValue.F_Value = defaultValue;
	DefaultValue.F_Value = defaultValue;
	IsFloat = true;
	ReflectionNode = new ClassReflectionNode(name, MemberValueType::Float, &CurrentValue.F_Value);
}

ConsoleVariable::ConsoleVariable(std::string name, ECVarType::Type cvartype, std::function<void()> func, std::function<void(bool state)> intfunc, std::function<void(float state)> floatfunc) :ConsoleVariable(name, cvartype, false)
{
	ExecuteFunction = func;
	OnChangedFunction = intfunc;
	OnChangedFloatFunction = floatfunc;
	if (OnChangedFloatFunction != nullptr)
	{
		IsFloat = true;
		ReflectionNode = new ClassReflectionNode(name, MemberValueType::Float, &CurrentValue.F_Value);
	}
	else
	{
		ReflectionNode = new ClassReflectionNode(name, MemberValueType::Int, &CurrentValue.Int_Value);
	}
	if (ExecuteFunction == nullptr)
	{
		NeedsValue = true;
	}
}

const std::string & ConsoleVariable::GetName() const
{
	return Name;
}

std::string ConsoleVariable::GetLaunchName() const
{
	return "-" + Name;
}

bool ConsoleVariable::GetBoolValue() const
{
	return CurrentValue.Int_Value;
}

void ConsoleVariable::SetValue(int value)
{
	for (ConsoleVariable* V : LinkedVars)
	{
		V->SetValue(value);
	}
	CurrentValue.Int_Value = value;
	if (OnChangedBoolFunction)
	{
		OnChangedBoolFunction(value);
	}
	if (OnChangedFunction)
	{
		OnChangedFunction(value);
	}
}

void ConsoleVariable::SetValueF(float value)
{
	for (ConsoleVariable* V : LinkedVars)
	{
		V->SetValueF(value);
	}
	CurrentValue.F_Value = value;
	if (OnChangedFloatFunction)
	{
		OnChangedFloatFunction(value);
	}
}

int ConsoleVariable::GetIntValue() const
{
	return CurrentValue.Int_Value;
}

bool ConsoleVariable::IsValueVar() const
{
	return NeedsValue;
}

float ConsoleVariable::GetFloatValue() const
{
	return CurrentValue.F_Value;
}

void ConsoleVariable::Execute()
{
	if (ExecuteFunction)
	{
		ExecuteFunction();
	}
}

bool ConsoleVariable::HasValue() const
{
	return RawValueString.length() > 0;
}

void ConsoleVariable::SetRawValue(std::string value)
{
	RawValueString = value;
}

ConsoleVariable::ConsoleVariable(std::string name, ECVarType::Type cvartype, bool needsValue)
{
	Type = cvartype;
	NeedsValue = needsValue;
	if (cvartype != ECVarType::LaunchOnly)
	{
		int Index = 0;
		if (VectorUtils::Contains(ConsoleVariableManager::Get()->ConsoleVars, this, Index))
		{
			ConsoleVariableManager::Get()->ConsoleVars[Index]->LinkedVars.push_back(this);
		}
		else
		{
			ConsoleVariableManager::Get()->ConsoleVars.push_back(this);
		}
	}
	else if (cvartype != ECVarType::ConsoleOnly)
	{
		int Index = 0;
		if (VectorUtils::Contains(ConsoleVariableManager::Get()->LaunchArgs, this, Index))
		{
			ConsoleVariableManager::Get()->LaunchArgs[Index]->LinkedVars.push_back(this);
		}
		else
		{
			ConsoleVariableManager::Get()->LaunchArgs.push_back(this);
		}
	}
	ConsoleVariableManager::Get()->AllVars.push_back(this);
	Name = name;
	std::transform(Name.begin(), Name.end(), Name.begin(), ::tolower);
}

std::string ConsoleVariable::GetValueString()
{
	if (IsFloat)
	{
		return std::to_string(GetFloatValue());
	}
	return	std::to_string(GetIntValue());
}

std::string ConsoleVariable::GetRawValueString()
{
	return RawValueString;
}

bool GetValueClean(std::string value, int& outvalue)
{
	if (value.find('-') != -1)
	{
		outvalue = -1;
		return false;
	}
	try
	{
		outvalue = stoi(value);
	}
	catch (const std::exception&)
	{
		return false;
	}
	return true;
}

ConsoleVariableManager* ConsoleVariableManager::Get()
{
	if (Instance == nullptr)
	{
		Instance = new ConsoleVariableManager();
	}
	return Instance;
}

void ConsoleVariableManager::SetupCVarsFromCFG(std::vector<std::string>& VarLines)
{
	for (int i = 0; i < VarLines.size(); i++)
	{
		ProcessVarString(VarLines[i], false);
	}
}

void ConsoleVariableManager::ProcessVarString(std::string value, bool LaunchOnly)
{
	std::transform(value.begin(), value.end(), value.begin(), ::tolower);
	std::vector<std::string> SplitArgs = StringUtils::Split(value, ' ');
	for (ConsoleVariable* CV : Instance->AllVars)
	{
		if (LaunchOnly && (CV->Type == ECVarType::LaunchOnly || CV->Type == ECVarType::ConsoleAndLaunch))
		{
			continue;
		}
		for (int i = 0; i < SplitArgs.size(); i++)
		{
			std::string Arg = SplitArgs[i];
			if (Arg.compare(CV->GetName()) == 0)
			{
				if (i + 1 < SplitArgs.size())
				{
					int parsedvalue = -1;
					CV->SetRawValue(SplitArgs[i + 1]);
					if (GetValueClean(SplitArgs[i + 1], parsedvalue))
					{
						i++;
					}
					else
					{
						Log::LogMessage("Argument " + CV->GetLaunchName() + " Is missing Value, -1 assumed", Log::Severity::Warning);
					}
					CV->SetValue(parsedvalue);
				}
				else
				{
					Log::LogMessage("Argument " + CV->GetLaunchName() + " Is missing Value, -1 assumed", Log::Severity::Warning);
					CV->SetValue(-1);
				}
			}
		}
	}
}

void ConsoleVariableManager::GetCFGVariables(std::vector<std::string> &Lines)
{
	for (ConsoleVariable* CV : Instance->AllVars)
	{
		if (CV->Type == ECVarType::LaunchOnly)
		{
			continue;
		}
		if (!CV->IsDefaultValue())
		{
			Lines.push_back(CV->GetName() + " " + std::to_string(CV->GetIntValue()));
		}
	}
}

void ConsoleVariableManager::SetupVars(std::string LaunchArgString)
{
	std::transform(LaunchArgString.begin(), LaunchArgString.end(), LaunchArgString.begin(), ::tolower);
	std::vector<std::string> SplitArgs = StringUtils::Split(LaunchArgString, ' ');
	for (ConsoleVariable* CV : Instance->LaunchArgs)
	{
		for (int i = 0; i < SplitArgs.size(); i++)
		{
			std::string Arg = SplitArgs[i];
			if (Arg.compare(CV->GetLaunchName()) == 0)
			{
				if (CV->IsValueVar())
				{
					if (i + 1 < SplitArgs.size())
					{
						std::string parsetarget = "";
						CV->SetRawValue(SplitArgs[i + 1]);
						int parsedvalue = -1;
						if (GetValueClean(SplitArgs[i + 1], parsedvalue))
						{
							i++;
						}
						else
						{
							Log::LogMessage("Int Argument " + CV->GetLaunchName() + " Is missing Value, -1 assumed", Log::Severity::Warning);
						}
						CV->SetValue(parsedvalue);
					}
					else
					{
						Log::LogMessage("Argument " + CV->GetLaunchName() + " Is missing Value, -1 assumed", Log::Severity::Warning);
						CV->SetValue(-1);
					}
				}
				else
				{
					CV->SetValue(1);
				}
			}
		}
	}
}

bool ConsoleVariableManager::TrySetCVar(std::string command, ConsoleVariable** Var)
{
	if (command.length() == 0)
	{
		return false;
	}
	std::transform(command.begin(), command.end(), command.begin(), ::tolower);
	std::vector<std::string> SplitArgs = StringUtils::Split(command, ' ');
	if (SplitArgs.size() == 0)
	{
		return false;
	}
	for (ConsoleVariable* CV : Instance->ConsoleVars)
	{
		if (CV->GetName() == SplitArgs[0])
		{
			*Var = CV;
			if (CV->NeedsValue)
			{
				if (SplitArgs.size() > 1)
				{
					if (CV->IsFloat)
					{
						float value = stof(SplitArgs[1]);
						CV->SetValueF(value);
					}
					else
					{
						int value = stoi(SplitArgs[1]);
						CV->SetValue(value);
					}
					return true;
				}
			}
			else
			{
				CV->Execute();
			}
			return true;
		}
	}
	return false;
}
static ConsoleVariable Help("help", ECVarType::ConsoleOnly, std::bind(&ConsoleVariableManager::DebugLogCVars));
void ConsoleVariableManager::DebugLogCVars()
{
	Log::LogMessage("Console Vars");
	for (int i = 0; i < Get()->ConsoleVars.size(); i++)
	{
		Log::LogMessage(Get()->ConsoleVars[i]->GetName());
	}
}

void ConsoleVariableManager::ExecuteCommand(std::string command)
{
	ConsoleVariable* Var = nullptr;
	std::string Response = "Command Unknown: " + command;
	if (ConsoleVariableManager::TrySetCVar(command, &Var))
	{
		Response = (Var->GetName() + " " + Var->GetValueString());
	}
	Log::LogMessage(Response);
}

void ConsoleVariableManager::ToggleVar(std::string var)
{
	for (int i = 0; i < Get()->ConsoleVars.size(); i++)
	{
		if (Get()->ConsoleVars[i]->GetName() == var)
		{
			Get()->ConsoleVars[i]->SetValue(!Get()->ConsoleVars[i]->GetBoolValue());
			return;
		}
	}

}

ConsoleVariable* ConsoleVariableManager::Find(std::string name)
{
	for (int i = 0; i < ConsoleVars.size(); i++)
	{
		if (ConsoleVars[i]->GetName() == name)
		{
			return ConsoleVars[i];
		}
	}
	return nullptr;
}

void IConsoleSettings::GetVariables(std::vector<ConsoleVariable*>& VarArray)
{
	Seralise();
	for (auto itor = VarMap.begin(); itor != VarMap.end(); itor++)
	{
		itor->second.Cvar = new ConsoleVariable(itor->first, 0, ECVarType::ConsoleAndLaunch, false);
		VarArray.push_back(itor->second.Cvar);
	}
}

IConsoleSettings::~IConsoleSettings()
{}

void IConsoleSettings::GatherData()
{
	IsReading = true;
	Seralise();
}

void IConsoleSettings::LinkProp(std::string name, int* value)
{
	if (IsReading)
	{
		IConsoleSettingsVar* Var = nullptr;
		auto itor = VarMap.find(name);
		if (itor != VarMap.end())
		{
			Var = &itor->second;
		}
		if (Var != nullptr && Var->Cvar != nullptr)
		{
			*value = Var->Cvar->GetIntValue();
		}
	}
	else
	{
		VarMap.emplace(name, IConsoleSettingsVar(value));
	}
}
