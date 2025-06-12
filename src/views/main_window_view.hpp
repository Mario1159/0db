#pragma once

#include <gtkmm.h>

#include "../controllers/file_controller.hpp"
#include "../views/file_list_pane.hpp"
#include "../views/track_list_pane.hpp"

#include <memory>

class main_window_view {

public:
  main_window_view(Glib::RefPtr<Gtk::Application> app,
                   Glib::RefPtr<Gtk::Builder> builder);

  void show();

private:
  Glib::RefPtr<Gtk::Application> app;
  Gtk::Window *main_window;
  file_controller file_control;

  std::unique_ptr<file_list_pane> file_list_view;
  std::unique_ptr<track_list_pane> track_list_view;
};
