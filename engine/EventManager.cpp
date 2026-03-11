#include "EventManager.h"

std::unordered_map<EventType, std::vector<Listener*>> EventManager::listeners;

void EventManager::subscribe(EventType eventType, Listener* listener)
{
    listeners[eventType].push_back(listener);
}

void EventManager::unsubscribe(EventType eventType, Listener* listener)
{
    listeners[eventType].erase(std::find(listeners[eventType].begin(), listeners[eventType].end(), listener));
}

void EventManager::notify(EventType type, EventContext context)
{
    for (auto l : listeners[type]) {
        l->onNotify(type, context);
    }
}
