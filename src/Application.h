#ifndef APPLICATION_H
#define APPLICATION_H

#include "window.h"
#include "fractal.h"
#include "ui/imgui_layer.h"

namespace fractal
{
class Application {
public:
	Application();

	void run();
	void update();
	void on_event(Event &event);

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

	Window &get_window() const
	{
		return *window;
	}

	int get_iters() const
	{
		return iterations;
	}

	static Application &Get()
	{
		static Application instance;
		return instance;
	}

private:
	void save();
	void set_world_scale(const double scale);
	void scr_to_world(const Point2D &n, Point2D &v) const;

private:
	const int threads;
	Fractal mandelbrot;
	std::unique_ptr<Window> window;

	// Fractal Generation
	int iterations{ 258 };
	Colour rgb_offset{ 0.0f, 0.0f, 0.0f };
	Point2D offset{ 0.0, 0.0 }, start_pan{ 0.0, 0.0 };
	Point2D mouse_coords{ 0.0, 0.0 }, scale{ 1.0, 1.0 };
	double scaling_factor{ 1.0f };

	// Update Flags
	bool is_avx2{ false },  is_supported_avx2{ false }, is_screenshot{ false };
	bool is_running{ true }, is_panning{ false }, is_fframe{ true };

	static Application *s_instance;
};
} // namespace fractal

#endif // APPLICATION_H