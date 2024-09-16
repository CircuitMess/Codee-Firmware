#ifndef BIT_FIRMWARE_MELODYPLAYER_H
#define BIT_FIRMWARE_MELODYPLAYER_H

#include <cstdint>
#include <vector>
#include <initializer_list>
#include <memory>
#include "Util/Threaded.h"
#include "ChirpSystem.h"

struct Tone {
	float beats = 0;
	enum class Type { None, Flat, Slide } type;
protected:
	Tone(float beats, Type type) : beats(beats), type(type){}
};

struct NoneTone : public Tone {
	NoneTone(float beats) : Tone(beats, Tone::Type::None){ }
};

struct FlatTone : public Tone {
	FlatTone(uint16_t freq, float beats) : Tone(beats, Type::Flat), freq(freq){ }
	uint16_t freq = 0;
};

struct SlideTone : public Tone {
	SlideTone(uint16_t start, uint16_t stop, float beats) : Tone(beats, Type::Slide), start(start), stop(stop){ }
	uint16_t start = 0;
	uint16_t stop = 0;
};

extern std::shared_ptr<Tone> toneNone(float beats);
extern std::shared_ptr<Tone> toneFlat(uint16_t freq, float beats);
extern std::shared_ptr<Tone> toneSlide(uint16_t start, uint16_t stop, float beats);

class MelodyPlayer : private Threaded {
public:
	MelodyPlayer(uint8_t bpm, std::vector<std::shared_ptr<Tone>> tones);
	~MelodyPlayer() override;

	void play();
	bool isPlaying();

private:
	ChirpSystem* audio;

	float beatDuration; // [ms]
	std::vector<std::shared_ptr<Tone>> tones;

	void loop() override;

	std::vector<Chirp> toChirps(const std::vector<std::shared_ptr<Tone>>& tones);

};


#endif //BIT_FIRMWARE_MELODYPLAYER_H
