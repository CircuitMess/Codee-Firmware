#ifndef BIT_FIRMWARE_TEXTRC_H
#define BIT_FIRMWARE_TEXTRC_H

#include "RenderComponent.h"

struct TextStyle {
	const lgfx::IFont* font;
	uint16_t color;
	uint16_t scale;
	lgfx::textdatum_t datum;
};

class TextRC : public RenderComponent {
public:
	TextRC(std::string text, TextStyle style = DefaultStyle);
	~TextRC() override = default;

	void setText(std::string text);
	void setStyle(TextStyle style);

protected:
	void push(Sprite& parent, PixelDim pos, float rot, bool flipX, bool flipY) const override;

private:
	std::string text;
	TextStyle style;

	static constexpr TextStyle DefaultStyle = { &lgfx::fonts::Font0, TFT_BLACK, 1, textdatum_t::top_left };
};


#endif //BIT_FIRMWARE_TEXTRC_H
