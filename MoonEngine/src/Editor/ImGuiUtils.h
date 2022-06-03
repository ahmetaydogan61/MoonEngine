#pragma once
#include "imgui/imgui.h"

namespace ImGuiUtils
{
	static void AddPadding(float x, float y)
	{
		ImVec2 p0 = ImGui::GetCursorScreenPos();
		p0.x += x;
		p0.y += y;
		ImGui::SetCursorScreenPos(p0);
	}

	static void AddPaddingToRect(float offset, float ratio)
	{
		ImVec2 p0 = ImGui::GetCursorScreenPos();
		p0.x += offset - ImGui::GetItemRectSize().x;
		ImGui::SetCursorScreenPos(p0);
	}

	static void TextCentered(const char* text, bool sameLine)
	{
		ImVec4 buttonColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
		ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
		ImGui::Button(text); 
		if (sameLine)
			ImGui::SameLine();
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();
		
	}

	static void Image(ImTextureID textID, const ImVec2& size)
	{
		ImGui::Image(textID, size, { 0, 1 }, { 1, 0 });
	}

	static bool ImageButton(ImTextureID textID, const ImVec2& size)
	{
		return ImGui::ImageButton(textID, size, { 0, 1 }, { 1, 0 });
	}

	static void StyleCustomDark(int cartoony)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		//Colors
		colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
		colors[ImGuiCol_TextDisabled] = ImColor(150, 150, 150, 255);

		colors[ImGuiCol_WindowBg] = ImColor(40, 40, 40, 255);
		colors[ImGuiCol_ChildBg] = ImColor(40, 40, 40, 255);
		colors[ImGuiCol_PopupBg] = ImColor(15, 15, 15, 255);

		colors[ImGuiCol_Border] = ImColor(0, 0, 0, 155);
		colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 255);

		colors[ImGuiCol_FrameBg] = ImColor(15, 15, 15, 255);
		colors[ImGuiCol_FrameBgHovered] = ImColor(25, 25, 25, 255);
		colors[ImGuiCol_FrameBgActive] = ImColor(50, 50, 50, 255);

		colors[ImGuiCol_TitleBg] = ImColor(15, 15, 15, 255);
		colors[ImGuiCol_TitleBgActive] = ImColor(15, 15, 15, 255);
		colors[ImGuiCol_TitleBgCollapsed] = ImColor(15, 15, 15, 255);

		colors[ImGuiCol_MenuBarBg] = ImColor(15, 15, 15, 255);

		colors[ImGuiCol_ScrollbarBg] = ImColor(15, 15, 15, 255);
		colors[ImGuiCol_ScrollbarGrab] = ImColor(125, 125, 125, 255);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(150, 150, 150, 255);
		colors[ImGuiCol_ScrollbarGrabActive] = ImColor(150, 150, 150, 255);

		colors[ImGuiCol_CheckMark] = ImColor(0, 175, 255, 255);
		colors[ImGuiCol_SliderGrab] = ImColor(0, 150, 255, 255);
		colors[ImGuiCol_SliderGrabActive] = ImColor(0, 175, 255, 255);

		colors[ImGuiCol_Button] = ImColor(100, 100, 100, 255);
		colors[ImGuiCol_ButtonHovered] = ImColor(0, 150, 255, 255);
		colors[ImGuiCol_ButtonActive] = ImColor(0, 175, 255, 255);

		colors[ImGuiCol_Header] = ImColor(0, 150, 255, 225);
		colors[ImGuiCol_HeaderHovered] = ImColor(125, 125, 125, 255);
		colors[ImGuiCol_HeaderActive] = ImColor(125, 125, 125, 255);

		colors[ImGuiCol_Separator] = ImColor(15, 15, 15, 255);
		colors[ImGuiCol_SeparatorHovered] = ImColor(200, 200, 200, 255);
		colors[ImGuiCol_SeparatorActive] = ImColor(250, 250, 250, 255);

		colors[ImGuiCol_ResizeGrip] = ImColor(100, 100, 100, 50);
		colors[ImGuiCol_ResizeGripHovered] = ImColor(125, 125, 125, 200);
		colors[ImGuiCol_ResizeGripActive] = ImColor(125, 125, 125, 250);

		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 1.00f, 0.75f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

		//Sizes
		//Main
		style.WindowPadding = ImVec2(5, 5);
		style.FramePadding = ImVec2(6, 4);
		style.ItemSpacing = ImVec2(6, 3);
		style.ScrollbarSize = 14;
		style.GrabMinSize = 10;

		//Borders
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 0;
		style.FrameBorderSize = cartoony;
		style.PopupBorderSize = 0;

		style.WindowRounding = 3;
		style.ChildRounding = 3;
		style.FrameRounding = 3;
		style.PopupRounding = 3;
		style.ScrollbarRounding = 5;
		style.GrabRounding = 12;

		style.WindowMenuButtonPosition = ImGuiDir_None;

