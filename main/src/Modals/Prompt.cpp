#include "Prompt.h"

Prompt::Prompt(LVScreen* parent, const char* text) : LVModal(parent){

	lv_style_set_pad_all(btnDefault, 3);

	lv_style_set_pad_all(btnFocused, 2);
	lv_style_set_border_opa(btnFocused, LV_OPA_100);
	lv_style_set_border_color(btnFocused, lv_color_black());
	lv_style_set_border_width(btnFocused, 1);
	lv_style_set_bg_color(btnFocused, lv_color_hex(0x309bb9));
	lv_style_set_bg_opa(btnFocused, LV_OPA_100);

	lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_layout(obj, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);

	lv_obj_t* textContainer = lv_obj_create(obj);
	lv_obj_remove_flag(textContainer, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_size(textContainer, lv_pct(100), LV_SIZE_CONTENT);
	lv_obj_set_style_pad_ver(textContainer, 7, 0);
	lv_obj_set_style_pad_hor(textContainer, 2, 0);

	lv_obj_t* label = lv_label_create(textContainer);
	lv_obj_set_width(label, lv_pct(100));
	lv_label_set_text(label, text);
	lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
	lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
//	lv_obj_set_style_text_font(label, &pixelbasic7, 0);
	lv_obj_set_style_text_color(label, lv_color_black(), 0);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

	lv_obj_t* buttonContainer = lv_obj_create(obj);
	lv_obj_remove_flag(buttonContainer, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_size(buttonContainer, lv_pct(100), LV_SIZE_CONTENT);
	lv_obj_set_style_border_width(buttonContainer, 1, 0);
	lv_obj_set_style_border_color(buttonContainer, lv_color_black(), 0);
	lv_obj_set_style_border_opa(buttonContainer, LV_OPA_100, 0);

	yesBtn = lv_button_create(buttonContainer);
	lv_obj_align(yesBtn, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_set_size(yesBtn, lv_pct(50), LV_SIZE_CONTENT);
	lv_obj_t* yesLabel = lv_label_create(yesBtn);
//	lv_obj_set_style_text_font(yesLabel, &pixelbasic7, 0);
	lv_obj_set_style_text_color(yesLabel, lv_color_black(), 0);
	lv_obj_align(yesLabel, LV_ALIGN_CENTER, 0, 0);
	lv_label_set_text(yesLabel, "YES");
	lv_obj_set_height(yesBtn, LV_SIZE_CONTENT);

	noBtn = lv_button_create(buttonContainer);
	lv_obj_align(noBtn, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_set_size(noBtn, lv_pct(50), LV_SIZE_CONTENT);
	lv_obj_t* noLabel = lv_label_create(noBtn);
//	lv_obj_set_style_text_font(noLabel, &pixelbasic7, 0);
	lv_obj_set_style_text_color(noLabel, lv_color_black(), 0);
	lv_obj_align(noLabel, LV_ALIGN_CENTER, 0, 0);
	lv_label_set_text(noLabel, "NO");
	lv_obj_set_height(noBtn, LV_SIZE_CONTENT);

	lv_obj_set_height(buttonContainer, LV_SIZE_CONTENT);

	lv_obj_add_style(yesBtn, btnDefault, LV_STATE_DEFAULT);
	lv_obj_add_style(noBtn, btnDefault, LV_STATE_DEFAULT);
	lv_obj_add_style(yesBtn, btnFocused, LV_STATE_FOCUSED);
	lv_obj_add_style(noBtn, btnFocused, LV_STATE_FOCUSED);

	lv_group_add_obj(inputGroup, yesBtn);
	lv_group_add_obj(inputGroup, noBtn);
	lv_group_focus_obj(noBtn);


//	lv_obj_set_height(obj, LV_SIZE_CONTENT);
//	lv_obj_set_height(container, LV_SIZE_CONTENT);

	lv_obj_add_event_cb(yesBtn, [](lv_event_t* event){
		auto modal = static_cast<LVModal*>(lv_event_get_user_data(event));
		lv_obj_send_event(*modal, EV_PROMPT_YES, modal);
	}, LV_EVENT_CLICKED, this);

	lv_obj_add_event_cb(noBtn, [](lv_event_t* event){
		auto modal = static_cast<LVModal*>(lv_event_get_user_data(event));
		lv_obj_send_event(*modal, EV_PROMPT_NO, modal);
	}, LV_EVENT_CLICKED, this);

	lv_obj_add_event_cb(yesBtn, [](lv_event_t* event){
		auto modal = static_cast<LVModal*>(lv_event_get_user_data(event));
		if(lv_event_get_key(event) == LV_KEY_ESC){
			lv_obj_send_event(*modal, EV_PROMPT_NO, modal);
		}
	}, LV_EVENT_KEY, this);

	lv_obj_add_event_cb(noBtn, [](lv_event_t* event){
		auto modal = static_cast<LVModal*>(lv_event_get_user_data(event));
		if(lv_event_get_key(event) == LV_KEY_ESC){
			lv_obj_send_event(*modal, EV_PROMPT_NO, modal);
		}
	}, LV_EVENT_KEY, this);

}
