#include <gtkmm.h>

#include "views/main_window_view.hpp"

Glib::RefPtr<Gtk::Application> app;

void on_app_activate() {
  Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();
  main_window_view main_window(app, builder);
  main_window.show();
}

int main(int argc, char **argv) {
  app = Gtk::Application::create("org.open0db.zerodb");
  app->signal_activate().connect([]() { on_app_activate(); });
  return app->run(argc, argv);
}
