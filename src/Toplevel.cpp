#include "ei_widget.h"

using namespace ei;

Toplevel::Toplevel (Widget *parent) : Widget("toplevel", parent) {}

Toplevel::~Toplevel () {}

void Toplevel::configure (Size *requested_size, color_t *color, int *border_width, const char **title, bool_t *closable,
                          axis_set_t *resizable, Size *min_size) {
    m_requested_size = requested_size == nullptr ? Size(320, 240) : *requested_size;
    m_color = color == nullptr ? ei::default_background_color : *color;
    m_border_width = border_width == nullptr ? 4 : *border_width;
    m_title = title == nullptr ? "Toplevel" : *title;
    m_closable = closable == nullptr ? EI_TRUE : *closable;
    m_resizable = resizable == nullptr ? ei_axis_both : *resizable;
    m_min_size = min_size == nullptr ? Size(160, 120) : *min_size;
}

void Toplevel::draw (surface_t surface, surface_t pick_surface, Rect *clipper) {

}