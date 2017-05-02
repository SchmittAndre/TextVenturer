#pragma once

// Functions
typedef void(*EventFuncNotify)(void* self, void* sender);

// Generic
template <typename EventFunc>
struct GenericEventFunc
{
    bool operator==(const GenericEventFunc<EventFunc> & other) const
    {
        return self == other.self && func == other.func;
    }

    void* self;
    EventFunc func;
};

template <typename EventFunc>
struct EventHasher
{
    size_t operator() (const GenericEventFunc<EventFunc> & f) const
    {
        return reinterpret_cast<size_t>(f.self) ^ reinterpret_cast<size_t>(f.func);
    }
};

template <typename EventFunc>
using BaseEvent = std::unordered_set<GenericEventFunc<EventFunc>, EventHasher<EventFunc>>;

// Typed
typedef BaseEvent<EventFuncNotify> NotifyEvent;