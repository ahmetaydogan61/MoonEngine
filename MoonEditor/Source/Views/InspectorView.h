#pragma once
#include "Views/Views.h"

namespace MoonEngine
{
	struct InspectorView : public BasicView
	{
		InspectorView();
		~InspectorView() = default;
		void Render();
	};
}