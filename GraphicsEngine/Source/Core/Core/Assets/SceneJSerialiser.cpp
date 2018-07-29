#include "stdafx.h"
#include "SceneJSerialiser.h"
#include "Scene.h"
#include "Core/GameObject.h"
#include <iostream>
#include <fstream>
#include "rapidjson\prettywriter.h"
#include "Core/Engine.h"
#include "Core/Utils/FileUtils.h"
rapidjson::Document::AllocatorType* SceneJSerialiser::jallocator = nullptr;
SceneJSerialiser::SceneJSerialiser()
{
	testpath = Engine::GetRootDir();
	testpath.append("\\asset\\scene\\test.scene");
}


SceneJSerialiser::~SceneJSerialiser()
{
}

void SceneJSerialiser::SaveScene(Scene* target, std::string path)
{
	jallocator = &doc.GetAllocator();
	SerialiseObjects(target);
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	doc.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
	//puts(sb.GetString());
	WriteToFile(path,sb.GetString());
	//write
}
void SceneJSerialiser::LoadScene(Scene* target,std::string path)
{
	if (!FileUtils::File_ExistsTest(path))
	{
		return;
	}
	std::string data = GetFile(path);
	rapidjson::StringStream JStream(data.c_str());
	doc.ParseStream(JStream);
	DeserialiseObjects(target);
}
void SceneJSerialiser::SerialiseObjects(Scene* target)
{

	doc.SetObject();
	std::vector<GameObject*> objects = *target->GetObjects();
	rapidjson::Value jsonGOs(rapidjson::kArrayType);
	for (int i = 0; i < objects.size(); i++)
	{
		rapidjson::Value jsongovalue(rapidjson::kObjectType);
		objects[i]->SerialiseGameObject(jsongovalue);
		jsonGOs.PushBack(jsongovalue, doc.GetAllocator());
	}
	SerialHelpers::addJsonValue(doc, doc.GetAllocator(), "GOS", jsonGOs);

}
void SceneJSerialiser::DeserialiseObjects(Scene* Target)
{
	const auto& gos = doc.FindMember("GOS");
	if (gos == doc.MemberEnd())
	{
		return;
	}

	//std::vector<GameObject*> newobjects;
	for (unsigned int i = 0; i < gos->value.Size(); i++)
	{
		GameObject* newgo = new GameObject();
		newgo->DeserialiseGameObject(gos->value[i]);
		Target->AddGameobjectToScene(newgo);
	/*	newobjects.push_back(newgo);*/
	}
}

std::string SceneJSerialiser::GetFile(std::string filename)
{
	std::string out;
	std::ifstream myfile(filename);
	if (myfile.is_open())
	{
		std::string line;
		while (std::getline(myfile, line))
		{
			out.append(line);
		}
		myfile.close();
	}
	else
	{
		Log::OutS  << "failed to load " << filename << Log::OutS;
	}
	return out;
}
bool SceneJSerialiser::WriteToFile(std::string filename, std::string data)
{
	std::string out;
	std::ofstream myfile(filename, std::ofstream::out);
	if (myfile.is_open())
	{
		std::string line;
		myfile.write(data.c_str(), data.length());
		myfile.close();
	}
	else
	{
		Log::OutS  << "failed to save " << filename << Log::OutS;
		return false;
	}
	return true;
}