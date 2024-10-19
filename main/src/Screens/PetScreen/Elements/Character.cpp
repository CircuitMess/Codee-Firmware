#include "Character.h"
#include <sstream>

Character::Character(lv_obj_t* parent, uint8_t level, bool rusty) : LVObject(parent), level(level), rusty(rusty){
	lv_obj_set_size(*this, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	startAnim();
}

void Character::setLevel(uint8_t level){
	if(this->level == level || level > 6) return;
	this->level = level;
	if(!queuedAnim){
		queuedAnim.rusty = rusty;
	}
	queuedAnim.level = level;
}

void Character::setRusty(bool rusty){
	if(this->rusty == rusty) return;
	this->rusty = rusty;
	if(!queuedAnim){
		queuedAnim.level = level;
	}
	queuedAnim.rusty = rusty;
}

void Character::playIdle(){
	if(queuedAnim && queuedAnim.alt) return;
	if(!queuedAnim){
		queuedAnim.level = level;
		queuedAnim.rusty = rusty;
	}
	queuedAnim.alt = true;
}

void Character::startAnim(){
	const char* path;
	bool alt;
	if(queuedAnim){
		path = getAnimPath(queuedAnim.level, queuedAnim.rusty, queuedAnim.alt);
		alt = queuedAnim.alt;
		queuedAnim = {};
	}else{
		path = getAnimPath(level, rusty, false);
		alt = false;
	}

	if(gif != nullptr){
		lv_obj_delete(*gif);
	}

	gif = new LVGIF(*this, path);
	gif->setLoopCallback([this](){
		if(queuedAnim){
			startAnim();
		}
	});

	if(alt){
		gif->setLooping(LVGIF::LoopType::On);
		queuedAnim = { .level = level, .rusty = rusty, .alt = false };
	}else{
		gif->setLooping(LVGIF::LoopType::On);
	}

	gif->start();
}

constexpr const char* Character::getAnimPath(uint8_t level, bool rusty, bool alt){
	const auto i = getAnimIndex(level);
	if(!rusty && !alt){
		return Paths[i].idle;
	}else if(rusty && !alt){
		return Paths[i].idle_r;
	}else if(!rusty && alt){
		return Paths[i].alt;
	}else if(rusty && alt){
		return Paths[i].alt_r;
	}else{
		return Paths[i].idle;
	}
}

constexpr uint8_t Character::getAnimIndex(uint8_t level){
	if(level <= 2){
		return 0;
	}else if(level <= 5){
		return 1;
	}else{
		return 2;
	}
}
