#include "ei_widget.h"
#include "ei_application.h"

using namespace ei;

uint32_t Widget::s_idGenerator = 0;

// packed is RGB888 format
static color_t packedToColor (uint32_t packed) {
    color_t c;
    c.red = packed >> 16;
    c.green = packed >> 8;
    c.blue = packed;
    c.alpha = 0xff;
    return c;
}

Widget::Widget (const widgetclass_name_t &class_name, Widget *p) : name(class_name), parent(p),
                                                                   pick_id(s_idGenerator++),
                                                                   pick_color(packedToColor(pick_id)),
                                                                   content_rect(&screen_location),
                                                                   geom_manager(nullptr),
                                                                   m_requested_size() {
    if (p != nullptr) p->children.push_back(this);
}

Widget::~Widget () {
}

void Widget::draw (surface_t surface, surface_t pick_surface, Rect *clipper) {
    for (auto &c: children) {
        c->draw(surface, pick_surface, clipper);
    }
}

void Widget::geomnotify (Rect rect) {
    screen_location = rect;
    Application::getInstance()->invalidate_rect(rect);
}

Widget *Widget::pick (uint32_t id) {
    if (getPick_id() == id) return this;
    
    for (auto &child: children) {
        if (child->getPick_id() == id) return child;
    }
    
    return nullptr;
}

uint32_t Widget::getPick_id () const {
    return pick_id;
}

Widget *Widget::getParent () const {
    return parent;
}

GeometryManager *Widget::get_geom_manager () {
    return geom_manager;
}

void Widget::set_geometry_manager (GeometryManager *g) {
    geom_manager = g;
}

Size *Widget::get_requested_size () {
    return &m_requested_size;
}

const std::list<Widget *> &Widget::getChildren () const {
    return children;
}

void Widget::run_geom () {
    if (get_geom_manager() != nullptr) {
        geom_manager->run(this);
    }
}

void Widget::remove_child (Widget *child) {
    children.remove(child);
}

Rect *Widget::getContentRect () const {
    return content_rect;
}

const widgetclass_name_t &Widget::getName () const {
    return name;
}
