#include "Editor/EditorLayer.h"

using namespace MoonEngine;

int main()
{
	Application* app = new Application();
	app->PushLayer(new EditorLayer());
	app->Run();
	delete app;
}