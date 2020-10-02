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

		m_xCenter += (zoom.x - m_Width / 2) * m_Scale;
		m_yCenter += (zoom.y - m_Height / 2) * m_Scale;
		m_Scale *= zoom.scale;
	}

	std::pair<double, double> ZoomList::doZoom(int x, int y)
	{
		double xFractal = (x - m_Width) * m_Scale + m_xCenter;
		double yFractal = (y - m_Height) * m_Scale + m_yCenter;

		return { xFractal, yFractal };
	}
}