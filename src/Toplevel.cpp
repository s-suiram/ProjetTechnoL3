#include <ei_eventmanager.h>
#include "ei_widget.h"

using namespace ei;

surface_t Toplevel::cross_tex = nullptr;
Point Toplevel::resize_square_size = Point(10, 10);

Toplevel::Toplevel (Widget *parent) : Widget("toplevel", parent) {
    if (cross_tex == nullptr) {
        Toplevel::cross_tex = hw_image_load(DATA_DIR"cross.png");
    }
    
    m_title_dragging = false;
    m_last_mouse_pos = Point(0, 0);
    
    EventManager::getInstance().bind(ei_ev_mouse_buttonup, this, "", close_callback, nullptr);
    EventManager::getInstance().bind(ei_ev_mouse_buttondown, this, "", title_press_callback, nullptr);
    EventManager::getInstance().bind(ei_ev_mouse_move, nullptr, "all", mouse_move_callback, this);
    EventManager::getInstance().bind(ei_ev_mouse_buttonup, nullptr, "all", release_callback, this);
}

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
    color_t black = {0x00, 0x00, 0x00, 0xff};
    
    
    // Draw frame
    linked_point_t points;
    points.push_back(screen_location.top_left);
    points.push_back(screen_location.top_left + Size(screen_location.size.width(), 0));
    points.push_back(screen_location.top_left + screen_location.size);
    points.push_back(screen_location.top_left + Size(0, screen_location.size.height()));
    
    draw_polygon(surface, points, m_color, clipper);
    draw_polygon(pick_surface, points, pick_color, clipper);
    
    // Draw title bar
    
    Rect title_bar = title_bar_rect();
    
    points.clear();
    points.push_back(title_bar.top_left);
    points.push_back(title_bar.top_left + Size(title_bar.size.width(), 0));
    points.push_back(title_bar.top_left + title_bar.size);
    points.push_back(title_bar.top_left + Size(0, title_bar.size.height()));
    
    draw_polygon(surface, points, {0xff, 0xff, 0xff, 0xff}, clipper);
    draw_polygon(pick_surface, points, pick_color, clipper);
    
    const Rect cross = cross_rect();
    ei_copy_surface(surface, cross_tex, &cross.top_left, EI_TRUE);
    
    surface_t title_surface = hw_text_create_surface(m_title, default_font, &black);
    
    ei_copy_surface(surface, title_surface, &title_bar.top_left, EI_TRUE);
    
    hw_surface_free(title_surface);
    
    // Draw left border
    points.clear();
    points.push_back(title_bar.top_left);
    points.push_back(title_bar.top_left + Size(-m_border_width, 0));
    points.push_back(
            screen_location.top_left + Size(-m_border_width, screen_location.size.height() + (float) m_border_width));
    points.push_back(screen_location.top_left + Size(0, screen_location.size.height() + (float) m_border_width));
    
    draw_polygon(surface, points, black, clipper);
    
    // Draw right border
    
    for (Point &p: points) {
        p.x() += screen_location.size.width() + (float) m_border_width;
    }
    
    draw_polygon(surface, points, black, clipper);
    
    // Draw down border
    
    points.clear();
    points.push_back(screen_location.top_left + Size(0, screen_location.size.height()));
    points.push_back(screen_location.top_left + Size(0, screen_location.size.height() + (float) m_border_width));
    points.push_back(screen_location.top_left +
                     Size(screen_location.size.width(), screen_location.size.height() + (float) m_border_width));
    points.push_back(screen_location.top_left + Size(screen_location.size.width(), screen_location.size.height()));
    
    draw_polygon(surface, points, black, clipper);
    
    // Draw up border
    
    for (Point &p: points) {
        p.y() -= screen_location.size.height() + title_bar.size.height();
    }
    draw_polygon(surface, points, black, clipper);
    
    Widget::draw(surface, pick_surface, clipper);
    
    // Resize square
    
    if (m_resizable != ei_axis_none) {
        points.clear();
        points.push_back(screen_location.top_left + screen_location.size);
        points.push_back(screen_location.top_left +
                         Size(screen_location.size.width() - resize_square_size.width(),
                              screen_location.size.height()));
        points.push_back(screen_location.top_left + (screen_location.size - resize_square_size));
        points.push_back(screen_location.top_left +
                         Size(screen_location.size.width(),
                              screen_location.size.height() - resize_square_size.width()));
        
        draw_polygon(surface, points, black, clipper);
    }
}

