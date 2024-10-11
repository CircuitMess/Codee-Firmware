#include "LabelElement.h"
#include <utility>
#include "Settings/Settings.h"
#include "Util/Services.h"

LabelElement::LabelElement(lv_obj_t* parent, const char* name, std::function<void()> cb, lv_align_t textAlign) : LVObject(parent), cb(std::move(cb)){
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
	lv_style_set_border_color(focusedStyle, lv_palette_main(LV_PALETTE_CYAN));
	lv_style_set_border_opa(focusedStyle, LV_OPA_COVER);

	lv_style_set_text_color(labelStyle, lv_color_white());


	lv_obj_set_style_bg_opa(*this, 0, 0);

	lv_obj_set_height(*this, Height);
	lv_obj_set_width(*this, lv_pct(100));

	lv_obj_add_style(*this, focusedStyle, SelFocus);
	lv_obj_add_style(*this, defaultStyle, SelDefault);

	label = lv_label_create(*this);
	lv_obj_align(label, textAlign, 0, 0);
	lv_obj_add_style(label, labelStyle, 0);
	lv_label_set_text(label, name);

	lv_obj_add_event_cb(obj, [](lv_event_t* e){
		auto element = static_cast<LabelElement*>(lv_event_get_user_data(e));
		if(element->cb) element->cb();
	}, LV_EVENT_CLICKED, this);
}
