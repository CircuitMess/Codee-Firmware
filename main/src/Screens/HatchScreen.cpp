#include "HatchScreen.h"
#include "Services/StatsManager.h"
#include "Util/Services.h"
#include "Devices/Input.h"
#include "UIThread.h"
#include "PetScreen/PetScreen.h"

HatchScreen::HatchScreen() : queue(4){
	lv_obj_set_style_bg_image_src(*this, "S:/Bg/Level1.bin", 0);
	lv_obj_set_style_bg_image_opa(*this, LV_OPA_COVER, 0);

	gif = new LVGIF(*this, "S:/Anim/Hatch");
	gif->setLoopCallback([this](){
		auto stats = (StatsManager*) Services.get(Service::Stats);
		stats->reset();
		stats->setHatched(true);

		transition([](){ return std::make_unique<PetScreen>(); });
	});
	lv_obj_add_flag(*gif, LV_OBJ_FLAG_HIDDEN);

	modal = lv_image_create(*this);
	lv_image_set_src(modal, "S:/Modal.bin");
	lv_obj_center(modal);

	label = lv_label_create(modal);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(label, "Press any key\nto receive\nyour Codee!");
	lv_obj_set_style_text_color(label, lv_color_black(), 0);
	lv_obj_set_style_text_line_space(label, 8, 0);
	lv_obj_center(label);
}

void HatchScreen::onStart(){
	Events::listen(Facility::Input, &queue);
}

void HatchScreen::onStop(){
	gif->stop();
	Events::unlisten(&queue);
}

void HatchScreen::loop(){
	Event event{};
	if(!queue.get(event, 0)) return;

	if(event.facility == Facility::Input && event.data){
		auto e = (Input::Data*) event.data;
		if(e->action == Input::Data::Press){
			lv_obj_delete(modal);
			lv_obj_delete(label);
			lv_obj_remove_flag(*gif, LV_OBJ_FLAG_HIDDEN);
			gif->start();
			Events::unlisten(&queue);
		}
	}

	free(event.data);
}
