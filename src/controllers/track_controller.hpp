#pragma once

#include <gtkmm.h>

#include "../models/track_item.hpp"

class track_controller {
public:
  static void
  update_track_list(Gtk::ColumnView *track_list_view,
                    Glib::RefPtr<Gio::ListStore<track_item>> track_model);
};
