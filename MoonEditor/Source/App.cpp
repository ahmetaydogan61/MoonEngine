#include "mpch.h"
#include "EditorLayer.h"
#include <Core/Application.h>

int main()
{
	using namespace MoonEngine;

	ApplicationPrefs prefs =
	{
		.AppName = "Moon Engine",
		.Resolution{1280, 720},
		.VsyncOn = true,
		.MaximizeOnStart = true
	};

	Application* application = new Application(prefs);
	application->PushLayer(MakeShared<EditorLayer>());
	application->Run();
}
