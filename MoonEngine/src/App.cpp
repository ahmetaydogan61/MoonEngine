#include "Core.h"
using namespace MoonEngine;

class ApplicationLayer : Layer
{
	OrthographicCamera* camera;
	CameraControllerOrthographic* cameraController;
	Scene* scene;

	void Create()
	{
		LayerName = "Application Layer";
		Renderer::Init();
		camera = new OrthographicCamera(Window::GetAspectRatio());
		cameraController = new CameraControllerOrthographic(*camera);
		scene = new Scene();
		scene->Create(camera);
		scene->CreateEntity();
	}

	void Update()
	{
		cameraController->OnUpdate();
		scene->Update();
	}

	void DrawGUI()
	{
		ImGui::Begin("Debug");
		ImGui::Button("Hello World");
		ImGui::End();
	}
};


int main()
{
	Application* app = new Application();
	ApplicationLayer* appLayer = new ApplicationLayer();
	app->PushLayer((Layer*)appLayer);
	app->Run();
	delete app;
}