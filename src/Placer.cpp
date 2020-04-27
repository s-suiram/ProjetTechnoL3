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
    auto parent = widget->getParent();
    auto parent_content_rect = parent->getContentRect();
    place.top_left =
            parent_content_rect->top_left + parent_content_rect->size * Size(m_rel_x, m_rel_y) + Point(m_x, m_y);
    if (m_rel_width != 0.0 && m_rel_height == 0.0)
        place.size = parent_content_rect->size.width() * m_rel_width,
                place.size.height() = m_height;
    else if (m_rel_height != 0.0 && m_rel_width == 0.0)
        place.size = parent_content_rect->size.height() * m_rel_height,
                place.size.width() = m_width;
    else if (m_rel_width != 0.0 && m_rel_height != 0.0)
        place.size = parent_content_rect->size * Size(m_rel_width, m_rel_height);
    else
        place.size = Size(m_width, m_height);
    
    switch (m_anchor) {
        case ei_anc_north:
            place.top_left.x() -= place.size.width() / 2.0;
            break;
        case ei_anc_center:
            place.top_left -= place.size / 2;
            break;
        case ei_anc_northeast:
            place.top_left.x() -= place.size.width();
            break;
        case ei_anc_east:
            place.top_left -= Size(place.size.width(), place.size.height() / 2.0);
            break;
        case ei_anc_southeast:
            place.top_left -= place.size;
            break;
        case ei_anc_south:
            place.top_left -= Size(place.size.width() / 2.0, place.size.height());
            break;
        case ei_anc_southwest:
            place.top_left.y() -= place.size.height();
            break;
        case ei_anc_west:
            place.top_left.y() -= place.size.height() / 2.0;
            break;
        default:
            break;
    }
    widget->geomnotify(place);
    for (auto &child: widget->getChildren()) {
        child->get_geom_manager()->run(child);
    }
}

void Placer::release (Widget *widget) {
    if (widget->get_geom_manager() == nullptr) return;
    widget->set_geometry_manager(nullptr);
    widget->geomnotify(Rect(Point(0, 0), Size(0, 0)));
}
