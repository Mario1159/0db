#include <gtkmm.h>
#include <memory>

#include "views/main_window_view.hpp"

Glib::RefPtr<Gtk::Application> app;
Glib::RefPtr<Gtk::Builder> builder;
std::unique_ptr<main_window_view> main_window;

void on_app_activate() {
  builder = Gtk::Builder::create();
  main_window = std::make_unique<main_window_view>(app, builder);
  main_window->show();
}

int main(int argc, char **argv) {
  app = Gtk::Application::create("org.open0db.zerodb");
  app->signal_activate().connect(sigc::ptr_fun(on_app_activate));
  return app->run(argc, argv);
}
