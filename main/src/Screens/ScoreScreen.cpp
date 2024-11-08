#include "ScoreScreen.h"
#include "Util/Services.h"
#include "UIThread.h"
#include "Screens/PetScreen/PetScreen.h"
#include "Screens/LevelUpScreen.h"

ScoreScreen::ScoreScreen(Stats statsIncrease) : statsManager((StatsManager*) Services.get(Service::Stats)), statsIncrease(statsIncrease){
	sprintf(bgPath, "S:/Bg/Level%d.bin", statsManager->getLevel());
	lv_obj_set_style_bg_image_src(*this, bgPath, 0);

	frame = lv_image_create(*this);
	lv_image_set_src(frame, "S:/Modal.bin");
	lv_obj_center(frame);
	lv_obj_set_size(frame, 115, 82);
	lv_obj_set_style_pad_all(frame, 8, 0);
	lv_obj_set_layout(frame, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(frame, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(frame, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	lv_obj_t* title = lv_label_create(frame);
	lv_label_set_text(title, "Stats increased!");
	lv_obj_set_style_text_color(title, lv_color_black(), 0);


	for(auto& statsRow: statsRows){
		statsRow = lv_obj_create(frame);
		lv_obj_set_layout(statsRow, LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(statsRow, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(statsRow, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
		lv_obj_set_style_pad_column(statsRow, 4, 0);
	}

	startingStats = statsManager->get();
	startLevel = statsManager->getLevel();

	oil = new StatSprite(statsRows[0], StatSprite::Type::Oil, startingStats.oilLevel);
	lv_obj_t* oilLabel = lv_label_create(statsRows[0]);
	lv_label_set_text_fmt(oilLabel, "+%d", statsIncrease.oilLevel);
	lv_obj_set_style_text_color(oilLabel, lv_color_black(), 0);
	lv_obj_set_size(statsRows[0], LV_SIZE_CONTENT, LV_SIZE_CONTENT);

	happiness = new StatSprite(statsRows[1], StatSprite::Type::Happiness, startingStats.happiness);
	lv_obj_t* happinessLabel = lv_label_create(statsRows[1]);
	lv_label_set_text_fmt(happinessLabel, "+%d", statsIncrease.happiness);
	lv_obj_set_style_text_color(happinessLabel, lv_color_black(), 0);
	lv_obj_set_size(statsRows[1], LV_SIZE_CONTENT, LV_SIZE_CONTENT);

	xp = new StatSprite(statsRows[2], StatSprite::Type::XP, statsManager->getExpPercentage(), true);
	lv_obj_t* xpLabel = lv_label_create(statsRows[2]);
	lv_label_set_text_fmt(xpLabel, "+%d", statsIncrease.experience);
	lv_obj_set_style_text_color(xpLabel, lv_color_black(), 0);
	lv_obj_set_size(statsRows[2], LV_SIZE_CONTENT, LV_SIZE_CONTENT);

	exitTimer = lv_timer_create([](lv_timer_t* t){
		auto scr = (ScoreScreen*) t->user_data;
		scr->done();
	}, ExitTimeout, this);
	lv_timer_pause(exitTimer);

	lv_obj_t* clickObj = lv_obj_create(*this);
	lv_obj_set_size(clickObj, 0, 0);
	lv_obj_add_flag(clickObj, LV_OBJ_FLAG_FLOATING);
	lv_group_add_obj(inputGroup, clickObj);
	lv_group_set_editing(inputGroup, true);

	lv_obj_add_event_cb(clickObj, [](lv_event_t* e){
		auto scr = (ScoreScreen*) lv_event_get_user_data(e);
		scr->click();
	}, LV_EVENT_KEY, this);
}

ScoreScreen::~ScoreScreen(){
	lv_anim_delete(xp, nullptr);
	lv_timer_delete(exitTimer);
}

void ScoreScreen::onStart(){
	oil->set(startingStats.oilLevel + statsIncrease.oilLevel);
	happiness->set(startingStats.happiness + statsIncrease.happiness);

	uint8_t levelDifference = statsManager->getLevel(startingStats.experience + statsIncrease.experience) - statsManager->getLevel() + 1;

	lv_anim_init(&xpAnim);
	lv_anim_set_user_data(&xpAnim, this);
	lv_anim_set_var(&xpAnim, xp);
	lv_anim_set_values(&xpAnim, startingStats.experience, startingStats.experience + statsIncrease.experience);
	lv_anim_set_duration(&xpAnim, levelDifference * XPAnimDurationPerLevel);
	lv_anim_set_exec_cb(&xpAnim, [](void* var, int32_t v){
		auto bar = (StatSprite*) var;
		auto val = StatsManager::getExpPercentage(v);

		bar->set(val);
	});
	lv_anim_set_completed_cb(&xpAnim, [](lv_anim_t* anim){
		auto screen = (ScoreScreen*) anim->user_data;
		lv_timer_resume(screen->exitTimer);
	});
	lv_anim_set_path_cb(&xpAnim, lv_anim_path_linear);
	lv_anim_start(&xpAnim);
}

void ScoreScreen::click(){
	if(lv_anim_count_running()){
		lv_anim_delete_all();

		oil->set(startingStats.oilLevel + statsIncrease.oilLevel, false);
		happiness->set(startingStats.happiness + statsIncrease.happiness, false);
		xp->set(StatsManager::getExpPercentage(startingStats.experience + statsIncrease.experience), false);

		lv_timer_resume(exitTimer);
	}else{
		done();
	}
}

void ScoreScreen::done(){
	lv_timer_pause(exitTimer);

	statsManager->update(statsIncrease);
	const uint8_t currentLevel = statsManager->getLevel();

	if(currentLevel == startLevel){
		transition([](){ return std::make_unique<PetScreen>(); });
	}else{
		transition([currentLevel](){ return std::make_unique<LevelUpScreen>(currentLevel); }, LV_SCR_LOAD_ANIM_FADE_IN);
	}
}
