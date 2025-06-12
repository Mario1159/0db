#pragma once

#include <gtkmm.h>

#include "../controllers/file_controller.hpp"
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

  void on_volume_changed_sync_volume_level_label();

  Glib::RefPtr<Gtk::Application> app;
  Glib::RefPtr<Gtk::Builder> builder;

  Gtk::Window *main_window;
  Gtk::Scale *slider;

  file_controller file_control;

  std::unique_ptr<file_list_pane> file_list_view;
  std::unique_ptr<track_list_pane> track_list_view;
};
