#pragma once
#include "Rendering/Core/Mesh.h"
#include "Transform.h"

#include "Rendering/Core/Material.h"
#include "Editor/IInspectable.h"
#include "physx\PxShape.h"
#include "physx\PxRigidStatic.h"
#include "rapidjson\document.h"
#include "Physics/PhysicsEngine.h"
class Component;
//class RigidBody;
class MeshRendererComponent;
class GameObject : public IInspectable
{
public:
	enum EMoblity { Static, Dynamic };
	CORE_API GameObject(std::string name = "", EMoblity stat = EMoblity::Static, int ObjectID = -1);
	CORE_API ~GameObject();

	CORE_API Transform* GetTransform();
	CORE_API class Scene* GetScene();
	void Internal_SetScene(Scene* scene);

	//Update
	CORE_API void FixedUpdate(float delta);
	CORE_API void Update(float delta);
	CORE_API void BeginPlay();

	void Render(bool ignoremat, RHICommandList* list);
	EMoblity GetMobility();

	//temp
	Mesh* GetMesh();
	RigidBody* actor;

	//getters
	Material* GetMat();
	std::string GetName() { return Name; }
	void SetName(std::string name) { Name = name; }
	bool GetReflection();
	bool GetDoesUseMainShader();
	bool HasCached = false;

	//Editor only
	void EditorUpdate();
	physx::PxRigidStatic* SelectionShape;
	CORE_API Component* AttachComponent(Component* Component);
	std::vector<Component*> GetComponents();
	template<class T>
	T* GetComponent();

	void CopyPtrs(GameObject* newObject);

	void ProcessSerialArchive(class Archive* Arch);

	void PostChangeProperties();
	void ChangePos_editor(glm::vec3 NewPos);
	template<class T>
	std::vector<T*> GetAllComponentsOfType()
	{
		std::vector<T*> Comps;
		for (int i = 0; i < m_Components.size(); i++)
		{
			T* Target = dynamic_cast<T*>(m_Components[i]);
			if (Target != nullptr)
			{
				Comps.push_back(Target);
			}
		}
		return Comps;
	}
private:
	//all object created from scene will have id 
	//other wise -1 is value for non scene objects 
	int ObjectID = 0;
	std::string Name;
	glm::vec3 PositionDummy;

	Transform* m_transform = nullptr;
	EMoblity Mobilty;
	std::vector<Component*> m_Components;
	MeshRendererComponent* m_MeshRenderer = nullptr;
	GameObject* mParent = nullptr;
	std::vector<GameObject*> Children;
	virtual std::vector<Inspector::InspectorProperyGroup> GetInspectorFields() override;
	Scene* OwnerScene;
	const char * ComponentArrayKey = "Components";
};

template<class T>
inline T * GameObject::GetComponent()
{
	for (int i = 0; i < m_Components.size(); i++)
	{
		T* Target = dynamic_cast<T*>(m_Components[i]);
		if (Target != nullptr)
		{
			return Target;
		}
	}
	return nullptr;
}
