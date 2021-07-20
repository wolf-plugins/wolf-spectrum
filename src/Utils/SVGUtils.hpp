#ifndef NANOSVG_UTILS
#define NANOSVG_UTILS

#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdint>
#include <cassert>
#include "nanosvg.h"
#include "nanovg.h"

START_NAMESPACE_DISTRHO

namespace SVGUtils
{

//based on VCVRack's code: https://github.com/VCVRack/Rack/blob/bd4f68c13d0bb8577aa1fb35c25b89da63f062af/include/util/math.hpp
struct Vec {
	float x = 0.f;
	float y = 0.f;

	Vec() {}
	Vec(float x, float y) : x(x), y(y) {}

	Vec neg() {
		return Vec(-x, -y);
	}
	Vec plus(Vec b) {
		return Vec(x + b.x, y + b.y);
	}
	Vec minus(Vec b) {
		return Vec(x - b.x, y - b.y);
	}
	Vec mult(float s) {
		return Vec(x * s, y * s);
	}
	Vec mult(Vec b) {
		return Vec(x * b.x, y * b.y);
	}
	Vec div(float s) {
		return Vec(x / s, y / s);
	}
	Vec div(Vec b) {
		return Vec(x / b.x, y / b.y);
	}
	float dot(Vec b) {
		return x * b.x + y * b.y;
	}
	float norm() {
		return hypotf(x, y);
	}
	Vec flip() {
		return Vec(y, x);
	}
	Vec round() {
		return Vec(roundf(x), roundf(y));
	}
	Vec floor() {
		return Vec(floorf(x), floorf(y));
	}
	Vec ceil() {
		return Vec(ceilf(x), ceilf(y));
	}
	bool isEqual(Vec b) {
		return x == b.x && y == b.y;
	}
	bool isZero() {
		return x == 0.0f && y == 0.0f;
	}
};

NVGcolor getNVGColor(uint32_t color);
float getLineCrossing(Vec p0, Vec p1, Vec p2, Vec p3);
NVGpaint getPaint(NVGcontext *vg, NSVGpaint *p);
void nvgDrawSVG(NVGcontext *vg, NSVGimage *svg);

};

END_NAMESPACE_DISTRHO

#endif