#include "HatchScreen.h"
#include "Services/StatsManager.h"
#include "Util/Services.h"
#include "Devices/Input.h"
#include "UIThread.h"
#include "PetScreen/PetScreen.h"

HatchScreen::HatchScreen() : queue(4){

	gif = lv_gif_create(*this);
	lv_gif_set_src(gif, "S:/hatching.gif");
	lv_gif_pause(gif);

	modal = lv_image_create(*this);
	lv_image_set_src(modal, "S:/modal.bin");
	lv_obj_center(modal);
	lv_obj_move_foreground(modal);


	label = lv_label_create(modal);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(label, "Press any key\nto receive\nyour CircuitPet!");
	lv_obj_set_style_text_color(label, lv_color_black(), 0);
	lv_obj_set_style_text_line_space(label, 8, 0);
	lv_obj_center(label);

	lv_obj_add_event_cb(gif, [](lv_event_t* e){
		auto ui = (UIThread*) Services.get(Service::UI);
		ui->startScreen([](){ return std::make_unique<PetScreen>(); });
	}, LV_EVENT_READY, this);

	Events::listen(Facility::Input, &queue);
}

HatchScreen::~HatchScreen(){
	Events::unlisten(&queue);
}

void HatchScreen::onStart(){

}

void HatchScreen::onStop(){
	auto stats = (StatsManager*) Services.get(Service::Stats);
	stats->reset();
	stats->setHatched(true);
	lv_gif_pause(gif);
}

void HatchScreen::loop(){
	Event event{};
	if(!queue.get(event, 0)) return;


	if(event.facility == Facility::Input && event.data){
		auto e = (Input::Data*) event.data;
		if(e->action == Input::Data::Press){

			lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(modal, LV_OBJ_FLAG_HIDDEN);
			lv_gif_resume(gif);
			Events::unlisten(&queue);
		}
	}

	free(event.data);
}
