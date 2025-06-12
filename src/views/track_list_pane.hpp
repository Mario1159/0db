#pragma once

#include <gtkmm.h>

#include <memory>

// Forward declaration to solve circular dependency between
// controllers/track_controller
class track_list_pane;

#include "../controllers/track_controller.hpp"
#include "../models/track_item.hpp"

class track_list_pane {
public:
  track_list_pane(const Glib::RefPtr<Gtk::Builder>& builder);
  void update();

private:
  Gtk::ColumnView *track_list_view;
  Glib::RefPtr<Gio::ListStore<track_item>> track_model;
  std::unique_ptr<track_controller> controller;
};
