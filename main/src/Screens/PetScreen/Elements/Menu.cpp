#include <string>
#include <cmath>
#include <utility>
#include "Menu.h"
#include "Devices/Input.h"
#include "Util/stdafx.h"

Menu::Menu(lv_obj_t* parent, std::function<void(uint8_t)> launch) : LVObject(parent), launch(std::move(launch)), evts(6){
	lv_obj_set_size(*this, 128, ItemSize.y);

	container = lv_obj_create(*this);
	lv_obj_set_size(container, ItemSize.x, ItemSize.y);
	lv_obj_center(container);

	lv_obj_add_flag(container, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
	lv_obj_add_event_cb(container, [](lv_event_t* e){
		lv_event_code_t code = lv_event_get_code(e);
		lv_obj_t* obj = (lv_obj_t*) lv_event_get_target(e);

		if(code == LV_EVENT_REFR_EXT_DRAW_SIZE){
			lv_event_set_ext_draw_size(e, 64);
		}else if(code == LV_EVENT_DRAW_MAIN_END){
			lv_obj_refresh_ext_draw_size(obj);
		}
	}, LV_EVENT_ALL, nullptr);

	lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_gap(container, 12, 0);

	grp = lv_group_create();

	const auto addItem = [this](const char* icon){
		lv_obj_t* img = lv_image_create(container);
		lv_image_set_src(img, icon);
		lv_obj_add_flag(img, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
		lv_group_add_obj(grp, img);
	};

	for(int i = 0; i < 6; i++){
		const auto path = "S:/Menu/Game" + std::to_string(i+1) + ".bin";
		addItem(path.c_str());
	}
	addItem("S:/Menu/Settings.bin");

	hideTimer = lv_timer_create([](lv_timer_t* timer){
		auto menu = (Menu*) timer->user_data;
		menu->hide();
	}, HideTimeout, this);

	Events::listen(Facility::Input, &evts);
}

Menu::~Menu(){
	Events::unlisten(&evts);
	lv_timer_delete(hideTimer);
	lv_group_delete(grp);
}

void Menu::loop(){
	Event evt{};
	if(!evts.get(evt, 0)) return;
	if(evt.facility != Facility::Input){
		free(evt.data);
		return;
	}

	const auto data = (Input::Data*) evt.data;

	if(data->action != Input::Data::Press) return;

	if(hidden){
		show();
	}else{
		lv_timer_reset(hideTimer);

		if(data->btn == Input::A){
			lv_group_focus_prev(grp);
		}else if(data->btn == Input::B){
			lv_group_focus_next(grp);
		}else if(data->btn == Input::C && !hiding){ // TODO: Shake if locked // locked icon
			free(evt.data);
			const auto target = lv_group_get_focused(grp);
			const auto index = lv_obj_get_index(target);
			launch(index);
			return;
		}
	}

	free(evt.data);
}

void Menu::hide(){
	if(hidden || hiding) return;
	hiding = true;
	hidden = true;

	lv_timer_pause(hideTimer);

	startPos = lv_obj_get_y(*this);

	lv_anim_init(&hideAnim);
	lv_anim_set_exec_cb(&hideAnim, [](void* var, int32_t val){
		auto menu = (Menu*) var;
		lv_obj_set_style_translate_y(*menu, val, 0);
	});
	lv_anim_set_var(&hideAnim, this);
	lv_anim_set_duration(&hideAnim, 500);
	lv_anim_set_values(&hideAnim, 0, 128-startPos);
	lv_anim_set_path_cb(&hideAnim, lv_anim_path_ease_in);
	lv_anim_set_completed_cb(&hideAnim, [](lv_anim_t* anim){
		auto menu = (Menu*) anim->var;
		menu->hiding = false;
	});
	lv_anim_start(&hideAnim);
}

void Menu::show(){
	if(!hidden && !hiding) return;
	hidden = false;

	lv_timer_reset(hideTimer);
	lv_timer_resume(hideTimer);

	if(hiding){
		lv_anim_delete(this, nullptr);
	}
	hiding = true;

	const auto pos = lv_obj_get_y(*this);
	const uint32_t duration = std::round(map((float) pos, startPos, 128, 0, 300));

	lv_anim_init(&hideAnim);
	lv_anim_set_exec_cb(&hideAnim, [](void* var, int32_t val){
		auto menu = (Menu*) var;
		lv_obj_set_style_translate_y(*menu, val, 0);
	});
	lv_anim_set_var(&hideAnim, this);
	lv_anim_set_duration(&hideAnim, duration);
	lv_anim_set_values(&hideAnim, pos-startPos, 0);
	lv_anim_set_path_cb(&hideAnim, lv_anim_path_ease_out);
	lv_anim_set_completed_cb(&hideAnim, [](lv_anim_t* anim){
		auto menu = (Menu*) anim->var;
		menu->hiding = false;
	});
	lv_anim_start(&hideAnim);
}
