#ifndef FRACTAL_H
#define FRACTAL_H

#include <vector>
#include <future>

#include "window.h"

namespace fractal
{
struct Colour {
	float r{ 0.0f }, g{ 0.0f }, b{ 0.0f };
};

class Fractal {
public:
	enum class Render { BASIC, AVX };

	Fractal(const int threads, const int width, const int length);
	~Fractal();

	void generate_frame(Render type, const int iters, const Colour &rgb, const Point2D &pix_tl,
			    const Point2D &pix_br, const Point2D &frac_tl, const Point2D &frac_br);
	void wait();
	const char *get_frame() const;

private:
	bool calc_section(char *mem, const int width, const int iters, const Colour rgb,
			  const Point2D pix_tl, const Point2D pix_br, const Point2D frac_tl,
			  const Point2D frac_br);
	bool calc_section_avx(char *mem, const int width, const int iters, const Colour rgb,
			      const Point2D pix_tl, const Point2D pix_br, const Point2D frac_tl,
			      const Point2D frac_br);

private:
	const int width;
	std::vector<std::future<bool> > futures;
	std::unique_ptr<char[]> frame;
};
} // namespace fractal

#endif // FRACTAL_H