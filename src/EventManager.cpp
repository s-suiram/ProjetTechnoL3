#include <ei_application.h>
#include "ei_eventmanager.h"

using namespace ei;

EventManager::EventManager () {

}

void
EventManager::bind (ei_eventtype_t eventtype, Widget *widget, tag_t tag, ei_callback_t callback, void *user_param) {
    EventCallbackSettings settings;
    settings.type = eventtype;
    settings.widget = widget;
    settings.tag = tag;
    settings.callback = callback;
    settings.user_param = user_param;
    callbacks.push_back(settings);
}

void
EventManager::unbind (ei_eventtype_t eventtype, Widget *widget, tag_t tag, ei_callback_t callback, void *user_param) {
    EventCallbackSettings settings;
    settings.type = eventtype;
    settings.widget = widget;
    settings.tag = tag;
    settings.callback = callback;
    settings.user_param = user_param;
    callbacks.remove(settings);
}

void EventManager::dispatch (Event *e) {
    for (auto &s : callbacks) {
        bool_t consumed = EI_FALSE;
        if (s.type != e->type) continue;
        
        if (s.tag == "all") consumed = s.callback(nullptr, e, s.user_param);
        else if (e->type >= ei_ev_mouse_buttondown) { // all event that need picking
            Point p;
            if (e->type >= ei_ev_touch_begin) {
                p = ((TouchEvent *) e)->where;
            } else {
                p = ((MouseEvent *) e)->where;
            }
            Widget *picked = Application::getInstance()->widget_pick(p);
            if (picked == s.widget) consumed = s.callback(s.widget, e, s.user_param);
        }
        
        if (consumed == EI_TRUE) return;
    }
}

bool EventCallbackSettings::operator== (EventCallbackSettings const &rhs) const {
    return type == rhs.type
           && tag == rhs.tag
           && user_param == rhs.user_param
           && widget == rhs.widget;
}
