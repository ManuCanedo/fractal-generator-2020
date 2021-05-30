#include "fpch.h"

#include "fractal.h"

namespace fractal
{
Fractal::Fractal(const int width, const int height, const int threads)
	: width(width), futures(threads),
	  frame(std::make_unique<char[]>(3 * static_cast<int64_t>(width) * height))
{
}

Fractal::~Fractal()
{
	Fractal::wait();
}

void Fractal::generate_frame(Render type, const int iters, const Colour &rgb, const Point2D &pix_tl,
			     const Point2D &pix_br, const Point2D &frac_tl, const Point2D &frac_br)
{
	const double scr_section_w = (pix_br.x - pix_tl.x) / futures.size();
	const double frac_section_w = (frac_br.x - frac_tl.x) / futures.size();

	for (size_t i = 0; i < futures.size(); ++i) {
		const Point2D p_tl(pix_tl.x + (scr_section_w * i), pix_tl.y);
		const Point2D p_br(pix_tl.x + (scr_section_w * (i + 1)), pix_br.y);
		const Point2D f_tl(frac_tl.x + (frac_section_w * i), frac_tl.y);
		const Point2D f_br(frac_tl.x + (frac_section_w * (i + 1)), frac_br.y);

		switch (type) {
		case Render::BASIC:
			futures[i] =
				std::async(std::launch::async, &Fractal::calc_section, this,
					   frame.get(), width, iters, rgb, p_tl, p_br, f_tl, f_br);
			break;
		case Render::AVX:
			futures[i] = std::async(std::launch::async, &Fractal::calc_section_avx,
						this, frame.get(), width, iters, rgb, p_tl, p_br, f_tl, f_br);
			break;
		}
	}
}

void Fractal::wait()
{
	std::for_each(futures.begin(), futures.end(), [](auto &fut) { fut.wait(); });
}

const char *Fractal::get_frame() const
{
	if (futures.front().valid())
		return frame.get();
	return nullptr;
}

bool Fractal::calc_section(char *data, const int width, const int iters, const Colour rgb,
			   const Point2D pix_tl, const Point2D pix_br, const Point2D frac_tl,
			   const Point2D frac_br)
{
	const double scale_x = (frac_br.x - frac_tl.x) / (pix_br.x - pix_tl.x);
	const double scale_y = (frac_br.y - frac_tl.y) / (pix_br.y - pix_tl.y);

	double pos_y = frac_tl.y;
	int offset_y = 0;

	for (int y = static_cast<int>(pix_tl.y); y < pix_br.y; ++y) {
		double pos_x = frac_tl.x;
		for (int x = static_cast<int>(pix_tl.x); x < pix_br.x; ++x) {
			std::complex<double> z = { 0.0, 0.0 };
			std::complex<double> c = { pos_x, pos_y };
			int n = 0;

			while (z.real() * z.real() + z.imag() * z.imag() < 4 && n < iters) {
				z = { z.real() * z.real() - z.imag() * z.imag() + c.real(),
				      2 * z.real() * z.imag() + c.imag() };
				++n;
			}

			char *pix = &data[3 * (offset_y + x)];
			char r = 0, g = 0, b = 0;

			if (n < iters) {
				r = static_cast<char>(256 * (0.5 * sin(0.1f * n + rgb.r) + 0.5));
				g = static_cast<char>(256 * (0.2 * sin(0.1f * n + rgb.g) + 0.5));
				b = static_cast<char>(256 * (0.5 * sin(0.1f * n + rgb.b) + 0.5));
			}
			pix[0] = r;
			pix[1] = g;
			pix[2] = b;
			pos_x += scale_x;
		}
		pos_y += scale_y;
		offset_y += width;
	}

	return true;
}

bool Fractal::calc_section_avx(char *data, const int width, const int iters, Colour rgb,
			       const Point2D pix_tl, const Point2D pix_br, const Point2D frac_tl,
			       const Point2D frac_br)
{
	const double scale_x = (frac_br.x - frac_tl.x) / (pix_br.x - pix_tl.x);
	const double scale_y = (frac_br.y - frac_tl.y) / (pix_br.y - pix_tl.y);

	double pos_y = frac_tl.y;
	int offset_y = 0;
	int x, y;

	/* ----------     ----------     ----------     ---------- */
	/* ----------            Intrinsics             ---------- */
	/* ----------     ----------     ----------     ---------- */
	// Double Registers
	__m256d _zr, _zi, _zr2, _zi2, _cr, _ci, _auxDouble1, _auxDouble2, _maskDouble;
	__m256d _two = _mm256_set1_pd(2.0), _four = _mm256_set1_pd(4.0);
	__m256d _xScale = _mm256_set1_pd(scale_x), _xSkip = _mm256_set1_pd(scale_x * 4);
	__m256d _xPos, _xPosOffset = _mm256_mul_pd(_mm256_set_pd(0, 1, 2, 3), _xScale);

	// Integer Registers
	__m256i _n, _maskInt, _auxInt;
	__m256i _one = _mm256_set1_epi64x(1), _iters = _mm256_set1_epi64x(iters);

	for (y = static_cast<int>(pix_tl.y); y < pix_br.y; ++y) {
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
			// iters > n
			_maskInt = _mm256_cmpgt_epi64(_iters, _n);
			// (z.real() * z.real() + z.imag() * z.imag() < 4 && n < iters)
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
			char *pix = &data[3 * (offset_y + x)];

			for (int i = 3; i >= 0; --i) {
				// reg _n contains [pix3 n][pix2 n][pix1 n][pix0 n]
				const int n = static_cast<int>(int(_n.m256i_i64[i]));
				char r = 0, g = 0, b = 0;

				if (n < iters) {
					r = static_cast<char>(256 *
							      (0.5 * sin(0.1f * n + rgb.r) + 0.5));
					g = static_cast<char>(256 *
							      (0.2 * sin(0.1f * n + rgb.g) + 0.5));
					b = static_cast<char>(256 *
							      (0.5 * sin(0.1f * n + rgb.b) + 0.5));
				}
				pix[0] = r;
				pix[1] = g;
				pix[2] = b;
				pix += 3;
			}
		}
		pos_y += scale_y;
		offset_y += width;
	}

	return true;
}
} // namespace fractal