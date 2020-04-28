#include <ei_eventmanager.h>
#include "ei_widget.h"

using namespace ei;

Button::Button (Widget *parent) : Widget("button", parent), m_clicked(false) {
    EventManager::getInstance().bind(ei_ev_mouse_buttondown, this, "", mouse_down_callback, nullptr);
    EventManager::getInstance().bind(ei_ev_mouse_buttonup, this, "", mouse_up_callback, nullptr);
}


Button::~Button () {
    EventManager::getInstance().unbind(ei_ev_mouse_buttondown, this, "", mouse_down_callback, nullptr);
    EventManager::getInstance().unbind(ei_ev_mouse_buttonup, this, "", mouse_up_callback, nullptr);
}

void
Button::configure (Size *requested_size, const color_t *color, int *border_width, int *corner_radius, relief_t *relief,
                   const char **text, font_t *text_font, color_t *text_color, anchor_t *text_anchor, surface_t *img,
                   Rect **img_rect, anchor_t *img_anchor) {
    
    m_color = color == nullptr ? ei::default_background_color : *color;
    m_border_width = border_width == nullptr ? ei::default_button_border_width : *border_width;
    m_corner_radius = corner_radius == nullptr ? ei::default_button_corner_radius : *corner_radius;
    m_relief = relief == nullptr ? ei_relief_raised : *relief;
    m_text = text == nullptr ? nullptr : *text;
    m_text_font = text_font == nullptr ? ei::default_font : *text_font;
    m_text_color = text_color == nullptr ? ei::font_default_color : *text_color;
    m_text_anchor = text_anchor == nullptr ? ei_anc_center : *text_anchor;
    m_img = img == nullptr ? nullptr : *img;
    m_img_rect = img_rect == nullptr ? nullptr : *img_rect;
    m_img_anchor = img_anchor == nullptr ? ei_anc_center : *img_anchor;
    
    if (requested_size == nullptr) {
        if (img != nullptr) {
            m_requested_size = hw_surface_get_size(img);
        } else if (text != nullptr && text_font != nullptr) {
            Size text_size;
            hw_text_compute_size(m_text, m_text_font, text_size);
            m_requested_size = text_size;
        } else {
            m_requested_size = Size(0, 0);
        }
    } else {
        m_requested_size = *requested_size;
    }
}

static color_t darken (color_t c) {
    c.red -= 20;
    c.green -= 20;
    c.blue -= 20;
    return c;
}

void Button::draw (surface_t surface, surface_t pick_surface, Rect *clipper) {
    linked_point_t points;
    points.push_back(screen_location.top_left);
    points.push_back(screen_location.top_left + Size(screen_location.size.x(), 0));
    points.push_back(screen_location.top_left + screen_location.size);
    points.push_back(screen_location.top_left + Size(0, screen_location.size.y()));
    
    color_t final_color;
    
    if (m_clicked) {
        final_color = darken(m_color);
    } else {
        final_color = m_color;
    }
    
    draw_polygon(surface, points, final_color, clipper);
    draw_polygon(pick_surface, points, pick_color, clipper);
    
    if (m_text != nullptr) {
        surface_t text_surface = hw_text_create_surface(m_text, m_text_font, &m_text_color);
        Point text_pos;
        Size text_size = hw_surface_get_size(text_surface);
        text_pos.x() = screen_location.top_left.x() + screen_location.size.width() / 2 - text_size.width() / 2;
        text_pos.y() = screen_location.top_left.y() + screen_location.size.height() / 2 - text_size.height() / 2;
        ei_copy_surface(surface, text_surface, &text_pos, EI_TRUE);
        hw_surface_free(text_surface);
    } else if (m_img != nullptr) {
        printf("implement button image\n");
    }
    
    Widget::draw(surface, pick_surface, clipper);
}

bool_t Button::mouse_down_callback (Widget *w, Event *e, void *user_param) {
    auto b = (Button *) w;
    b->m_clicked = true;
    return EI_FALSE;
}

bool_t Button::mouse_up_callback (Widget *w, Event *e, void *user_param) {
    auto b = (Button *) w;
    b->m_clicked = false;
    return EI_FALSE;
}
