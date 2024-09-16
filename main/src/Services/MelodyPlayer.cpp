#include "MelodyPlayer.h"
#include "Util/Services.h"
#include "Util/stdafx.h"

std::shared_ptr<Tone> toneNone(float beats){ return std::make_shared<NoneTone>(beats); }
std::shared_ptr<Tone> toneFlat(uint16_t freq, float beats){ return std::make_shared<FlatTone>(freq, beats); }
std::shared_ptr<Tone> toneSlide(uint16_t start, uint16_t stop, float beats){ return std::make_shared<SlideTone>(start, stop, beats); }

MelodyPlayer::MelodyPlayer(uint8_t bpm, std::vector<std::shared_ptr<Tone>> tones) : Threaded("MelodyPlayer", 2 * 1024, 12, 1), beatDuration((int)(1000.0f * 80.0f / (float) bpm)), tones(std::move(tones)){
	audio = (ChirpSystem*) Services.get(Service::Audio);
	beatDuration /= 5.0f;
}

MelodyPlayer::~MelodyPlayer(){
	stop();
}

void MelodyPlayer::play(){
	start();
}

bool MelodyPlayer::isPlaying(){
	return running();
}

void IRAM_ATTR MelodyPlayer::loop(){
	static const float MaxPlay = 2000;

	int played = 0;
	while(played < tones.size()){
		std::vector<std::shared_ptr<Tone>> queued;
		float queuedDuration = 0;
		for(;;){
			int i = played + queued.size();
			if(i >= tones.size() || (queuedDuration + tones[i]->beats * beatDuration) > MaxPlay) break;
			queuedDuration += tones[i]->beats * beatDuration;
			queued.push_back(tones[i]);
		}

		played += queued.size();

		const auto chirps = toChirps(queued);
		audio->play(chirps);

		if(queuedDuration < 100){
			queuedDuration = 0;
		}else{
			queuedDuration -= 100;
		}
		delayMillis((int) queuedDuration);
		while(audio->isPlaying()) delayMillis(1);
	}

	stop(0);
}

std::vector<Chirp> MelodyPlayer::toChirps(const std::vector<std::shared_ptr<Tone>>& tones){
	std::vector<Chirp> chirps;
	for(const auto& tone : tones){
		if(tone->type == Tone::Type::Flat){
			auto* flat = (FlatTone*) tone.get();
			chirps.push_back(Chirp { flat->freq, flat->freq, (uint16_t) (flat->beats * beatDuration) });
		}else if(tone->type == Tone::Type::Slide){
			auto* flat = (SlideTone*) tone.get();
			chirps.push_back(Chirp { flat->start, flat->stop, (uint16_t) (flat->beats * beatDuration) });
		}else if(tone->type == Tone::Type::None){
			auto* none = (NoneTone*) tone.get();
			chirps.push_back(Chirp { 0, 0, (uint16_t) (none->beats * beatDuration) });
		}
	}
	return chirps;
}
