#include "MoonEngine.h"
#include "Core/Window.h"

using namespace MoonEngine;

class ApplicationLayer : Layer
{
	OrthographicCamera* camera;
	CameraControllerOrthographic* cameraController;

	std::string LayerName = "Application Layer";

	void Create()
	{
		Renderer::Init();
		camera = new OrthographicCamera(Window::GetAspectRatio());
		cameraController = new CameraControllerOrthographic(*camera);
	}

	void Update()
	{
		Renderer::Clear();
		cameraController->OnUpdate();
		Renderer::DrawQuad(glm::vec2(), glm::vec2(1.0f), glm::vec3(1.0f));
		Renderer::Render(*camera);
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