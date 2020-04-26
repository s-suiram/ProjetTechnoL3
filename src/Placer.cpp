#include "ei_geometrymanager.h"

using namespace ei;

GeometryManager::GeometryManager () {}

GeometryManager::~GeometryManager () {}

void Placer::configure (Widget *widget, anchor_t *anchor, int *x, int *y, float *width, float *height, float *rel_x,
                        float *rel_y, float *rel_width, float *rel_height) {
    if (widget->get_geom_manager() == nullptr) {
        widget->set_geometry_manager(this);
        goto default_val;
    } else {
        if (widget->get_geom_manager() != this) {
            widget->get_geom_manager()->release(widget);
            widget->set_geometry_manager(this);
            goto default_val;
        } else {
            m_anchor = anchor != nullptr ? *anchor : m_anchor;
            m_x = x != nullptr ? *x : m_x;
            m_y = y != nullptr ? *y : m_y;
            m_width = width != nullptr ? *width : m_width;
            m_height = height != nullptr ? *height : m_height;
            m_rel_x = rel_x != nullptr ? *rel_x : m_rel_x;
            m_rel_y = rel_y != nullptr ? *rel_y : m_rel_y;
            m_rel_width = rel_width != nullptr ? *rel_width : m_rel_width;
            m_rel_height = rel_height != nullptr ? *rel_height : m_rel_height;
            return;
        }
    }
    
    default_val:
    m_anchor = anchor != nullptr ? *anchor : ei_anc_northwest;
    m_x = x != nullptr ? *x : 0;
    m_y = y != nullptr ? *y : 0;
    m_width = width != nullptr ? *width : widget->get_requested_size()->width();
    m_height = height != nullptr ? *height : widget->get_requested_size()->height();
    m_rel_x = rel_x != nullptr ? *rel_x : 0.0f;
    m_rel_y = rel_y != nullptr ? *rel_y : 0.0f;
    m_rel_width = rel_width != nullptr ? *rel_width : 0.0f;
    m_rel_height = rel_height != nullptr ? *rel_height : 0.0f;
}

void Placer::run (Widget *widget) {
    Rect place;
    place.top_left = Point(m_x, m_y);
    place.size = Size(m_width, m_height);
    widget->geomnotify(place);
}

void Placer::release (Widget *widget) {
    if (widget->get_geom_manager() == nullptr) return;
    widget->set_geometry_manager(nullptr);
    widget->geomnotify(Rect(Point(0, 0), Size(0, 0)));
}
