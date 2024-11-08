#include "Game.h"
#include <utility>
#include <esp_log.h>
#include "Util/Services.h"
#include "UIThread.h"
#include "Util/Notes.h"
#include "Screens/ScoreScreen.h"
#include "Util/stdafx.h"
#include "Screens/PetScreen/PetScreen.h"

Game::Game(Sprite& base, Games gameType, const char* root, std::vector<ResDescriptor> resources) :
		collision(this), inputQueue(12), audio(*(ChirpSystem*) Services.get(Service::Audio)), gameType(gameType), base(base),
		resMan(root), resources(std::move(resources)),
		loadTask([this](){ loadFunc(); }, "loadTask", 4096, 12, 0),
		render(this, base){

	exited = false;
}

void Game::load(){
	if(loaded || loadTask.running()) return;

	loadTask.start();
}

void Game::loadFunc(){
	resMan.load(resources);
	onLoad();
	loaded = true;
	loadTask.stop(0);
}

void Game::start(){
	if(started) return;

	if(!loaded){
		ESP_LOGE("Game", "Attempting to start game that wasn't loaded.");
		return;
	}

	started = true;
	onStart();
	Events::listen(Facility::Input, &inputQueue);
}

void Game::stop(){
	if(!started) return;
	started = false;
	onStop();
	Events::unlisten(&inputQueue);
}

bool Game::isLoaded() const{
	return loaded;
}

File Game::getFile(const std::string& path){
	return resMan.getResource(path);
}

void Game::addObject(const GameObjPtr& obj){
	objects.insert(obj);
}

void Game::addObjects(std::initializer_list<const GameObjPtr> objs){
	for(const auto& obj: objs){
		addObject(obj);
	}
}

void Game::removeObject(const GameObjPtr& obj){
	collision.removeObject(*obj);
	objects.erase(obj);
}

void Game::removeObjects(std::initializer_list<const GameObjPtr> objs){
	for(const auto& obj: objs){
		removeObject(obj);
	}
}

void Game::handleInput(const Input::Data& data){

}

void Game::exit(){
	exited = true;


	auto ui = (UIThread*) Services.get(Service::UI);
	if(ui == nullptr){
		return;
	}

	auto stats = returnStats();

	if(stats.oilLevel + stats.happiness + stats.experience == 0){
		ui->startScreen([](){ return std::make_unique<PetScreen>(); });
		return;
	}

	ui->startScreen([stats](){ return std::make_unique<ScoreScreen>(stats); });
}

void Game::loop(uint micros){
	Event e;
	if(inputQueue.get(e, 0)){
		if(e.facility == Facility::Input){
			auto data = (Input::Data*) e.data;
			if(data->btn == Input::D && data->action == Input::Data::Press){
				stop();
//				audio.play({{ NOTE_E6, NOTE_E6, 100 },
//							{ NOTE_C6, NOTE_C6, 100 },
//							{ NOTE_E6, NOTE_E6, 100 },
//							{ NOTE_C6, NOTE_C6, 100 }});

				auto ui = (UIThread*) Services.get(Service::UI);
				ui->exitGame();

				free(e.data);
				return;
			}

			handleInput(*data);
		}
		free(e.data);

		if(exited) return;
	}

	collision.update(micros);
	onLoop((float) micros / 1000000.0f);
	if(exited) return;

	preRender(base);
	render.update(micros);
	onRender(base);

//	collision.drawDebug(base);
}

void Game::onStart(){

}

void Game::onStop(){

}

void Game::onLoad(){}

void Game::onLoop(float deltaTime){}

void Game::preRender(Sprite& canvas){}

void Game::onRender(Sprite& canvas){}
