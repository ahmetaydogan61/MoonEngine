#include "mpch.h"
#include "EditorLayer.h"
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
	application->PushLayer(MakeShared<EditorLayer>());
	application->Run();
}
