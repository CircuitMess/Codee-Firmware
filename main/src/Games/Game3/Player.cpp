#include "Player.h"
#include "GameEngine/Collision/CircleCC.h"
#include "Game3.h"

Game3::Player::Player(File walk, File eat, File eatBad) : walk(walk), eat(eat), eatBad(eatBad){
	go = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(this->walk),
			std::make_unique<CircleCC>(collisionRadius, collisionOffset)
	);
	go->setPos({ (128 - width) / 2, 128 - height });
	anim = std::static_pointer_cast<AnimRC>(go->getRenderComponent());
	anim->setLoopMode(GIF::Infinite);
	anim->start();

}

Game3::Player::~Player(){
	anim->stop();
}

void Game3::Player::loop(float deltaTime){
	if(eatDone){
		finishEating();
		eatDone = false;
	}

//	if(eating) return;
	float moveX = deltaTime * velocity * speed;
	moveX += go->getPos().x;
	if(moveX < 8 - width / 4 || moveX + width - 10 >= 128) return;
	go->setPos({ moveX, go->getPos().y });
}

void Game3::Player::startEating(int value){
//	eating = true;
	if(value > 0){
		anim->setAnim(eat);
	}else{
		anim->setAnim(eatBad);
		eatingBad = true;
	}
	anim->start();

	anim->setLoopMode(GIF::Single);
	anim->setLoopDoneCallback([this](uint32_t t){
		eatDone = true;
	});
}

void Game3::Player::finishEating(){
	anim->setAnim(walk);
	anim->setLoopMode(GIF::Infinite);
	anim->setLoopDoneCallback({});
	eatingBad = false;
}

std::shared_ptr<GameObject> Game3::Player::getGameObject(){
	return go;
}

void Game3::Player::filled(Game3* game){
	anim->setLoopMode(GIF::Single);
	anim->setLoopDoneCallback([game](uint32_t){
		game->state = Game3::WinPause;
	});
}

bool Game3::Player::isEatingBad() const{
	return eatingBad;
}

void Game3::Player::killed(Game3* game){
	anim->setLoopMode(GIF::Single);
	anim->setLoopDoneCallback([game](uint32_t){
		game->state = Game3::DeadPause;
	});
}

void Game3::Player::btnPressed(Input::Button btn){
	if(btn != Input::A && btn != Input::B) return;

	lastPressed = btn;

	if(btn == Input::A){
		velocity = -1.0f;
	}else if(btn == Input::B){
		velocity = 1.0f;
	}
}

void Game3::Player::btnReleased(Input::Button btn){
	if(btn != Input::A && btn != Input::B) return;

	if(btn == lastPressed){
		velocity = 0.0f;
		lastPressed = Input::D;
	}
}
