#ifndef CLOCKSTAR_FIRMWARE_BOOLELEMENT_H
#define CLOCKSTAR_FIRMWARE_BOOLELEMENT_H


#include <functional>
#include "LV_Interface/LVObject.h"
#include "LV_Interface/LVStyle.h"

class BoolElement : public LVObject {
public:
	explicit BoolElement(lv_obj_t* parent, const char* name, std::function<void(bool)> cb, bool value = false);

	void setValue(bool value);
	[[nodiscard]] bool getValue() const;

private:
	lv_obj_t* label;
	lv_obj_t* switchElement;
	LVStyle defaultStyle;
	LVStyle focusedStyle;
	LVStyle labelStyle;
	LVStyle switchStyle;
	LVStyle switchCheckedStyle;
	LVStyle switchKnobStyle;
	LVStyle switchKnobCheckedStyle;

	bool value;
	std::function<void(bool)> cb;

	static constexpr lv_style_selector_t SelDefault = LV_PART_MAIN | LV_STATE_DEFAULT;
	static constexpr lv_style_selector_t SelFocus = LV_PART_MAIN | LV_STATE_FOCUSED;

	static constexpr uint8_t SwitchWidth = 25;
	static constexpr uint8_t SwitchHeight = 15;
	static constexpr uint8_t Height = 23;
};

#endif //CLOCKSTAR_FIRMWARE_BOOLELEMENT_H
