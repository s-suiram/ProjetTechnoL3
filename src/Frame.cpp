#include "ei_widget.h"

using namespace ei;

Frame::Frame (Widget *parent) : Widget("frame", parent) {}

Frame::~Frame () {

}

void Frame::draw (surface_t surface, surface_t pick_surface, Rect *clipper) {
    linked_point_t points;
    points.push_back(screen_location.top_left);
    points.push_back(screen_location.top_left + Size(screen_location.size.width(), 0));
    points.push_back(screen_location.top_left + screen_location.size);
    points.push_back(screen_location.top_left + Size(0, screen_location.size.height()));
    draw_polygon(surface, points, m_color, clipper);
    draw_polygon(pick_surface, points, pick_color, clipper);
    Widget::draw(surface, pick_surface, clipper);
}

void Frame::configure (Size *requested_size, const color_t *color, int *border_width, relief_t *relief, char **text,
                       font_t *text_font, color_t *text_color, anchor_t *text_anchor, surface_t *img, Rect **img_rect,
                       anchor_t *img_anchor) {
    m_color = color == nullptr ? ei::default_background_color : *color;
    m_border_width = border_width == nullptr ? 0 : *border_width;
    m_relief = relief == nullptr ? ei_relief_none : *relief;
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
