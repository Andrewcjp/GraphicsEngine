#include "UIManager.h"
#include "Rendering/Renderers/TextRenderer.h"
#include "glm\glm.hpp"
#include "Core/UIWidget.h"
#include "Core/UIBox.h"
#include "Core/UIButton.h"
#include "Core/UIListBox.h"
#include "Core/UILabel.h"
#include "Core/GameObject.h"
#include "Core/Input/Input.h"
#include "UI/EditorUI/UIEditField.h"
#include "UI/GameUI/UIGraph.h"
#include "Rendering/Core/GPUStateCache.h"
#include "Core/Performance/PerfManager.h"
#include "UI/Core/UIDrawBatcher.h"
#include "GameUI/DebugConsole.h"
#include "Editor/Inspector.h"
#include "Editor/EditorWindow.h"

#include "UI/Core/UIPopoutbox.h"
#include "UI/EditorUI/UIAssetManager.h"
#include "UI/GameUI/UIDropDown.h"
#include "Rendering/Core/DebugLineDrawer.h"
#include "Editor/EditorCore.h"
#include <algorithm>
UIManager* UIManager::instance = nullptr;
UIWidget* UIManager::CurrentContext = nullptr;
UIManager::UIManager()
{}

UIManager::UIManager(int w, int h)
{
	BottomHeight = 0.2f;
	TopHeight = 0.1f;
	RightWidth = 0.2f;
	LeftWidth = 0.2f;
	LineBatcher = new DebugLineDrawer(true);
	DrawBatcher = new UIDrawBatcher();
	Initalise(w, h);
	instance = this;
#if WITH_EDITOR
	InitEditorUI();
#endif
}

void UIManager::InitEditorUI()
{

	BottomHeight = 0.25f;
	UIBox* TOP = new UIBox(m_width, GetScaledHeight(0.2f), 0, 0);
	TOP->SetScaled(1.0f, TopHeight, 0.0f, 1.0f - TopHeight);
	AddWidget(TOP);

	UIGraph* graph = new UIGraph(LineBatcher, 250, 150, 15, 25);
	Graph = graph;
	AddWidget(graph);
#if WITH_EDITOR
	inspector = new Inspector(m_width, GetScaledHeight(0.2f), 0, 0);
	inspector->SetScaled(RightWidth, 1.0f - (TopHeight), 1 - RightWidth);
	AddWidget(inspector);

	ViewportRect = CollisionRect(GetScaledWidth(0.70f), GetScaledHeight(0.70f), 0, 0);
	UIButton* button = new UIButton(200, 50, 0, 500);
	button->SetScaled(0.05f, 0.075f, 0.5f - 0.05f, 1.0f - (TopHeight));
	button->BindTarget(std::bind(&EditorWindow::EnterPlayMode, EditorWindow::GetInstance()));
	button->SetText("Play");
	AddWidget(button);
	button = new UIButton(200, 50, 0, 500);
	button->SetScaled(0.05f, 0.075f, 0.5f, 1.0f - (TopHeight));
	button->BindTarget(std::bind(&EditorWindow::ExitPlayMode, EditorWindow::GetInstance()));
	button->SetText("Stop");
	AddWidget(button);
	button = new UIButton(200, 50, 0, 500);
	button->SetScaled(0.05f, 0.075f, 0.5f + 0.05f, 1.0f - (TopHeight));
	button->BindTarget(std::bind(&EditorWindow::Eject, EditorWindow::GetInstance()));
	button->SetText("Eject");
	AddWidget(button);
#endif
	//testbox = new UIPopoutbox(100, 300, 250, 150);
	//testbox->SetScaled(RightWidth, TopHeight * 2, 0.5f - (RightWidth / 2), 0.5f - (TopHeight * 2 / 2));
	//AddWidget(testbox);
	DebugConsole* wid = new DebugConsole(100, 100, 100, 100);
	wid->SetScaled(1.0f, 0.05f, 0.0f, 0);
	AddWidget(wid);

	AssetManager = new UIAssetManager();
	AssetManager->SetScaled(1.0f - RightWidth, BottomHeight);
	AddWidget(AssetManager);
	UpdateBatches();
}

