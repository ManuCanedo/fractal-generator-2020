#include "fpch.h"

#include "application.h"

#include "bmp.h"
#include "tools/alloc_tracker.h"
#include "tools/cpu_features.h"

#include <GLFW/glfw3.h>

constexpr auto WIDTH = 1600;
constexpr auto HEIGHT = 900;
constexpr auto THREADS = 40;

namespace fractal
{
static std::mutex s_mutex;
Application *Application::s_instance = nullptr;

Application::Application() : threads(THREADS), futures(threads), scale({ WIDTH / 2, HEIGHT })
{
	Timer timer;
	s_instance = this;

	window = std::unique_ptr<Window>(Window::Get({ "Fractal Explorer", WIDTH, HEIGHT }));
	window->set_event_callback([this](Event &event) { on_event(event); });
	fractal = std::make_unique<uint8_t[]>(3 * static_cast<int64_t>(window->width()) *
					      window->height());
	is_supported_avx2 = check_vec_ext_support();
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

	const Point2D pix_tl = { 0.0, 0.0 };
	const Point2D pix_br = { static_cast<double>(window->width()),
				 static_cast<double>(window->height()) };
	Point2D frac_tl = { -2.0, -1.0 };
	Point2D frac_br = { 1.0, 1.0 };

	scr_to_world(pix_tl, frac_tl);
	scr_to_world(pix_br, frac_br);

	const double scr_section_w = (pix_br.x - pix_tl.x) / threads;
	const double frac_section_w = (frac_br.x - frac_tl.x) / threads;
	int iters = iterations;

	if (is_screenshot)
		save();

	// RENDER FRAME
	if (!is_fframe) {
		std::for_each(futures.begin(), futures.end(), [](auto& fut) { fut.wait(); });
		glDrawPixels(window->width(), window->height(), GL_RGB, GL_UNSIGNED_BYTE,
			     fractal.get());
	} else {
		is_fframe = false;
	}

	if (is_avx2 && is_supported_avx2) {
		for (size_t i = 0; i < futures.size(); ++i) {
			futures[i] = std::async(
				std::launch::async, &Application::calc_section_avx, this,
				fractal.get(), window->width(), iters,
				Point2D(pix_tl.x + (scr_section_w * i), pix_tl.y),
				Point2D(pix_tl.x + (scr_section_w * (i + 1)), pix_br.y),
				Point2D(frac_tl.x + (frac_section_w * i), frac_tl.y),
				Point2D(frac_tl.x + (frac_section_w * (i + 1)), frac_br.y));
		}
	} else {
		for (size_t i = 0; i < futures.size(); ++i) {
			futures[i] = std::async(
				std::launch::async, &Application::calc_section, this, fractal.get(),
				window->width(), iters,
				Point2D(pix_tl.x + (scr_section_w * i), pix_tl.y),
				Point2D(pix_tl.x + (scr_section_w * (i + 1)), pix_br.y),
				Point2D(frac_tl.x + (frac_section_w * i), frac_tl.y),
				Point2D(frac_tl.x + (frac_section_w * (i + 1)), frac_br.y));
		}
	}
	window->on_update();
}

bool Application::calc_section(uint8_t *mem, const int width, const int it,
			       const Point2D &pix_tl, const Point2D &pix_br, const Point2D &frac_tl,
			       const Point2D &frac_br)
{
	const double scale_x = (frac_br.x - frac_tl.x) / (pix_br.x - pix_tl.x);
	const double scale_y = (frac_br.y - frac_tl.y) / (pix_br.y - pix_tl.y);

	double pos_y = frac_tl.y;
	double pos_x = frac_tl.x;
	int row_sz = width;
	int offset_y = 0;

	for (int y = static_cast<int>(pix_tl.y); y < pix_br.y; ++y) {
		pos_x = frac_tl.x;
		for (int x = static_cast<int>(pix_tl.x); x < pix_br.x; ++x) {
			std::complex<double> z = { 0.0, 0.0 };
			std::complex<double> c = { pos_x, pos_y };
			int n = 0;

			while (z.real() * z.real() + z.imag() * z.imag() < 4 && n < it) {
				z = { z.real() * z.real() - z.imag() * z.imag() + c.real(),
				      2 * z.real() * z.imag() + c.imag() };
				++n;
			}

			uint8_t *pix = mem + 3 * (static_cast<int64_t>(offset_y) + x);
			uint8_t r = 0, g = 0, b = 0;

			if (n < it) {
				r = static_cast<uint8_t>(
					256 * (0.5 * sin(0.1f * n + rgb_offset.x) + 0.5));
				g = static_cast<uint8_t>(
					256 * (0.2 * sin(0.1f * n + rgb_offset.y) + 0.5));
				b = static_cast<uint8_t>(
					256 * (0.5 * sin(0.1f * n + rgb_offset.z) + 0.5));
			}
			pix[0] = r;
			pix[1] = g;
			pix[2] = b;
			pos_x += scale_x;
		}
		pos_y += scale_y;
		offset_y += row_sz;
	}
	return true;
}

bool Application::calc_section_avx(uint8_t *mem, const int width, const int it,
				   const Point2D &pix_tl, const Point2D &pix_br,
				   const Point2D &frac_tl, const Point2D &frac_br)
{
	const double scale_x = (frac_br.x - frac_tl.x) / (pix_br.x - pix_tl.x);
	const double scale_y = (frac_br.y - frac_tl.y) / (pix_br.y - pix_tl.y);

	double pos_y = frac_tl.y;
	int row_sz = width, offset_y = 0;
	int x, y;

	/* ----------     ----------     ----------     ---------- */
	/* ----------            Intrinsics             ---------- */
	/* ----------     ----------     ----------     ---------- */
	// Double Registers
	__m256d _zr, _zi, _zr2, _zi2, _cr, _ci, _auxDouble1, _auxDouble2, _maskDouble;
	__m256d _two = _mm256_set1_pd(2.0), _four = _mm256_set1_pd(4.0);
	__m256d _xScale = _mm256_set1_pd(scale_x), _xSkip = _mm256_set1_pd(scale_x * 4);
	__m256d _xPos, _xPosOffset = _mm256_mul_pd(_mm256_set_pd(0, 1, 2, 3), _xScale);

	// Int Registers
	__m256i _n, _maskInt, _auxInt;
	__m256i _one = _mm256_set1_epi64x(1), _iters = _mm256_set1_epi64x(it);

	for (y = static_cast<int>(pix_tl.y); y < pix_br.y; y++) {
		_auxDouble1 = _mm256_set1_pd(frac_tl.x);
		_xPos = _mm256_add_pd(_auxDouble1, _xPosOffset);
		_ci = _mm256_set1_pd(pos_y);

		for (x = static_cast<int>(pix_tl.x); x < pix_br.x; x += 4) {
			//std::complex<double> z{ 0.0, 0.0 };
			_zr = _mm256_setzero_pd();
			_zi = _mm256_setzero_pd();
			//std::complex<double> c{ pos_x, pos_y };
			_cr = _xPos;
			//int n{ 0 };
			_n = _mm256_setzero_si256();
			//while ()
		repeat:
			// z.real() = z.real() * z.real() - z.imag() * z.imag() + c.real()
			_zr2 = _mm256_mul_pd(_zr, _zr);
			_zi2 = _mm256_mul_pd(_zi, _zi);
			_auxDouble1 = _mm256_sub_pd(_zr2, _zi2);
			_auxDouble2 = _mm256_mul_pd(_zr, _zi);
			_zr = _mm256_add_pd(_auxDouble1, _cr);
			_zi = _mm256_fmadd_pd(_auxDouble2, _two, _ci);
			//  z.real() * z.real() + z.imag() * z.imag() < 4
			_auxDouble1 = _mm256_add_pd(_zr2, _zi2);
			_maskDouble = _mm256_cmp_pd(_auxDouble1, _four, _CMP_LT_OQ);
			// it > n
			_maskInt = _mm256_cmpgt_epi64(_iters, _n);
			// (z.real() * z.real() + z.imag() * z.imag() < 4 && n < it)
			_maskInt = _mm256_and_si256(_maskInt, _mm256_castpd_si256(_maskDouble));
			// ++n
			_auxInt = _mm256_and_si256(_one, _maskInt);
			_n = _mm256_add_epi64(_n, _auxInt);

			// if cond true true, keeps iterating
			if (_mm256_movemask_pd(_mm256_castsi256_pd(_maskInt)) > 0)
				goto repeat;

			_xPos = _mm256_add_pd(_xPos, _xSkip);

			/* ----------     ----------     ----------     ---------- */
			/* ----------        End of Intrinsics          ---------- */
			/* ----------     ----------     ----------     ---------- */
			uint8_t *pix = &mem[3 * (static_cast<int64_t>(offset_y) + x)];

			for (int i = 3; i >= 0; --i) {
				// reg _n contains [pix3 n][pix2 n][pix1 n][pix0 n]
				const int n = static_cast<int>(int(_n.m256i_i64[i]));
				uint8_t r = 0, g = 0, b = 0;

				if (n < iterations) {
					r = static_cast<uint8_t>(
						256 * (0.5 * sin(0.1f * n + rgb_offset.x) + 0.5));
					g = static_cast<uint8_t>(
						256 * (0.2 * sin(0.1f * n + rgb_offset.y) + 0.5));
					b = static_cast<uint8_t>(
						256 * (0.5 * sin(0.1f * n + rgb_offset.z) + 0.5));
				}
				pix[0] = r;
				pix[1] = g;
				pix[2] = b;
				pix += 3;
			}
		}
		pos_y += scale_y;
		offset_y += row_sz;
	}
	return true;
}

void Application::save()
{
	static uint8_t index = 0;
	int width = window->width(), height = window->height();

	Timer timer;
	Bmp image(width, height);
	{
		std::lock_guard<std::mutex> lock(s_mutex);
		for (size_t y = 0; y < height; ++y)
			for (size_t x = 0; x < width; ++x) {
				uint8_t *pPixel =
					&fractal[3 * (y * static_cast<int64_t>(width) + x)];
				image.set_pixel(x, y, pPixel[0], pPixel[1], pPixel[2]);
			}
	}

	std::string fileName("Fractal_Screenshot_");
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
	auto app = std::make_unique<fractal::Application>();
	LOG_TRACE("Memory: {0}KB", s_alloc_tracker.curr_usage());

	fractal::Timer timer;
	app->run();

	return 0;
}
