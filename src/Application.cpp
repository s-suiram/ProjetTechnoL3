#include <ei_eventmanager.h>

#include "ei_application.h"
#include "ei_types.h"
#include "hw_interface.h"

using namespace ei;

Application *Application::instance = nullptr;
font_t ei::default_font;

Application::Application(Size *main_window_size) {
  if (instance != nullptr) {
    printf("A application instance already exists\n");
    exit(EXIT_FAILURE);
  }
  hw_init();
  window = hw_create_window(main_window_size, EI_FALSE);
  instance = this;
  picking_surface = hw_surface_create(window, main_window_size);
  root = new Frame(nullptr);
  root->geomnotify(Rect(Point(0, 0), *main_window_size));
  default_font = hw_text_font_create(default_font_filename, font_default_size);
}

Application::~Application() {
  delete root;
  hw_surface_free(picking_surface);
  hw_quit();
}

void Application::run() {
  double time = hw_now();
  while (!should_quit) {
    for (auto &c : root->getChildren()) {
      c->run_geom();
    }
    if (hw_now() - time > 1.0 / 60.0) {
      hw_surface_lock(picking_surface);
      hw_surface_lock(root_surface());
      root_widget()->draw(root_surface(), picking_surface, nullptr);
      hw_surface_unlock(root_surface());
      hw_surface_unlock(picking_surface);
      hw_surface_update_rects(update_rects);
      update_rects.clear();
      time = hw_now();
    }
    auto e = hw_event_wait_next();
    EventManager::getInstance().dispatch(e);
  }
}

void Application::invalidate_rect(const Rect &rect) {
  update_rects.push_front(Rect(rect));
}

void Application::quit_request() { should_quit = true; }

surface_t Application::root_surface() { return window; }

Frame *Application::root_widget() { return root; }

static uint32_t colorToPacked(color_t col) {
  return 0x0000u | col.red << 16 | col.green << 8 | col.blue;
}

Widget *Application::widget_pick(const Point &where) {
  color_t col_id = hw_get_pixel(picking_surface, where);
  uint32_t id = colorToPacked(col_id);
  return root_widget()->pick(id);
}