Rect Toplevel::title_bar_rect () {
    Size cross_size = hw_surface_get_size(cross_tex);
    Point p = screen_location.top_left + Point(0, -cross_size.height());
    Size s;
    s.width() = screen_location.size.width();
    s.height() = cross_size.height() + 1;
    return Rect(p, s);
}

Rect Toplevel::cross_rect () {
    Size cross_size = hw_surface_get_size(cross_tex);
    Point p(screen_location.top_left.x() + screen_location.size.width() - cross_size.width(),
            screen_location.top_left.y() - cross_size.height());
    return Rect(p, cross_size);
}

bool_t Toplevel::close_callback (Widget *widget, Event *event, void *user_param) {
    auto *mouseEvent = (MouseEvent *) event;
    auto *tl = (Toplevel *) widget;
    Rect cross = tl->cross_rect();
    if (tl->m_closable && cross.contains(mouseEvent->where)) {
        tl->getParent()->remove_child(tl);
        return EI_TRUE;
    }
    return EI_FALSE;
}

bool_t Toplevel::title_press_callback (Widget *w, Event *e, void *user_param) {
    auto tl = (Toplevel *) w;
    auto mouseEvent = (MouseEvent *) e;
    auto title_bar_rect = tl->title_bar_rect();
    auto resize_square = Rect(tl->screen_location.top_left + tl->screen_location.size - resize_square_size,
                              resize_square_size);
    if (title_bar_rect.contains(mouseEvent->where)) {
        tl->m_title_dragging = true;
    } else if (resize_square.contains(mouseEvent->where)) {
        tl->m_resize_drag = true;
    }
    return EI_TRUE;
}

bool_t Toplevel::release_callback (Widget *w, Event *e, void *user_param) {
    auto tl = (Toplevel *) user_param;
    tl->m_title_dragging = false;
    tl->m_resize_drag = false;
    return EI_TRUE;
}

bool_t Toplevel::mouse_move_callback (Widget *w, Event *e, void *user_param) {
    auto tl = (Toplevel *) user_param;
    auto mouseEvent = (MouseEvent *) e;
    if (tl->m_title_dragging) {
        auto new_pos = tl->screen_location.top_left - (tl->m_last_mouse_pos - mouseEvent->where);
        ((Placer *) tl->get_geom_manager())->configure(tl, nullptr, &new_pos.x(), &new_pos.y(),
                                                       nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    } else if (tl->m_resize_drag) {
        if (tl->m_resizable == ei_axis_none) return EI_FALSE;
        auto new_size = tl->screen_location.size - (tl->m_last_mouse_pos - mouseEvent->where);
        if (tl->m_resizable == ei_axis_x) new_size.height() = tl->screen_location.size.height();
        if (tl->m_resizable == ei_axis_y) new_size.width() = tl->screen_location.size.width();
        
        if (new_size.width() < tl->m_min_size.width()) new_size.width() = tl->m_min_size.width();
        if (new_size.height() < tl->m_min_size.height()) new_size.height() = tl->m_min_size.height();
        ((Placer *) tl->get_geom_manager())->configure(tl, nullptr, nullptr, nullptr, &new_size.width(),
                                                       &new_size.height(),
                                                       nullptr, nullptr, nullptr, nullptr);
    }
    tl->m_last_mouse_pos = mouseEvent->where;
    return EI_TRUE;
}
