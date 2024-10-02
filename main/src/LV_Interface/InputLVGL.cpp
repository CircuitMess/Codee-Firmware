#include "InputLVGL.h"
#include "Pins.hpp"

InputLVGL* InputLVGL::instance = nullptr;
const std::map<Input::Button, lv_key_t> InputLVGL::keyMap = {{ Input::Button::Left,    LV_KEY_LEFT },
															 { Input::Button::Right,  LV_KEY_RIGHT },
															 { Input::Button::A,     LV_KEY_ENTER },
															 { Input::Button::B,     LV_KEY_ESC }};

const std::map<lv_key_t, lv_key_t> InputLVGL::vertRemap = {{ LV_KEY_LEFT,    LV_KEY_UP },
														   { LV_KEY_RIGHT,  LV_KEY_DOWN }};

InputLVGL::InputLVGL() : Threaded("InputLVGL", 1024, 6, 0), queue(QueueSize){
	instance = this;

	Events::listen(Facility::Input, &queue);


	inputDevice = lv_indev_create();
	lv_indev_set_type(inputDevice, LV_INDEV_TYPE_ENCODER);
	lv_indev_set_read_cb(inputDevice, [](lv_indev_t* drv, lv_indev_data_t* data){ InputLVGL::getInstance()->read(drv, data); });
	lv_indev_set_long_press_time(inputDevice, 350);

	start();
}

void InputLVGL::read(lv_indev_t* drv, lv_indev_data_t* data){
	if(keyMap.count(lastKey) == 0) return;

	auto key = keyMap.at(lastKey);
	if(vertNav && vertRemap.contains(key)){
		key = vertRemap.at(key);
	}

	data->key = key;
	data->state = (action == Input::Data::Action::Press) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

InputLVGL* InputLVGL::getInstance(){
	return instance;
}

void InputLVGL::loop(){
	Event event{};

	if(queue.get(event, portMAX_DELAY)){
		auto inputData = ((Input::Data*) event.data);
		lastKey = inputData->btn;
		action = inputData->action;
		free(event.data);
	}
}

lv_indev_t* InputLVGL::getIndev() const{
	return inputDevice;
}

void InputLVGL::setVertNav(bool vertNav){
	InputLVGL::vertNav = vertNav;
}
