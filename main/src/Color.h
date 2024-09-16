#ifndef BIT_FIRMWARE_COLOR_H
#define BIT_FIRMWARE_COLOR_H

#include <cstdint>

typedef uint16_t Color;

struct Pixel {
	uint8_t r, g, b;

	bool operator==(const Pixel& right) const{
		return r == right.r && g == right.g && b == right.b;
	}
};

constexpr Color C_RGB(uint8_t r, uint8_t g, uint8_t b){
	return (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

constexpr Color C_HEX(uint32_t hex){
	return C_RGB(
			(hex >> 16) & 0xFF,
			(hex >> 8) & 0xFF,
			hex & 0xFF
	);
}

#endif //BIT_FIRMWARE_COLOR_H
