#include "Character.h"
#include <sstream>

const char* animNames[] = { "general", "scratch", "look", "stretch", "wave", "dance", "knock" };

Character::Character(lv_obj_t* parentSprite, uint8_t charLevel, bool rusty, Anim currentAnim) :
		LVObject(parentSprite), charLevel(charLevel), rusty(rusty), currentAnim(currentAnim){

	gif = lv_gif_create(*this);
	lv_gif_set_src(gif, getAnimPath(charLevel, rusty, currentAnim).c_str());

	lv_obj_add_event_cb(gif, [](lv_event_t* e){
		auto _this = (Character*) lv_event_get_user_data(e);

		if(_this->canChange){
			_this->startNextAnim();
			_this->canChange = false;
		}else{
			lv_gif_restart(_this->gif);
		}
	}, LV_EVENT_READY, this);

}

void Character::setCharLevel(uint8_t charLevel){
	if(Character::charLevel == charLevel) return;

	uint8_t curr = Character::charLevel;
	Character::charLevel = charLevel;

	if(getGIFLevel(curr) == getGIFLevel(charLevel)) return;

	registerNextAnim();
	startNextAnim();
}

void Character::setRusty(bool rusty){
	if(Character::rusty == rusty) return;

	Character::rusty = rusty;
	registerNextAnim();
}

void Character::setAnim(Anim anim){
	if(Character::currentAnim == anim) return;

	Character::currentAnim = anim;
	registerNextAnim();
}

void Character::startNextAnim(){
	if(!nextAnim) return;

	lv_gif_set_src(gif, getAnimPath(nextAnim->charLevel, nextAnim->rusty, nextAnim->anim).c_str());
	lv_gif_restart(gif);

	nextAnim = std::nullopt;
}

std::string Character::getAnimPath(uint8_t charLevel, bool rusty, Anim anim){
	uint8_t level = getGIFLevel(charLevel);
	char path[50];
	if(rusty){
		sprintf(path, "S:/Home/rusty/%02d_%s.gif", level, animNames[(uint8_t) anim]);
	}else{
		sprintf(path, "S:/Home/%02d_%s.gif", level, animNames[(uint8_t) anim]);
	}
	return std::string(path);
}

void Character::registerNextAnim(){
	nextAnim = CharacterAnim{ charLevel, rusty, currentAnim };
	canChange = true;
}

uint8_t Character::getGIFLevel(uint8_t level){
	uint8_t nextLevel;
	if(level <= 3){
		nextLevel = 1;
	}else if(level <= 5){
		nextLevel = 4;
	}else{
		nextLevel = 6;
	}
	return nextLevel;
}
