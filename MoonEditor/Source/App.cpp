#include "mpch.h"
#include "Editor/EditorLayer.h"
#include "Editor/EditorAssets.h"

#include <Core/Application.h>

int main()
{
	using namespace MoonEngine;

	WindowPrefs wPrefs =
	{
		.Resolution{1280, 720},
		.VsyncOn = true,
		.Fullscreen = false,
		.MaximizeOnStart = true
	};

	ApplicationPrefs prefs =
	{
		.AppName = "Moon Engine",
		.Window = wPrefs
	};

	Application* application = new Application(prefs);
	EditorAssets::LoadTextures();
	application->PushLayer(MakeShared<EditorLayer>());
	application->Run();
}
