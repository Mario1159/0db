#pragma once

#include <gtkmm.h>

#include "../controllers/track_controller.hpp"
#include "../models/track_item.hpp"

class track_list_pane {
public:
  track_list_pane(Glib::RefPtr<Gtk::Builder> builder);
  static Glib::RefPtr<Gio::ListModel>
  create_model(const Glib::RefPtr<Glib::ObjectBase> &item = {});

private:
  Gtk::ColumnView *track_list_view;
  Glib::RefPtr<Gio::ListStore<track_item>> track_model;
  track_controller controller;
};
