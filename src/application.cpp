#include "fpch.h"

#include "application.h"

#include "bmp.h"
#include "tools/alloc_tracker.h"
#include "tools/cpu_features.h"

#include <GLFW/glfw3.h>

namespace fractal
{
Application::Application(unsigned int width, unsigned int height, unsigned int sec)
	: sections(sec), mandelbrot(width, height, sec), scale(width / 4, height / 2),
	  offset(-2.5, -1)
{
	Timer timer;
	window = std::unique_ptr<Window>(Window::Get({ "Fractal Explorer", width, height }));
	window->set_event_callback([this](Event& event) { on_event(event); });
}

void Application::run()
{
	while (is_running)
		update();
}

void Application::update()
{
	// UPDATE WORLD
	set_world_scale();

	constexpr Point2D pix_tl = { 0.0, 0.0 };
	Point2D pix_br(window->width(), window->height());
	const Point2D frac_tl = scr_to_world(pix_tl);
	const Point2D frac_br = scr_to_world(pix_br);

	if (is_screenshot)
		save();

	// GENERATE FRAME
	if (const char* frame = mandelbrot.get_frame(); frame) {
		mandelbrot.wait();
		glDrawPixels(window->width(), window->height(), GL_RGB, GL_UNSIGNED_BYTE, frame);
	}
	mandelbrot.generate_frame((is_cuda) ? Fractal::Render::CUDA :
				  (is_avx2) ? Fractal::Render::AVX :
						    Fractal::Render::BASIC,
				  iterations, rgb_offset, pix_tl, pix_br, frac_tl, frac_br);
	window->on_update();
}

void Application::save()
{
	static char index = 0;
	int width = window->width();
	int height = window->height();
	Bmp image(width, height);

	mandelbrot.wait();
	for (size_t y = 0; y < height; ++y) {
		for (size_t x = 0; x < width; ++x) {
			const char* pPixel = &mandelbrot.get_frame()[3 * (y * width + x)];
			image.set_pixel(x, y, pPixel[0], pPixel[1], pPixel[2]);
		}
	}

	std::string fileName = "Fractal_Screenshot_";
	fileName.append(std::to_string(index++) + ".bmp");
	image.write(fileName);

	LOG_INFO("Screenshot saved as {0}", fileName);
	is_screenshot = false;
}

void Application::on_event(Event& event)
{
	EventDispatcher dispatcher(event);

	dispatcher.dispatch<WindowCloseEvent>([this](const WindowCloseEvent& event) {
		is_running = false;
		return true;
	});

	dispatcher.dispatch<MouseMovedEvent>([this](const MouseMovedEvent& event) {
		mouse_coords = { event.get_x(), event.get_y() };
		if (is_panning) {
			offset -= Point2D::inv_y((mouse_coords - start_pan) / scale);
			start_pan = mouse_coords;
		}
		return true;
	});

	dispatcher.dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent& event) {
		if (event.get_button() == MouseButtonCode::WHEEL) {
			start_pan = mouse_coords;
			is_panning = true;
		}
		return is_panning;
	});

	dispatcher.dispatch<MouseButtonReleasedEvent>(
		[this](const MouseButtonReleasedEvent& event) {
			if (event.get_button() == MouseButtonCode::WHEEL)
				is_panning = false;
			return true;
		});

	dispatcher.dispatch<MouseScrolledEvent>([this](const MouseScrolledEvent& event) {
		(event.get_offset_y() > 0) ? scaling_factor += 0.1f : scaling_factor -= 0.1f;
		return true;
	});

	dispatcher.dispatch<KeyPressedEvent>([this](const KeyPressedEvent& event) {
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

	dispatcher.dispatch<KeyHeldEvent>([this](const KeyHeldEvent& event) {
		switch (event.get_keycode()) {
		case KeyCode::UP:
		case KeyCode::DOWN:
			(event.get_keycode() == KeyCode::UP) ? scaling_factor += 0.1f :
								     scaling_factor -= 0.2f;
			break;
		}
		return true;
	});
}

inline void Application::set_world_scale()
{
	Point2D mouse_before_zoom = scr_to_world(mouse_coords);
	scale *= scaling_factor;
	offset += (mouse_before_zoom - scr_to_world(mouse_coords));
	scaling_factor = 1.0f;
}

constexpr Point2D Application::scr_to_world(const Point2D& n) const
{
	return { n.x / scale.x + offset.x, n.y / scale.y + offset.y };
}
} // namespace fractal

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