void UIManager::CreateDropDown(std::vector<std::string> &options, float width, float height, float x, float y, std::function<void(int)> Callback)
{
	UIDropDown * testbox3 = new UIDropDown(100, 300, 250, 150); //new UIDropDown(100, 300, x, y);
	testbox3->Priority = 10;
	testbox3->SetScaled(width, height * 2, x, y);

	for (int i = 0; i < options.size(); i++)
	{
		testbox3->AddItem(options[i]);
	}
	if (Callback)
	{
		testbox3->SelectionChanged = Callback;
	}
	AddWidget(testbox3);
	testbox3->UpdateScaled();
	instance->DropdownCurrent = testbox3;
	//UIManager::UpdateBatches();
}

void UIManager::AlertBox(std::string MSg)
{
	UIPopoutbox* testbox = new UIPopoutbox(100, 300, 250, 150);
	testbox->SetScaled(RightWidth, TopHeight * 2, 0.5f - (RightWidth / 2), 0.5f - (TopHeight * 2 / 2));
	testbox->SetText(MSg);
	AddWidget(testbox);
	UIManager::UpdateBatches();
}

UIManager::~UIManager()
{
	textrender.reset();
	for (int i = 0; i < widgets.size(); i++)
	{
		delete widgets[i];
	}
	delete LineBatcher;
	delete DrawBatcher;
}

void UIManager::Initalise(int width, int height)
{
	m_width = width;
	m_height = height;
	textrender = std::make_unique<TextRenderer>(m_width, m_height);
}

Inspector* UIManager::GetInspector()
{
	return inspector;
}

void UIManager::RenderTextToScreen(int id, std::string text)
{
	RenderTextToScreen(id, text, glm::vec3(0.6, 1.0f, 0.2f));
}

void UIManager::RenderTextToScreen(int id, std::string text, glm::vec3 colour)
{
	RenderTextToScreen(text, XSpacing, static_cast<float>(m_height - (YHeight*id)), 0.5f, colour);
}

void UIManager::RenderTextToScreen(std::string text, float x, float y, float scale, glm::vec3 colour)
{
	textrender->RenderFromAtlas(text, x, y, scale, colour, false);
}

void UIManager::UpdateSize(int width, int height)
{
	LineBatcher->OnResize(width, height);
	DrawBatcher->ClearVertArray();
	m_width = width;
	m_height = height;
	ViewportRect = CollisionRect(GetScaledWidth(0.60f), GetScaledHeight(0.60f), GetScaledWidth(LeftWidth), GetScaledHeight(BottomHeight));

	struct less_than_key
	{
		bool operator() (UIWidget* struct1, UIWidget* struct2)
		{
			return (struct1->Priority > struct2->Priority);
		}
	};

	std::sort(widgets.begin(), widgets.end(), less_than_key());

	for (int i = (int)widgets.size() - 1; i >= 0; i--)
	{
		widgets[i]->UpdateScaled();
	}

	textrender->UpdateSize(width, height);
	DrawBatcher->SendToGPU();
}

void UIManager::AddWidget(UIWidget * widget)
{
	widgets.push_back(widget);
}

void UIManager::UpdateBatches()
{
	if (instance != nullptr)
	{
		instance->UpdateSize(instance->m_width, instance->m_height);
	}
}

void UIManager::UpdateWidgets()
{
	for (int i = 0; i < widgets.size(); i++)
	{
		widgets[i]->UpdateData();
	}
	CleanUpWidgets();
}

void UIManager::RenderWidgets()
{
	//todo: move to not run every frame?
	DrawBatcher->RenderBatches();

#if UISTATS
	PerfManager::StartTimer("Line");
#endif
	LineBatcher->GenerateLines();
	LineBatcher->RenderLines();
#if UISTATS
	PerfManager::EndTimer("Line");
#endif
	//todo: GC?
}

void UIManager::RenderWidgetText()
{
	for (int i = 0; i < widgets.size(); i++)
	{
		if (widgets[i]->GetEnabled())
		{
			widgets[i]->Render();
		}
	}
}

//todo: prevent issue with adding while itoring!
void UIManager::MouseMove(int x, int y)
{
	for (int i = 0; i < widgets.size(); i++)
	{
		if (!widgets[i]->IsPendingKill)
		{
			widgets[i]->MouseMove(x, y);
		}
	}
	Blocking = !(ViewportRect.Contains(x, y));
#if 0
	widgets[2]->Colour = Blocking ? glm::vec3(1) : glm::vec3(0);
	UpdateBatches();
#endif
}

