#ifndef __FILE2__
#define __FILE2__

#include <memory>
#include <string>
#include <functional>
#include <map>

class tag_EventHandlerID;

using EventHandlerID = std::shared_ptr<tag_EventHandlerID>;

using EventParams = std::map<std::string, std::string>;

using EventHandlerCB = std::function<void(const std::string&/* Event*/, const EventParams& /*Params*/)>;

EventHandlerID register_event(const std::string& event, EventHandlerCB cb);
void discard_event(const EventHandlerID& cb);
void discard_event(const std::string& event);

void wait_event(const std::string& event, EventHandlerCB cb);
void signal_event(const std::string& event, const EventParams& params);

void run_on_background(std::function<void()> fn);

#endif //__FILE2__
