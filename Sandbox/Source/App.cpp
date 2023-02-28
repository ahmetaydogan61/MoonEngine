#include "mpch.h"
#include <Core/Application.h>
#include <Core/Input.h>
#include <Renderer/Renderer.h>

#define NOT_ADDED_STDLIB_CPP 
#include <Gui/ImFileBrowser.h>

using namespace MoonEngine;

class SandboxLayer : public ApplicationLayer
{
public:
	SandboxLayer()
	{
		m_Renderer = MakeShared<Renderer>();
	}
	~SandboxLayer() = default;

	Shared<Renderer> m_Renderer;
	ImGui::ImGuiFileBrowser dialog;

	void Init()
	{
		dialog = ImGuiFileBrowserConfig
		{
			.Label = "File Dialog",
			.Flags = ImGuiFileBrowserFlags_AllowRename | ImGuiFileBrowserFlags_AllowCreateFolder | ImGuiFileBrowserFlags_FilterExtensions | ImGuiFileBrowserFlags_SelectAll,
			.StartDirectory = "Resource/Assets",
			.RootDirectory = "Resource/Assets",
			.Extensions = {".moonscn" }
		};
	}

	void Update()
	{
		m_Renderer->Clear();

		if (!dialog.IsActive() && Input::GetMouseButtonDown(2))
			dialog.OpenFileBrowser();

		if (dialog.IsActive() && Input::GetMouseButtonDown(2))
			dialog.CloseFileBrowser();

	}

	void DrawGui()
	{
		dialog.BeginFileBrowser();
	}

};

int main()
{
	ApplicationPrefs prefs =
	{
		.AppName = "Moon Engine",
		.Resolution{1280, 720},
		.VsyncOn = true,
		.MaximizeOnStart = true
	};

	Application* application = new Application(prefs);
	application->PushLayer(MakeShared<SandboxLayer>());
	application->Run();
}
