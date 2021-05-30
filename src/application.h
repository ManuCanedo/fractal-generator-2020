#ifndef APPLICATION_H
#define APPLICATION_H

#include "window.h"
#include "fractal.h"
#include "ui/imgui_layer.h"

namespace fractal
{
class Application {
public:
	Application(unsigned int width = 1920, unsigned int height = 1080, unsigned int sec = 32);

	void run();
	void update();
	void on_event(Event& event);

	void set_roffset(const float val)
	{
		rgb_offset.r = val;
	}

	void set_goffset(const float val)
	{
		rgb_offset.g = val;
	}

	void set_boffset(const float val)
	{
		rgb_offset.b = val;
	}

	void set_avx(const bool val)
	{
		is_avx2 = val;
	}

	void set_save(const bool val)
	{
		is_screenshot = val;
	}

	Window& get_window() const
	{
		return *window;
	}

	int get_iters() const
	{
		return iterations;
	}

	static Application& Get()
	{
		static Application instance;
		return instance;
	}

private:
	void save();
	void set_world_scale();
	constexpr Point2D scr_to_world(const Point2D& n) const;

private:
	const int sections;
	Fractal mandelbrot;
	Point2D scale, offset;
	std::unique_ptr<Window> window;

	int iterations{ 258 };
	Colour rgb_offset{ 0.0f, 2.094f, 4.188f };
	Point2D start_pan{ 0.0, 0.0 }, mouse_coords{ 0.0, 0.0 };
	float scaling_factor{ 1.0f };

	bool is_cuda{ false }, is_avx2{ true }, is_running{ true };
	bool is_screenshot{ false }, is_panning{ false };
};
} // namespace fractal

#endif // APPLICATION_H
