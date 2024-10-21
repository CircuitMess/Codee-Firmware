#include "Player.h"
#include "Game4.h"

Game4::Player::Player(std::shared_ptr<GameObject> duckGoRc, std::shared_ptr<GameObject> duckGoCc, Game4* game4) : gamePtr(game4), gameObjectRc(duckGoRc),
																												  gameObjectCc(duckGoCc){
	animRc = std::static_pointer_cast<AnimRC>(gameObjectRc->getRenderComponent());
	animRc->start();
}

Game4::Player::~Player(){
	animRc->stop();
}

void Game4::Player::update(float deltaTime){
	if(goToAnim){
		animRc->setAnim(goToAnim);
		animRc->setLoopMode(GIF::Infinite);
		goToAnim = File();
	}

	updateInvincibility(deltaTime);
	if(winFlag){
		float x = gameObjectRc->getPos().x;
		x += 30.0f * deltaTime;
		gameObjectRc->setPos({ x, gameObjectRc->getPos().y });
		if(x >= WinStopDistance){
			walk();
			winFlag = false;
			gameObjectRc->setPos({ WinStopDistance + 12, startPosY - 25 });
			animRc->setAnim(winAnim);
			animRc->setLoopMode(GIF::Single);
			animRc->setLoopDoneCallback([this](uint32_t){
				gamePtr->endPauseState = Game4::Win;
			});
		}
	}

	if(isJumping){
		float y = gameObjectRc->getPos().y;
		y += velocity * deltaTime + 0.5f * gravity * pow(deltaTime, 2);
		velocity += gravity * deltaTime * multiplier;

		if(time < peakTime && !isDead){
			time += deltaTime;
		}else{
			multiplier = 5;
		}

		gameObjectRc->setPos({ XPos, y });
		gameObjectCc->setPos({ XPos, y });
		if(y > startPosY){
			gameObjectRc->setPos({ XPos, startPosY });
			gameObjectCc->setPos({ XPos, startPosY });

			multiplier = 1.0f;
			isJumping = false;
			time = 0.0f;
			if(isDead){
				if(goToAnim){
					animRc->setAnim(goToAnim);
					animRc->setLoopMode(GIF::Infinite);
					goToAnim = File();
				}
				return;
			}
			if(duckHold){
				duck();
			}else{

				animRc->setAnim(walking);
				animRc->setLoopMode(GIF::Infinite);
			}
		}
	}

	if(goToAnim){
		animRc->setAnim(goToAnim);
		animRc->setLoopMode(GIF::Infinite);
		goToAnim = File();
	}
}

void Game4::Player::walk(){
	if(!isDucked) return;
	isDucked = false;

	animRc->setAnim(unDucking);
	animRc->setLoopMode(GIF::Single);
	animRc->setLoopDoneCallback([this](uint32_t){
		goToAnim = walking;
	});
	gameObjectCc->setRot(0.0f);
}

void Game4::Player::jump(){
	if(isJumping) return;

	gamePtr->audio.play({{ 200, 200, 50 },
						 { 200, 800, 200 }});
	isJumping = true;
	velocity = 2 * maxHeight * gamePtr->getSpeed() / posXForMaxHeight;
	gravity = -2 * maxHeight * pow(gamePtr->getSpeed(), 2) / pow(posXForMaxHeight, 2);
	peakTime = posXForMaxHeight / gamePtr->getSpeed();
	walk();
	animRc->setAnim(jumpAnim);
	animRc->setLoopMode(GIF::Single);
}

void Game4::Player::duck(){
	if(isDucked) return;
	isDucked = true;

	animRc->setAnim(ducking);
	animRc->setLoopMode(GIF::Single);
	animRc->setLoopDoneCallback([this](uint32_t){
		goToAnim = ducked;
	});
	gameObjectCc->setRot(90.0f);
}

void Game4::Player::death(){
	gamePtr->endPauseState = Game4::Lose;
	isDead = true;
	animRc->setAnim(down);
	animRc->setLoopMode(GIF::Single);
	animRc->setLoopDoneCallback([this](uint32_t){
		animRc->stop();
	});
}

std::shared_ptr<GameObject> Game4::Player::getGameObjectCc(){
	return gameObjectCc;
}

void Game4::Player::setFiles(File walk, File down, File jump, File ducking, File ducked, File unDucking, File winAnim){
	this->walking = walk;
	this->down = down;
	this->jumpAnim = jump;
	this->ducking = ducking;
	this->ducked = ducked;
	this->unDucking = unDucking;
	this->winAnim = winAnim;
}

void Game4::Player::win(){
	winFlag = true;
	if(isDucked){
		walk();
	}
}

void Game4::Player::duckPressed(){
	duckHold = true;
	gamePtr->audio.play({{ 400, 500, 80 },
						 { 500, 300, 150 }});
	if(isJumping){
		multiplier = 10.0f;
	}else{
		duck();
	}
}

void Game4::Player::jumpPressed(){
	jump();

}

void Game4::Player::duckReleased(){
	duckHold = false;
	if(isJumping) return;
	walk();
}

void Game4::Player::updateInvincibility(float delta){
	if(!invincible) return;

	invincibilityTime += delta;

	if((int) (invincibilityTime / invincibilityBlinkDuration) % 2 == 0){
		gameObjectRc->getRenderComponent()->setVisible(false);
	}else{
		gameObjectRc->getRenderComponent()->setVisible(true);
	}

	if(invincibilityTime >= invincibilityDuration){
		invincibilityTime = 0;
		invincible = false;
		gameObjectRc->getRenderComponent()->setVisible(true);
	}
}


