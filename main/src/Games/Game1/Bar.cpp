#include "Bar.h"
#include <esp_random.h>
#include "lvgl.h"

Bar::Bar(std::shared_ptr<Sprite> sprite) : sprite(sprite){
}

void Bar::resetGoal(){
	yGoal = esp_random() % (sprite->height() - 2);
	draw();
}

int Bar::getY(){
	return yGoal + 1; // + 1 to middle of the goal
}

void Bar::draw(){
	const auto width = sprite->width();
	const auto height = sprite->height();

	sprite->clear(TFT_TRANSPARENT);

	for(int y = 1; y < height - 1; y++){
		float difference = abs(yGoal - y);
		double hue = ((height - 2) - difference) / (float) (height - 2) * 60.0 / 255.0 * 360;
		lv_color_t rgbColor0 = lv_color_hsv_to_rgb((uint16_t) hue, 100, 100);
		uint16_t c0 = lgfx::color565(rgbColor0.red, rgbColor0.green, rgbColor0.blue);
		sprite->fillRect(1, y, width - 2, 1, c0);
	}
	sprite->drawRoundRect(0, 0, width, height, 1, TFT_BLACK);
	sprite->fillRect(1, yGoal + 1, width - 2, 3, TFT_DARKGREEN);
}
