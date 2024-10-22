#include <string>
#include <cmath>
#include <utility>
#include "Menu.h"
#include "Devices/Input.h"
#include "Util/stdafx.h"
#include "Services/StatsManager.h"
#include "Util/Services.h"
#include "LV_Interface/InputLVGL.h"
#include "Services/ChirpSystem.h"

Menu::Menu(lv_obj_t* parent, lv_group_t* grp, std::function<void(uint8_t)> launch) : LVObject(parent), launch(std::move(launch)), grp(grp){
	lv_obj_set_size(*this, 128, FrameSize.y);
	lv_obj_set_style_pad_ver(*this, (FrameSize.y - ItemSize.y) / 2, 0);

	lv_obj_t* container = lv_obj_create(*this);
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

	const auto addItem = [this, container, grp](const char* icon){
		lv_obj_t* btn = lv_button_create(container);
		lv_obj_set_size(btn, ItemSize.x, ItemSize.y);
		lv_obj_set_style_bg_image_src(btn, icon, 0);
		lv_obj_set_style_bg_image_opa(btn, LV_OPA_COVER, 0);
		lv_obj_add_flag(btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
		lv_group_add_obj(grp, btn);

		lv_obj_add_event_cb(btn, [](lv_event_t* e){
			const auto code = lv_event_get_code(e);
			if(code != LV_EVENT_FOCUSED && code != LV_EVENT_CLICKED && code != LV_EVENT_CANCEL) return;

			auto menu = (Menu*) lv_event_get_user_data(e);
			lv_timer_reset(menu->hideTimer);

			if(code == LV_EVENT_CLICKED){
				const auto target = (lv_obj_t*) lv_event_get_target(e);
				const auto index = lv_obj_get_index(target);
				menu->click(index);
			}else if(code == LV_EVENT_CANCEL){
				menu->hide();
			}
		}, LV_EVENT_ALL, this);
	};

	addItem("S:/Menu/Settings.bin");

	auto stats = (StatsManager*) Services.get(Service::Stats);
	const auto lvl = stats->getLevel();
	for(int i = 0; i < 6; i++){
		if(lvl >= i + 1){
			addItem(IconPaths[i].unlocked);
		}else{
			addItem(IconPaths[i].locked);
		}
	}

	lv_obj_t* frame = lv_image_create(*this);
	lv_image_set_src(frame, "S:/Menu/Frame.bin");
	lv_obj_center(frame);

	hideTimer = lv_timer_create([](lv_timer_t* timer){
		auto menu = (Menu*) timer->user_data;
		menu->hide();
	}, HideTimeout, this);

	hideGrp = lv_group_create();
	lv_group_set_editing(hideGrp, true);
	lv_obj_t* hideObj = lv_obj_create(*this);
	lv_obj_set_size(hideObj, 0, 0);
	lv_obj_add_flag(hideObj, LV_OBJ_FLAG_FLOATING);
	lv_group_add_obj(hideGrp, hideObj);

	lv_obj_add_event_cb(hideObj, [](lv_event_t* e){
		const auto code = lv_event_get_code(e);
		if(code != LV_EVENT_KEY && code != LV_EVENT_CLICKED) return;

		auto menu = (Menu*) lv_event_get_user_data(e);
		if(menu->hidden){
			menu->show();
		}
	}, LV_EVENT_ALL, this);

	lv_obj_t* first = lv_obj_get_child(container, 1);
	lv_group_focus_obj(first);
	lv_obj_scroll_to_view(first, LV_ANIM_OFF);

	for(int i = 0; i < lv_obj_get_child_count(container); i++){
		lv_obj_t* child = lv_obj_get_child(container, i);
		lv_obj_add_event_cb(child, [](lv_event_t* e){
			const auto code = lv_event_get_code(e);
			if(code != LV_EVENT_FOCUSED) return;

			if(ChirpSystem* audio = (ChirpSystem*) Services.get(Service::Audio)){
				audio->play({{ 500, 500, 50 }});
			}
		}, LV_EVENT_FOCUSED, this);
	}
}

Menu::~Menu(){
	if(shaking){
		lv_anim_delete(shakeAnim.var, nullptr);
	}

	if(hiding){
		lv_anim_delete(hideAnim.var, nullptr);
	}

	lv_timer_delete(hideTimer);
	lv_group_delete(hideGrp);
}

void Menu::stop(){
	lv_timer_pause(hideTimer);
}

void Menu::click(int index){
	if(index == 0){
		if(ChirpSystem* audio = (ChirpSystem*) Services.get(Service::Audio)){
			audio->play({{ 500, 700, 50 }});
		}
		// Settings
		launch(index);
		return;
	}

	auto stats = (StatsManager*) Services.get(Service::Stats);
	if(index > stats->getLevel()){
		shake();
		return;
	}

	if(ChirpSystem* audio = (ChirpSystem*) Services.get(Service::Audio)){
		audio->play({{ 500, 700, 50 }});
	}
	// Game 1-6
	launch(index);
}

void Menu::shake(){
	if(ChirpSystem* audio = (ChirpSystem*) Services.get(Service::Audio)){
		audio->play({{ 200, 200, 50 },
					 { 0,   0,   50 },
					 { 200, 200, 50 }});
	}

	lv_obj_t* selected = lv_group_get_focused(grp);

	if(shaking){
		lv_anim_delete(shakeAnim.var, nullptr);
	}
	shaking = true;

	lv_anim_init(&shakeAnim);
	lv_anim_set_user_data(&shakeAnim, this);
	lv_anim_set_var(&shakeAnim, selected);
	lv_anim_set_values(&shakeAnim, -2, 2);
	lv_anim_set_duration(&shakeAnim, ShakeAnimDuration);
	lv_anim_set_repeat_count(&shakeAnim, 2);
	lv_anim_set_playback_duration(&shakeAnim, ShakeAnimDuration);
	lv_anim_set_exec_cb(&shakeAnim, [](void* var, int32_t v){
		lv_obj_set_style_translate_x((lv_obj_t*) var, v, 0);
	});
	lv_anim_set_path_cb(&shakeAnim, lv_anim_path_linear);
	lv_anim_set_deleted_cb(&shakeAnim, [](lv_anim_t* a){
		auto menu = (Menu*) a->user_data;
		menu->shaking = false;
		lv_obj_set_style_translate_x((lv_obj_t*) a->var, 0, 0);
	});
	lv_anim_start(&shakeAnim);
}

void Menu::hideNow(){
	if(hidden || hiding) return;
	hiding = false;
	hidden = true;

	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), hideGrp);

	startPos = lv_obj_get_y(*this);
	lv_obj_set_style_translate_y(*this, 128 - startPos, 0);
}

void Menu::hide(){
	if(hidden || hiding) return;
	hiding = true;
	hidden = true;

	lv_timer_pause(hideTimer);

	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), hideGrp);

	startPos = lv_obj_get_y(*this);

	lv_anim_init(&hideAnim);
	lv_anim_set_exec_cb(&hideAnim, [](void* var, int32_t val){
		auto menu = (Menu*) var;
		lv_obj_set_style_translate_y(*menu, val, 0);
	});
	lv_anim_set_var(&hideAnim, this);
	lv_anim_set_duration(&hideAnim, 500);
	lv_anim_set_values(&hideAnim, 0, 128 - startPos);
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

	lv_indev_set_group(lv_indev_active(), grp);

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
	lv_anim_set_values(&hideAnim, pos - startPos, 0);
	lv_anim_set_path_cb(&hideAnim, lv_anim_path_ease_out);
	lv_anim_set_completed_cb(&hideAnim, [](lv_anim_t* anim){
		auto menu = (Menu*) anim->var;
		menu->hiding = false;
	});
	lv_anim_start(&hideAnim);
}
