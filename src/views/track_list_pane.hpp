#pragma once

#include "gtkmm/label.h"
#include "gtkmm/progressbar.h"
#include "sigc++/connection.h"
#include <gtkmm.h>

#include <memory>

// Forward declaration to solve circular dependency between
// controllers/track_controller
class track_list_pane;

#include "../controllers/track_controller.hpp"
#include "../models/track_item.hpp"

class track_list_pane {
public:
  Gtk::ColumnView *track_list_view;
  Glib::RefPtr<Gio::ListStore<track_item>> track_model;
  std::shared_ptr<track_controller> controller;
  track_list_pane(const Glib::RefPtr<Gtk::Builder> &builder);
  Gtk::Scale *slider;

  Gtk::Button *play_button;
  Gtk::Button *stop_button;
  Gtk::Label *playing_button;

  void update();

private:
  Gtk::Label *progress_label;
  Gtk::Label *volume_label;
  Gtk::ProgressBar *progress_bar;
  sigc::connection progress_bar_timeout;
  sigc::connection messages_timeout;
  void on_volume_changed_sync_volume_level_label();
  void progress_bar_position();
  bool progress_bar_pos_timeout();
  bool msg_timeout();
};
