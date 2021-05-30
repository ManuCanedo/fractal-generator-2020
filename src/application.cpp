#include "fpch.h"

#include "application.h"

#include "bmp.h"
#include "tools/alloc_tracker.h"
#include "tools/cpu_features.h"

#include <GLFW/glfw3.h>

constexpr auto WIDTH = 1600;
constexpr auto HEIGHT = 900;
constexpr auto THREADS = 32;

namespace fractal
{
static std::mutex s_mutex;

Application::Application()
	: threads(THREADS), mandelbrot(WIDTH, HEIGHT, THREADS), scale({ WIDTH / 2, HEIGHT })
{
	Timer timer;
	window = std::unique_ptr<Window>(Window::Get({ "Fractal Explorer", WIDTH, HEIGHT }));
	window->set_event_callback([this](Event &event) { on_event(event); });
}

void Application::run()
{
	while (is_running)
		update();
}

void Application::update()
{
	// UPDATE WORLD
	set_world_scale(scaling_factor);

	constexpr Point2D pix_tl = { 0.0, 0.0 };
	const Point2D pix_br = { static_cast<double>(window->width()),
				 static_cast<double>(window->height()) };
	Point2D frac_tl = { -2.0, -1.0 };
	Point2D frac_br = { 1.0, 1.0 };

	scr_to_world(pix_tl, frac_tl);
	scr_to_world(pix_br, frac_br);

	if (is_screenshot)
		save();

	// GENERATE FRAME
	if (const char *frame = mandelbrot.get_frame(); frame) {
		mandelbrot.wait();
		glDrawPixels(window->width(), window->height(), GL_RGB, GL_UNSIGNED_BYTE, frame);
	}
	mandelbrot.generate_frame((is_avx2) ? Fractal::Render::AVX : Fractal::Render::BASIC,
				  iterations, rgb_offset, pix_tl, pix_br, frac_tl, frac_br);
	window->on_update();
}

void Application::save()
{
	static char index = 0;
	int width = window->width(), height = window->height();
	Bmp image(width, height);

	mandelbrot.wait();
	for (size_t y = 0; y < height; ++y) {
		for (size_t x = 0; x < width; ++x) {
			const char *pPixel =
				&mandelbrot.get_frame()[3 * (y * static_cast<int64_t>(width) + x)];
			image.set_pixel(x, y, pPixel[0], pPixel[1], pPixel[2]);
		}
	}

	std::string fileName = "Fractal_Screenshot_";
	fileName.append(std::to_string(index++));
	fileName.append(".bmp");
	image.write(fileName);

	LOG_INFO("Screenshot saved as {0}", fileName);
	is_screenshot = false;
}

void Application::on_event(Event &event)
{
	EventDispatcher dispatcher(event);

	dispatcher.dispatch<WindowCloseEvent>([this](const WindowCloseEvent &event) {
		is_running = false;
		return true;
	});

	dispatcher.dispatch<MouseMovedEvent>([this](const MouseMovedEvent &event) {
		mouse_coords = { event.get_x(), event.get_y() };
		if (is_panning) {
			offset -= Point2D::inv_y((mouse_coords - start_pan) / scale);
			start_pan = mouse_coords;
		}
		return true;
	});

	dispatcher.dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent &event) {
		if (event.get_button() == MouseButtonCode::WHEEL) {
			start_pan = mouse_coords;
			is_panning = true;
		}
		return is_panning;
	});

	dispatcher.dispatch<MouseButtonReleasedEvent>(
		[this](const MouseButtonReleasedEvent &event) {
			if (event.get_button() == MouseButtonCode::WHEEL)
				is_panning = false;
			return true;
		});

	dispatcher.dispatch<MouseScrolledEvent>([this](const MouseScrolledEvent &event) {
		(event.get_offset_y() > 0) ? scaling_factor = 1.1f : scaling_factor = 0.9f;
		return true;
	});

	dispatcher.dispatch<KeyPressedEvent>([this](const KeyPressedEvent &event) {
		switch (event.get_keycode()) {
		case KeyCode::LEFT: {
			if (iterations > 64)
				iterations -= 64;
			break;
		}
		case KeyCode::RIGHT: {
			iterations += 64;
			break;
		}
		case KeyCode::ESCAPE: {
			is_running = false;
			break;
		}
		case KeyCode::SPACE: {
			is_screenshot = true;
			break;
		}
		default:
			break;
		}
		return true;
	});

	dispatcher.dispatch<KeyHeldEvent>([this](const KeyHeldEvent &event) {
		switch (event.get_keycode()) {
		case KeyCode::UP:
		case KeyCode::DOWN:
			(event.get_keycode() == KeyCode::UP) ? scaling_factor = 1.1f :
								     scaling_factor = 0.9f;
			break;
		}
		return true;
	});
}

inline void Application::set_world_scale(const double scalingFactor)
{
	Point2D mouseCoordsBeforeZoom{ 0.0f, 0.0f }, mouseCoordsAfterZoom{ 0.0f, 0.0f };
	scr_to_world(mouse_coords, mouseCoordsBeforeZoom);
	scale *= scalingFactor;
	scr_to_world(mouse_coords, mouseCoordsAfterZoom);

	offset += mouseCoordsBeforeZoom - mouseCoordsAfterZoom;
	scaling_factor = 1.0f;
}

inline void Application::scr_to_world(const Point2D &n, Point2D &v) const
{
	v.x = n.x / scale.x + offset.x;
	v.y = n.y / scale.y + offset.y;
}
}

int main()
{
	fractal::Logger::Init();
	LOG_TRACE("System logger initialized");

	LOG_TRACE("Application launched");
	auto& app = fractal::Application::Get();
	LOG_TRACE("Memory: {0}KB", s_alloc_tracker.curr_usage());

	fractal::Timer timer;
	app.run();

	return 0;
}