#ifdef IMGUI_HAS_DOCK 
		style.TabBorderSize = cartoony;
		style.TabRounding = 8;

		colors[ImGuiCol_Tab] = ImColor(15, 15, 15, 255);
		colors[ImGuiCol_TabHovered] = ImColor(45, 45, 45, 255);
		colors[ImGuiCol_TabActive] = ImColor(45, 45, 45, 255);
		colors[ImGuiCol_TabUnfocused] = ImColor(15, 15, 15, 255);
		colors[ImGuiCol_TabUnfocusedActive] = ImColor(45, 45, 45, 255);
		colors[ImGuiCol_DockingPreview] = ImColor(255, 255, 255, 50);
		colors[ImGuiCol_DockingEmptyBg] = ImColor(50, 50, 50, 255);

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
#endif
	}

	static void StyleCustomGray(int is3D)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		//Colors
		colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
		colors[ImGuiCol_TextDisabled] = ImColor(150, 150, 150, 255);

		ImColor window = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_WindowBg] = window;
		colors[ImGuiCol_ChildBg] = window;
		colors[ImGuiCol_PopupBg] = window;

		colors[ImGuiCol_Border] = ImColor(75, 75, 75, 150);
		colors[ImGuiCol_BorderShadow] = ImColor(150, 150, 150, 50);

		colors[ImGuiCol_FrameBg] = ImColor(100, 100, 100, 255);
		colors[ImGuiCol_FrameBgHovered] = ImColor(125, 125, 125, 255);
		colors[ImGuiCol_FrameBgActive] = ImColor(150, 150, 150, 255);

		colors[ImGuiCol_TitleBg] = ImColor(20, 20, 20, 255);
		colors[ImGuiCol_TitleBgActive] = ImColor(10, 10, 10, 255);
		colors[ImGuiCol_TitleBgCollapsed] = ImColor(20, 20, 20, 255);

		colors[ImGuiCol_MenuBarBg] = window;

		colors[ImGuiCol_ScrollbarBg] = ImColor(75, 75, 75, 150);
		colors[ImGuiCol_ScrollbarGrab] = ImColor(150, 150, 150, 255);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(125, 125, 125, 255);
		colors[ImGuiCol_ScrollbarGrabActive] = ImColor(100, 100, 100, 255);

		colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255, 255);
		colors[ImGuiCol_SliderGrab] = ImColor(255, 255, 255, 255);
		colors[ImGuiCol_SliderGrabActive] = ImColor(0, 0, 0, 150);

		colors[ImGuiCol_Button] = ImColor(100, 100, 100, 255);
		colors[ImGuiCol_ButtonHovered] = ImColor(125, 125, 125, 255);
		colors[ImGuiCol_ButtonActive] = ImColor(175, 175, 175, 255);

		colors[ImGuiCol_Header] = ImColor(100, 100, 100, 225);
		colors[ImGuiCol_HeaderHovered] = ImColor(125, 125, 125, 255);
		colors[ImGuiCol_HeaderActive] = ImColor(150, 150, 150, 255);

		colors[ImGuiCol_Separator] = ImColor(110, 110, 110, 130);
		colors[ImGuiCol_SeparatorHovered] = ImColor(200, 200, 200, 150);
		colors[ImGuiCol_SeparatorActive] = ImColor(250, 250, 250, 255);

		colors[ImGuiCol_ResizeGrip] = ImColor(100, 100, 100, 50);
		colors[ImGuiCol_ResizeGripHovered] = ImColor(125, 125, 125, 200);
		colors[ImGuiCol_ResizeGripActive] = ImColor(125, 125, 125, 250);

		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 1.00f, 0.75f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

		//Sizes
		//Main
		style.WindowPadding = ImVec2(5, 5);
		style.FramePadding = ImVec2(6, 4);
		style.ItemSpacing = ImVec2(6, 3);
		style.ScrollbarSize = 14;
		style.GrabMinSize = 10;

		//Borders
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 0;
		style.FrameBorderSize = is3D;
		style.PopupBorderSize = 0;

		style.WindowRounding = 3;
		style.ChildRounding = 3;
		style.FrameRounding = 3;
		style.PopupRounding = 3;
		style.ScrollbarRounding = 3;
		style.GrabRounding = 3;

		style.WindowMenuButtonPosition = ImGuiDir_Right;

#ifdef IMGUI_HAS_DOCK 
		style.TabBorderSize = is3D;
		style.TabRounding = 3;

		colors[ImGuiCol_Tab] = ImColor(75, 75, 75, 255);
		colors[ImGuiCol_TabHovered] = ImColor(150, 150, 150, 255);
		colors[ImGuiCol_TabActive] = ImColor(125, 125, 125, 255);
		colors[ImGuiCol_TabUnfocused] = ImColor(100, 100, 100, 150);
		colors[ImGuiCol_TabUnfocusedActive] = ImColor(125, 125, 125, 150);
		colors[ImGuiCol_DockingPreview] = ImColor(255, 255, 255, 50);
		colors[ImGuiCol_DockingEmptyBg] = ImColor(50, 50, 50, 255);

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
#endif
	}
}