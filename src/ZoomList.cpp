// Precompiled header
#include "fpch.h"

#include "ZoomList.h"

namespace Fractal
{
	ZoomList::ZoomList(int width, int height)
		: m_Width(width), m_Height(height) {}

	void ZoomList::Add(const Zoom& zoom)
	{
		m_Zooms.push_back(zoom);

		m_xCenter += (static_cast<int64_t>(zoom.x) - m_Width / 2) * m_Scale;
		m_yCenter += (static_cast<int64_t>(zoom.y) - m_Height / 2) * m_Scale;
		m_Scale *= zoom.scale;
	}

	auto ZoomList::doZoom(int x, int y)
	{
		struct DoubleCoordinates
		{
			double d_x{ 0 }, d_y{ 0 };
		} pair;

		return pair;
	}
}