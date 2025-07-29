#pragma once

#include <gtkmm.h>

#include "../views/file_list_pane.hpp"
#include "../views/track_list_pane.hpp"

#include <memory>

class main_window_view {

public:
  main_window_view(const Glib::RefPtr<Gtk::Application> &app,
                   const Glib::RefPtr<Gtk::Builder> &builder);

  void show();

private:
  void connect_menu_bar_model();
  void bind_signals_and_actions();

  Glib::RefPtr<Gtk::Application> app;
  Glib::RefPtr<Gtk::Builder> builder;

  Gtk::Window *main_window;
 
  std::unique_ptr<file_list_pane> file_list_view;
  std::shared_ptr<track_list_pane> track_list_view;
};
