#ifndef CHATTER_FIRMWARE_PROMPT_H
#define CHATTER_FIRMWARE_PROMPT_H

#include "LV_Interface/LVModal.h"
#include "LV_Interface/LVStyle.h"

#define EV_PROMPT_YES ((lv_event_code_t) (_LV_EVENT_LAST + 1))
#define EV_PROMPT_NO ((lv_event_code_t) (_LV_EVENT_LAST + 2))

class Prompt : public LVModal{
public:
	Prompt(LVScreen* parent, const char* text);

private:
	LVStyle btnDefault;
	LVStyle btnFocused;

	lv_obj_t* noBtn;
	lv_obj_t* yesBtn;
};


#endif //CHATTER_FIRMWARE_PROMPT_H
