#pragma once

struct Color {
	Color() = default;

	Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha):
		red(red),
		green(green),
		blue(blue),
		alpha(alpha) {

	}

	Color(unsigned char red, unsigned char green, unsigned char blue):
		red(red),
		green(green),
		blue(blue),
		alpha(255) {

	}

	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
};