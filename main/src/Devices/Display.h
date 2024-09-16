#ifndef BIT_FIRMWARE_DISPLAY_H
#define BIT_FIRMWARE_DISPLAY_H

#include <LovyanGFX.h>
#include "FS/File.h"

typedef lgfx::LGFX_Sprite Sprite;

class Display {
public:
	Display();
	virtual ~Display();

	LGFX_Device& getLGFX();

	Sprite& getCanvas();

	void drawTest();


	/**
	 * @brief Draw a raw binary RGB565 file icon on a sprite at the specified position and size.
	 *
	 * @param sprite The sprite on which the file icon will be drawn.
	 * @param icon The file icon to be drawn.
	 * @param x The x-coordinate of the top-left corner of the file icon.
	 * @param y The y-coordinate of the top-left corner of the file icon.
	 * @param width The width of the file icon.
	 * @param height The height of the file icon.
	 * @param scale The scaling factor of the file icon (default: 1).
	 * @param maskingColor The color used to mask the file icon (default: TFT_TRANSPARENT).
	 */
	static void drawFile(Sprite& sprite, File icon, int16_t x, int16_t y, uint16_t width, uint16_t height,
						 uint8_t scale = 1, int32_t maskingColor = TFT_TRANSPARENT);
private:
	lgfx::Bus_SPI bus;
	lgfx::Panel_ST7735S panel;
	LGFX_Device lgfx;
	Sprite canvas;

	void setupBus();
	void setupPanel();

};


#endif //BIT_FIRMWARE_DISPLAY_H