void UIManager::MouseClick(int x, int y)
{
	for (int i = (int)widgets.size() - 1; i >= 0; i--)
	{
		if (!widgets[i]->IsPendingKill)
		{
			widgets[i]->MouseClick(x, y);
		}
	}
}

void UIManager::MouseClickUp(int x, int y)
{
	for (int i = 0; i < widgets.size(); i++)
	{
		if (!widgets[i]->IsPendingKill)
		{
			widgets[i]->MouseClickUp(x, y);
		}
	}
}

void UIManager::InitGameobjectList(std::vector<GameObject*>*gos)
{
	GameObjectsPtr = gos;
	box = new UIListBox(GetScaledWidth(0.15f), GetScaledHeight(.8f), 0, GetScaledHeight(0.2f));
	AddWidget(box);
	RefreshGameObjectList();
}

void UIManager::UpdateGameObjectList(std::vector<GameObject*>*gos)
{
	GameObjectsPtr = gos;
}

void UIManager::SelectedCallback(int i)
{
#if WITH_EDITOR
	if (instance)
	{
		EditorWindow::GetEditorCore()->SetSelectedObjectIndex(i);
	}
#endif
}

void UIManager::RefreshGameObjectList()
{
#if WITH_EDITOR
	if (box != nullptr && GameObjectsPtr != nullptr)
	{
		box->RemoveAll();
		box->SetScaled(LeftWidth, 1.0f - (BottomHeight + TopHeight), 0.0, BottomHeight);
		using std::placeholders::_1;
		box->SelectionChanged = std::bind(&UIManager::SelectedCallback, _1);
		for (int i = 0; i < (*(GameObjectsPtr)).size(); i++)
		{
			(*GameObjectsPtr)[i]->PostChangeProperties();
			box->AddItem((*(GameObjectsPtr))[i]->GetName().c_str());
		}
		GetInspector()->Refresh();
		UpdateBatches();
	}
#endif
}

int UIManager::GetWidth()
{
	return m_width;
}

int UIManager::GetHeight()
{
	return m_height;
}

int UIManager::GetScaledWidth(float PC)
{
	return (int)rint(PC *(instance->GetWidth()));
}

int UIManager::GetScaledHeight(float PC)
{
	return (int)rint(PC *(instance->GetHeight()));
}

bool UIManager::IsUIBlocking()
{
	return Blocking;
}

UIWidget * UIManager::GetCurrentContext()
{
	if (instance != nullptr)
	{
		return instance->CurrentContext;
	}
	return nullptr;
}

void UIManager::SetCurrentcontext(UIWidget * widget)
{
	if (instance != nullptr)
	{
		instance->CurrentContext = widget;
	}
}

void UIManager::RemoveWidget(UIWidget* widget)
{
	widget->IsPendingKill = true;
	WidgetsToRemove.push_back(widget);
}

void UIManager::CleanUpWidgets()
{
	if (WidgetsToRemove.empty())
	{
		return;
	}
	for (int x = 0; x < widgets.size(); x++)
	{
		for (int i = 0; i < WidgetsToRemove.size(); i++)
		{
			if (widgets[x] == WidgetsToRemove[i])//todo: performance of this?
			{
				widgets.erase(widgets.begin() + x);
				delete WidgetsToRemove[i];//delete and dont realloc?
				WidgetsToRemove[i] = nullptr;
				break;
			}

		}
	}
	WidgetsToRemove.clear();
	UpdateBatches();
}

void UIManager::CloseDropDown()
{
	if (instance != nullptr && instance->DropdownCurrent != nullptr)
	{
		instance->RemoveWidget(instance->DropdownCurrent);
		instance->DropdownCurrent = nullptr;
	}
}

IntRect UIManager::GetEditorRect()
{
	IntRect rect;
	rect.Min.x = GetScaledWidth(LeftWidth);
	rect.Min.y = GetScaledHeight(TopHeight);

	rect.Max.x = GetScaledWidth(0.8f - RightWidth);
	rect.Max.y = GetScaledHeight(0.9f - BottomHeight);
	return rect;
}

