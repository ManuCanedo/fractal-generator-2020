#pragma once

namespace Fractal
{
	struct Zoom
	{
		Zoom(int x, int y, double scale)
			:x(x), y(y), scale(scale) {}

		int x{ 0 }, y{ 0 };
		double scale{ 0.0 };
	};

	class ZoomList
	{
	public:
		ZoomList(int width, int height);
		~ZoomList() = default;

		void Add(const Zoom& zoom);
		std::pair<double, double> doZoom(int x, int y);

	private:
		double m_xCenter{ 0 };
		double m_yCenter{ 0 };
		double m_Scale{ 1.0 };

		std::vector<Zoom> m_Zooms;
		int m_Width{ 0 };
		int m_Height{ 0 };
	};
}

