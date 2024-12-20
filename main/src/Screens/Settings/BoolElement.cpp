#include "BoolElement.h"
#include <utility>
#include "widgets/switch/lv_switch.h"
#include "Settings/Settings.h"
#include "Util/Services.h"

BoolElement::BoolElement(lv_obj_t* parent, const char* name, std::function<void(bool)> cb, bool value) : LVObject(parent), value(value), cb(std::move(cb)){
	Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	lv_obj_add_flag(*this, LV_OBJ_FLAG_CLICKABLE);

	lv_style_set_border_width(defaultStyle, 1);
	lv_style_set_border_opa(defaultStyle, 0);
	lv_style_set_pad_all(defaultStyle, 3);
	lv_style_set_bg_opa(defaultStyle, 0);

	lv_style_set_border_width(focusedStyle, 1);
	lv_style_set_border_color(focusedStyle,  lv_palette_main(LV_PALETTE_CYAN));
	lv_style_set_border_opa(focusedStyle, LV_OPA_COVER);

	lv_style_set_text_color(labelStyle, lv_color_white());

	lv_style_set_border_width(switchStyle, 1);
	lv_style_set_border_color(switchStyle, lv_color_white());
	lv_style_set_bg_opa(switchStyle, LV_OPA_0);
	lv_style_set_radius(switchStyle, LV_RADIUS_CIRCLE);
	lv_style_set_anim_time(switchStyle, 120);

	lv_style_set_border_width(switchCheckedStyle, 1);
	lv_style_set_border_color(switchCheckedStyle, lv_color_white());
	lv_style_set_bg_color(switchCheckedStyle, lv_color_white());
	lv_style_set_bg_opa(switchCheckedStyle, LV_OPA_0);
	lv_style_set_text_color(switchCheckedStyle, lv_color_white());

	lv_style_set_bg_color(switchKnobStyle, lv_color_white());
	lv_style_set_bg_opa(switchKnobStyle, LV_OPA_0);
	lv_style_set_radius(switchKnobStyle, LV_RADIUS_CIRCLE);
	lv_style_set_pad_all(switchKnobStyle, -3);
	lv_style_set_border_color(switchKnobStyle, lv_color_white());
	lv_style_set_border_width(switchKnobStyle, 1);
	lv_style_set_border_opa(switchKnobStyle, LV_OPA_COVER);

	lv_style_set_bg_color(switchKnobCheckedStyle, lv_palette_main(LV_PALETTE_CYAN));
	lv_style_set_bg_opa(switchKnobCheckedStyle, LV_OPA_100);
	lv_style_set_radius(switchKnobCheckedStyle, LV_RADIUS_CIRCLE);
	lv_style_set_pad_all(switchKnobCheckedStyle, -3);
	lv_style_set_border_color(switchKnobCheckedStyle, lv_palette_main(LV_PALETTE_CYAN));
	lv_style_set_border_width(switchKnobCheckedStyle, 1);
	lv_style_set_border_opa(switchKnobCheckedStyle, LV_OPA_COVER);

	lv_obj_set_height(*this, Height);
	lv_obj_set_width(*this, lv_pct(100));

	lv_obj_add_style(*this, focusedStyle, SelFocus);
	lv_obj_add_style(*this, defaultStyle, SelDefault);

	label = lv_label_create(*this);
	lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_add_style(label, labelStyle, 0);
	lv_label_set_text(label, name);

	switchElement = lv_switch_create(*this);
	lv_obj_align(switchElement, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_set_size(switchElement, SwitchWidth, SwitchHeight);

	lv_obj_add_event_cb(obj, [](lv_event_t* e){
		auto element = static_cast<BoolElement*>(lv_event_get_user_data(e));
		element->setValue(!element->value);
		if(element->cb) element->cb(element->value);
		lv_obj_send_event(element->switchElement, LV_EVENT_VALUE_CHANGED, nullptr);
	}, LV_EVENT_CLICKED, this);


	lv_obj_add_style(switchElement, switchStyle, 0);
	lv_obj_add_style(switchElement, switchCheckedStyle, LV_PART_INDICATOR | LV_STATE_CHECKED);
	lv_obj_add_style(switchElement, switchKnobStyle, LV_PART_KNOB);
	lv_obj_add_style(switchElement, switchKnobCheckedStyle, LV_PART_KNOB | LV_STATE_CHECKED);
	lv_obj_set_style_radius(switchElement, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);

	setValue(value);
}

void BoolElement::setValue(bool value){
	this->value = value;
	value ? lv_obj_add_state(switchElement, LV_STATE_CHECKED) : lv_obj_clear_state(switchElement, LV_STATE_CHECKED);
}

bool BoolElement::getValue() const{
	return value;
}
