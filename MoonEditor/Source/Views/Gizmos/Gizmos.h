#pragma once

namespace MoonEngine
{
	enum class GizmoSelection
	{
		//These are the values from ImGuizmo OPERATION enum
		NONE = -1,
		TRANSLATE = 7,
		RORTATE = 120,
		SCALE = 896,
	};

	struct GizmosData
	{
		bool ShowGizmos = false;
		float IconSize = 0.5f;
		float LineWidth = 3.0f;
		glm::vec4 GizmosColor = glm::vec4(1.0f);

		bool HighlightSelected = true;
		bool ShowAllColliders = false;

		//Translation
		GizmoSelection GizmoSelection = GizmoSelection::TRANSLATE;
		float SnapAmount = 0.25f;
		bool IsSnapping = false;
		bool IsUsing = false;
	};
}