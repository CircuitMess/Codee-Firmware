#include "Display.h"
#include <Pins.hpp>
#include "Color.h"
#include "Util/EfuseMeta.h"

Display::Display(uint8_t revision) : canvas(&lgfx), revision(revision){
	setupBus();
	if(revision == 1){
		setupPanel2();
	}else{
		setupPanel1();
	}

	// LGFX init -> panel init -> bus init
	panel.setBus(&bus);
	lgfx.setPanel(&panel);
	lgfx.init();
	lgfx.setSwapBytes(true);

	canvas.setColorDepth(lgfx::rgb565_2Byte);
	canvas.createSprite(128, 128);
}

Display::~Display(){
	bus.release();
}

void Display::setupBus(){
	lgfx::Bus_SPI::config_t cfg = {
		.freq_write = 40000000,
		.freq_read = 40000000,
		.pin_sclk = TFT_SCK,
		.pin_miso = -1,
		.pin_mosi = TFT_MOSI,
		.pin_dc = TFT_DC,
		.spi_mode = 0,
		.spi_3wire = false,
		.use_lock = false,
		.dma_channel = LGFX_ESP32_SPI_DMA_CH,
		.spi_host = SPI2_HOST
	};
	bus.config(cfg);
}

void Display::setupPanel1(){
	lgfx::Panel_Device::config_t cfg = {
			.pin_cs = -1,
			.pin_rst = TFT_RST,
			.pin_busy = -1,
			.memory_width = 132,
			.memory_height = 132,
			.panel_width = 128,
			.panel_height = 128,
			.offset_x = 2,
			.offset_y = 1,
			.offset_rotation = 3,
			.readable = false,
			.invert = false,
			.rgb_order = false,
			.dlen_16bit = false,
			.bus_shared = false
	};
	panel.config(cfg);
}

void Display::setupPanel2(){
	lgfx::Panel_Device::config_t cfg = {
			.pin_cs = -1,
			.pin_rst = TFT_RST,
			.pin_busy = -1,
			.memory_width = 132,
			.memory_height = 132,
			.panel_width = 128,
			.panel_height = 128,
			.offset_x = 2,
			.offset_y = 1,
			.offset_rotation = 1,
			.readable = false,
			.invert = false,
			.rgb_order = false,
			.dlen_16bit = false,
			.bus_shared = false
	};
	panel.config(cfg);
}

LGFX_Device& Display::getLGFX(){
	return lgfx;
}

Sprite& Display::getCanvas(){
	return canvas;
}

void Display::drawTest(){
	printf("Draw test...\n");
	lgfx.clear(TFT_RED);
	lgfx.setCursor(10, 10);
	lgfx.printf("Hellooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
	lgfx.drawLine(0, 0, 127, 0, TFT_GREEN);
	lgfx.drawLine(0, 0, 0, 127, TFT_GREEN);
	lgfx.drawLine(0, 127, 127, 127, TFT_GREEN);
	lgfx.drawLine(127, 0, 127, 127, TFT_GREEN);
	printf("Done.\n");
}

void Display::drawFile(Sprite& sprite, File icon, int16_t x, int16_t y, uint16_t width, uint16_t height, uint8_t scale, int32_t maskingColor){
	static constexpr uint32_t BufferLength = 512;

	icon.seek(0);
	Color buffer[BufferLength];
	size_t bufferPos = 0;
	size_t available = icon.read(reinterpret_cast<uint8_t*>(buffer), BufferLength) / 2;

	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			if(bufferPos == available){
				available = icon.read(reinterpret_cast<uint8_t*>(buffer), BufferLength) / 2;
				if(available == 0){
					return;
				}
				bufferPos = 0;
			}

			uint16_t color = buffer[bufferPos++];

			if(color != maskingColor || maskingColor == -1){
				sprite.fillRect(x + j * scale, y + i * scale, scale, scale, color);
			}
		}
	}
}
