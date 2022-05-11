#pragma once
#include <string>
#include <sstream>
#include <stdint.h>
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#include "singleton.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"
#include "valve_sdk/misc/Color.hpp"

extern ImFont* g_pDefaultFont;
extern ImFont* g_pSecondFont;


class Vector;

class Render
	: public Singleton<Render>
{
	friend class Singleton<Render>;

private:
	ImDrawList * draw_list_act;
	ImDrawList * draw_list_rendering;
	ImDrawList* draw_list;
	ImDrawData draw_data;

	ImU32 GetU32(Color _color)
	{
		return ((_color[3] & 0xff) << 24) + ((_color[2] & 0xff) << 16) + ((_color[1] & 0xff) << 8)
			+ (_color[0] & 0xff);
	}
public:
	void Initialize();
	void GetFonts();
	ImFont* GetBigFonts();
	void ClearDrawList();
	void BeginScene(int charge, float dt);
	void Background();
	ImDrawList* RenderScene();

	float RenderText(const std::string& text, ImVec2 position, float size, ImVec4 color, bool center = false, bool outline = true);

	void RenderCircle3D(Vector position, float points, float radius, ImVec4 color);

	void RenderImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a = ImVec2(0, 0), const ImVec2& uv_b = ImVec2(1, 1), ImU32 col = 0xFFFFFFFF) {
		draw_list->AddImage(user_texture_id, a, b, uv_a, uv_b, col);
	}

	template <class T>
	inline void RenderBoxByType(T x1, T y1, T x2, T y2, ImVec4 color, float thickness = 1.f, int type = 0) {
		if (type == 0)
			RenderBox(x1, y1, x2, y2, color, thickness);
		else if (type == 1)
			RenderCoalBox(x1, y1, x2, y2, color);
		else if (type == 2)
			RenderBox(x1, y1, x2, y2, color, thickness, 8.f);
	}

	template <class T>
	inline void RenderBoxFilledByType(T x1, T y1, T x2, T y2, ImVec4 color, float thickness = 1.f, int type = 0) {
		if (type == 0 || type == 1)
			RenderBoxFilled(x1, y1, x2, y2, color, thickness);
		else if (type == 2)
			RenderBoxFilled(x1, y1, x2, y2, color, thickness, 8.f);
	}

	template <class T>
	inline void RenderCoalBox(T x1, T y1, T x2, T y2, ImVec4 color, float th = 1.f) {
		int w = x2 - x1;
		int h = y2 - y1;

		int iw = w / 4;
		int ih = h / 4;
		// top
		RenderLine(x1, y1, x1 + iw, y1, color, th);					// left
		RenderLine(x1 + w - iw, y1, x1 + w, y1, color, th);			// right
		RenderLine(x1, y1, x1, y1 + ih, color, th);					// top left
		RenderLine(x1 + w - 1, y1, x1 + w - 1, y1 + ih, color, th);	// top right
																	// bottom
		RenderLine(x1, y1 + h, x1 + iw, y1 + h, color, th);			// left
		RenderLine(x1 + w - iw, y1 + h, x1 + w, y1 + h, color, th);	// right
		RenderLine(x1, y1 + h - ih, x1, y1 + h, color, th);			// bottom left
		RenderLine(x1 + w - 1, y1 + h - ih, x1 + w - 1, y1 + h, color, th);	// bottom right
	}

	inline void RenderOutlineBox(float x1, float y1, float x2, float y2, ImVec4 color, float th = 1.f) {
		int w = x2 - x1;
		int h = y2 - y1;

		RenderLine(x1, y1 - th, x1 + w, y1 - th, color, th);
		RenderLine(x1, y1 + th, x1 + w, y1 + th, color, th);
		RenderLine(x1, y1 - th + h, x1 + w, y1 - th + h, color, th);
		RenderLine(x1, y1 + th + h, x1 + w, y1 + th + h, color, th);
		RenderLine(x1 - th, y1, x1 - th, y1 + h, color, th);
		RenderLine(x1 + th, y1, x1 + th, y1 + h, color, th);
		RenderLine(x1 - th + w, y1, x1 - th + w, y1 + h, color, th);
		RenderLine(x1 + th + w, y1, x1 + th + w, y1 + h, color, th);
	}
	template <class T>
	inline void RenderBox(T x1, T y1, T x2, T y2, ImVec4 color, float thickness = 1.f, float rounding = 0.f) {
		draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(color), rounding, 15, thickness);
	}
	inline void RenderBox(RECT r, ImVec4 color,float thickness = 1.f, float rounding = 0.f) {
		RenderBox(r.left, r.top, r.right, r.bottom, color, thickness, rounding);
	}
	template <class T>
	inline void RenderBoxFilled(T x1, T y1, T x2, T y2, ImVec4 color, float thickness = 1.f, float rounding = 0.f) {
		draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(color), rounding, 15);
	}
	inline void RenderBoxFilled(RECT r, ImVec4 color, float thickness = 1.f, float rounding = 0.f) {
		RenderBoxFilled(r.left, r.top, r.right, r.bottom, color, thickness, rounding);
	}
	template <class T>
	inline void RenderLine(T x1, T y1, T x2, T y2, ImVec4 color, float thickness = 1.f) {
		draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(color), thickness);
	}
	template <class T>
	inline float RenderText(const std::string& text, T x, T y, float size, ImVec4 clr, bool center = false, bool outline = true) {
		return RenderText(text, ImVec2(x, y), size, clr, center, outline);
	}
	template <class T>
	inline void RenderCircle(T x, T y, float radius, int points, ImVec4 color, float thickness = 1.f) {
		draw_list->AddCircle(ImVec2(x, y), radius, GetU32(color), points, thickness);
	}
	template <class T>
	inline void RenderCircleFilled(T x, T y, float radius, int points, ImVec4 color) {
		draw_list->AddCircleFilled(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(color), points);
	}
	inline void Render3dBox(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2 p4, ImVec2 p5, ImVec2 p6, ImVec2 p7, ImVec2 p8, ImVec4 color, float thickness = 1.f) {
		draw_list->AddLine(p1, p2, ImGui::ColorConvertFloat4ToU32(color), thickness);
		draw_list->AddLine(p1, p5, ImGui::ColorConvertFloat4ToU32(color), thickness);
		draw_list->AddLine(p5, p6, ImGui::ColorConvertFloat4ToU32(color), thickness);
		draw_list->AddLine(p2, p6, ImGui::ColorConvertFloat4ToU32(color), thickness);
		draw_list->AddLine(p2, p3, ImGui::ColorConvertFloat4ToU32(color), thickness);
		draw_list->AddLine(p6, p7, ImGui::ColorConvertFloat4ToU32(color), thickness);
		draw_list->AddLine(p3, p7, ImGui::ColorConvertFloat4ToU32(color), thickness);
		draw_list->AddLine(p3, p4, ImGui::ColorConvertFloat4ToU32(color), thickness);
		draw_list->AddLine(p7, p8, ImGui::ColorConvertFloat4ToU32(color), thickness);
		draw_list->AddLine(p4, p8, ImGui::ColorConvertFloat4ToU32(color), thickness);
		draw_list->AddLine(p4, p1, ImGui::ColorConvertFloat4ToU32(color), thickness);
		draw_list->AddLine(p8, p5, ImGui::ColorConvertFloat4ToU32(color), thickness);
	}
	inline void Render3dFilledBox(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2 p4, ImVec2 p5, ImVec2 p6, ImVec2 p7, ImVec2 p8, ImVec4 color) {
		ImVec2 points1[4] = { p1,p2,p3,p4 };
		ImVec2 points2[4] = { p5,p6,p7,p8 };
		ImVec2 points3[4] = { p1,p2,p6,p5 };
		ImVec2 points4[4] = { p1,p5,p8,p4 };
		ImVec2 points5[4] = { p6,p2,p3,p7 };
		ImVec2 points6[4] = { p7,p8,p4,p3 };
		draw_list->AddConvexPolyFilled(points1, 4, ImGui::ColorConvertFloat4ToU32(color));
		draw_list->AddConvexPolyFilled(points2, 4, ImGui::ColorConvertFloat4ToU32(color));
		draw_list->AddConvexPolyFilled(points3, 4, ImGui::ColorConvertFloat4ToU32(color));
		draw_list->AddConvexPolyFilled(points4, 4, ImGui::ColorConvertFloat4ToU32(color));
		draw_list->AddConvexPolyFilled(points5, 4, ImGui::ColorConvertFloat4ToU32(color));
		draw_list->AddConvexPolyFilled(points6, 4, ImGui::ColorConvertFloat4ToU32(color));
	}
};


