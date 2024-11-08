#ifndef BIT_FIRMWARE_EVENTS_H
#define BIT_FIRMWARE_EVENTS_H

#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <freertos/queue.h>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

enum class Facility { Input, Battery, Stats };

struct Event {
	Facility facility;
	void* data;
};

class EventQueue;
class Events {
public:
	static void listen(Facility facility, EventQueue* queue);
	static void unlisten(EventQueue* queue);

	static void post(Facility facility, const void* data, size_t size);

	template <typename T>
	static void post(Facility facility, const T& data){
		post(facility, &data, sizeof(T));
	}

	template <typename T>
	static void post(Facility facility, const T* data){
		post(facility, data, sizeof(T));
	}

private:
	static std::unordered_map<Facility, std::unordered_set<EventQueue*>> queues;
	static std::mutex mut;

};

class EventQueue {
public:
	EventQueue(size_t count);
	virtual ~EventQueue();

	bool get(Event& item, TickType_t timeout);
	void reset();

private:
	QueueHandle_t queue;

	bool post(Facility facility, void* data);
	friend Events;

};


#endif //BIT_FIRMWARE_EVENTS_H
