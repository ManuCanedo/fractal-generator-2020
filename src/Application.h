#ifndef APPLICATION_H
#define APPLICATION_H

#include "window.h"
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
		rgb_offset.x.exchange(val);
	}

	void set_goffset(const float val)
	{
		rgb_offset.y.exchange(val);
	}

	void set_boffset(const float val)
	{
		rgb_offset.z.exchange(val);
	}

	void set_avx(const bool val)
	{
		is_avx2 = val;
	}

	void set_complex_visualizer(const bool val)
	{
		is_complex_visualizer = val;
	}

	void set_save(const bool val)
	{
		is_screenshot = val;
	}

	Window &get_window() const
	{
		return *window;
	}

	constexpr int get_iters() const
	{
		return iterations;
	}

	static Application &Get()
	{
		return *s_instance;
	}

private:
	bool calc_section(uint8_t *mem, const int width, const int iters,
			  const Point2D &pix_tl, const Point2D &pix_br, const Point2D &frac_tl,
			  const Point2D &frac_br);
	bool calc_section_avx(uint8_t *mem, int width, int iters,
			      const Point2D &pix_tl, const Point2D &pix_br, const Point2D &frac_tl,
			      const Point2D &frac_br);
	void save();
	void set_world_scale(const double scale);
	void scr_to_world(const Point2D &n, Point2D &v) const;

private:
	static Application *s_instance;

	std::unique_ptr<uint8_t[]> fractal;
	std::unique_ptr<Window> window;
	const int threads;
	std::vector<std::future<bool> > futures;

	// Fractal Generation
	int iterations{ 258 };
	Point2D offset{ 0.0, 0.0 }, start_pan{ 0.0, 0.0 }, mouse_coords{ 0.0, 0.0 },
		scale{ 1.0, 1.0 };
		
	// Fractal Colouring
	struct Vec3f {
		std::atomic<float> x, y, z;
	} rgb_offset{ 0.0f, 0.0f, 0.0f };

	// Update Flags
	bool is_avx2{ false }, is_complex_visualizer{ false }, is_screenshot{ false };
	bool is_supported_avx2{ false }, is_running{ true }, is_panning{ false }, is_fframe{ true };
	double scaling_factor{ 1.0f };
};
}

#endif // APPLICATION_H