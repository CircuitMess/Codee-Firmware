#include "Menu.h"
#include "Util/Services.h"
#include "Services/StatsManager.h"

Menu::Menu(lv_obj_t* parent, lv_group_t* inputGroup) : LVObject(parent){

	items.reserve((uint32_t) Games::COUNT + 1);

	auto stats = (StatsManager*) Services.get(Service::Stats);

	lv_obj_add_flag(*this, LV_OBJ_FLAG_SCROLLABLE);


	lv_obj_set_height(*this, 36);
	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_gap(*this, 10, 0);

	lv_obj_set_style_pad_hor(*this, 44, 0);

	for(int8_t i = -1; i < (int) Games::COUNT; i++){

		lv_obj_t* obj;
		if(i == -1){

			obj = *(new GrayscaleImageElement(*this, "S:/MenuIcons/settings.bin", "S:/MenuIcons/settings.bin", false));
			lv_obj_add_event_cb(obj, [](lv_event_t* e){
				auto menu = (Menu*) lv_event_get_user_data(e);

				//TODO - open settings
			}, LV_EVENT_CLICKED, this);

		}else{
			items[i] = new GrayscaleImageElement(*this, GameItems[i].unlockedPath, GameItems[i].lockedPath, stats->getLevel() >= GameItems[i].levelRequired);
			obj = *items[i];
			lv_obj_add_event_cb(*items[i], [](lv_event_t* e){
				auto menu = (Menu*) lv_event_get_user_data(e);

				//TODO - launch game
			}, LV_EVENT_CLICKED, this);
		}


		lv_obj_add_event_cb(obj, [](lv_event_t* e){
			auto menu = (Menu*) lv_event_get_user_data(e);
			auto item = lv_event_get_target_obj(e);
			auto index = lv_obj_get_index(item);

			lv_obj_scroll_to(*menu, index * (42), 0, LV_ANIM_ON);

		}, LV_EVENT_FOCUSED, this);

		lv_group_add_obj(inputGroup, obj);
	}

	border = lv_image_create(*this);
	lv_image_set_src(border, "S:/MenuIcons/Border.bin");
	lv_obj_add_flag(border, LV_OBJ_FLAG_FLOATING);
	lv_obj_center(border);

	lv_obj_set_width(*this, 120);

	lv_group_set_wrap(inputGroup, false);

	lv_group_focus_obj(*items[0]);
	lv_obj_scroll_to(*this, 42, 0, LV_ANIM_OFF);

}
