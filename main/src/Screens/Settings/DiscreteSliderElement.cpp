#include "DiscreteSliderElement.h"
#include "Settings/Settings.h"
#include "Util/Services.h"

DiscreteSliderElement::DiscreteSliderElement(lv_obj_t* parent, const char* name, std::function<void(uint8_t)> cb, const std::vector<const char*>& displayValues,
											 uint8_t value) : LVSelectable(parent), value(value), cb(std::move(cb)), displayValues(displayValues){

	Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	lv_style_set_border_width(defaultStyle, 1);
	lv_style_set_border_opa(defaultStyle, 0);
	lv_style_set_pad_all(defaultStyle, 3);
	lv_style_set_bg_opa(defaultStyle, 0);

	lv_style_set_border_width(focusedStyle, 1);
	lv_style_set_border_color(focusedStyle, lv_palette_main(LV_PALETTE_CYAN));
	lv_style_set_border_opa(focusedStyle, LV_OPA_COVER);

//	lv_style_set_text_font(labelStyle, &devin);
	lv_style_set_text_color(labelStyle, lv_color_white());

	lv_style_set_bg_opa(sliderMainStyle, LV_OPA_0);
	lv_style_set_bg_color(sliderMainStyle, lv_color_hex3(0xbbb));
	lv_style_set_border_width(sliderMainStyle, 1);
	lv_style_set_border_opa(sliderMainStyle, LV_OPA_COVER);
	lv_style_set_border_color(sliderMainStyle, lv_color_white());
	lv_style_set_radius(sliderMainStyle, LV_RADIUS_CIRCLE);

	lv_obj_set_height(*this, Height);
	lv_obj_set_width(*this, lv_pct(100));

	lv_obj_add_style(*this, focusedStyle, selFocus);
	lv_obj_add_style(*this, defaultStyle, sel);

	label = lv_label_create(*this);
	lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_add_style(label, labelStyle, 0);
	lv_label_set_text(label, name);

	slider = lv_slider_create(*this);
	lv_obj_remove_style_all(slider);
	lv_obj_align(slider, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_set_size(slider, SliderWidth, SliderHeight);

	lv_slider_set_range(slider, 0, (int) displayValues.size() - 1);
	lv_obj_set_style_pad_hor(slider, 6, 0);
	lv_obj_add_style(slider, sliderMainStyle, LV_PART_MAIN);

	lv_style_set_bg_opa(sliderKnobStyle, LV_OPA_100);
	lv_style_set_radius(sliderKnobStyle, LV_RADIUS_CIRCLE);
	lv_style_set_height(sliderKnobStyle, 10);
	lv_style_set_width(sliderKnobStyle, 10);
	lv_style_set_pad_all(sliderKnobStyle, -2);
	lv_obj_add_style(slider, sliderKnobStyle, LV_PART_KNOB);
	lv_obj_set_style_bg_color(slider, lv_palette_main(LV_PALETTE_CYAN), LV_PART_KNOB | LV_STATE_EDITED);
	lv_obj_set_style_bg_color(slider, lv_color_white(), LV_PART_KNOB | LV_STATE_DEFAULT);

	valueLabel = lv_label_create(slider);
	lv_obj_add_flag(valueLabel, LV_OBJ_FLAG_FLOATING);
	lv_obj_set_style_align(valueLabel, LV_ALIGN_CENTER, 0);
//	lv_obj_set_style_text_font(valueLabel, &devin, 0);
	lv_obj_set_style_pad_top(valueLabel, 1, 0);
	lv_obj_set_style_text_color(valueLabel, lv_color_make(150, 150, 150), 0);

	lv_group_add_obj(inputGroup, slider);

	lv_obj_add_event_cb(slider, [](lv_event_t* e){
		auto element = static_cast<DiscreteSliderElement*>(lv_event_get_user_data(e));
		element->deselect();
	}, LV_EVENT_CLICKED, this);

	lv_obj_add_event_cb(slider, [](lv_event_t* e){
		auto element = static_cast<DiscreteSliderElement*>(lv_event_get_user_data(e));
		element->value = lv_slider_get_value(element->slider);
		if(element->value >= element->displayValues.size()){
			lv_label_set_text(element->valueLabel, "");
		}else{
			lv_label_set_text(element->valueLabel, element->displayValues[element->value]);
		}
		if(element->cb) element->cb(element->value);
	}, LV_EVENT_VALUE_CHANGED, this);

	lv_obj_add_event_cb(slider, [](lv_event_t* e){
		if(lv_event_get_key(e) != LV_KEY_ESC) return;

		auto element = static_cast<DiscreteSliderElement*>(lv_event_get_user_data(e));
		element->deselect();
	}, LV_EVENT_KEY, this);

	setValue(value);
}

void DiscreteSliderElement::setValue(uint8_t value){
	this->value = value;
	lv_slider_set_value(slider, value, LV_ANIM_OFF);
	lv_label_set_text(valueLabel, displayValues[value]);
}

uint8_t DiscreteSliderElement::getValue() const{
	return value;
}

void DiscreteSliderElement::onSelect(){
	lv_group_set_editing(inputGroup, true);

}

void DiscreteSliderElement::onDeselect(){
	lv_group_set_editing(inputGroup, false);
}
