//
// Created by hector on 11/20/23.
//

#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include <list>
#include <map>
#include <typeindex>
#include <memory>
#include <functional>

#include "../logger/logger.h"
#include "event.h"

class EventCallbackBase {
private:
    virtual void Call(Event& e) = 0;

public:
    virtual ~EventCallbackBase() = default;
    void Execute(Event& e) {
        Call(e);
    }
};

template<typename TOwner, typename TEvent>
class EventCallback : public EventCallbackBase {
private:
    typedef void (TOwner::*CallbackFunction)(TEvent&);

    TOwner* ownerInstance;
    CallbackFunction callbackFunction;

    void Call(Event& e) override {
        std::invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
    }

public:
    EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction) : ownerInstance(ownerInstance),
                                                                              callbackFunction(callbackFunction) {
        this->ownerInstance = ownerInstance;
        this->callbackFunction = callbackFunction;
    };
    ~EventCallback() override = default;
};

typedef std::list<std::unique_ptr<EventCallbackBase>> HandlerList;

class EventBus {
private:
    std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

public:
    EventBus() {
        Logger::Log("Event bus created");
    }

    ~EventBus() {
        Logger::Log("Event bus destroyed");
    }

    // clears the subs list
    void Reset() {
        this->subscribers.clear();
    }

    template<typename TEvent, typename TOwner>
    void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::*callbackFunction)(TEvent&)) {
        if (!subscribers[typeid(TEvent)].get()) {
            subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
        }
        auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
        subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
    }

    template<typename TEvent, typename... TArgs>
    void EmitEvent(TArgs&&... args) {
        if (const auto handlers = subscribers[typeid(TEvent)].get()) {
            // for (auto handler: *handlers) {
            for (auto it = handlers->begin(); it != handlers->end(); ++it) {
                auto handler = it->get();
                TEvent event(std::forward<TArgs>(args)...);
                handler->Execute(event);
            }
        }
    }
};

#endif //EVENT_BUS_H
