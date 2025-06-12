#pragma once

#include <gtkmm.h>

// Forward declaration to solve circular dependency between
// views/track_list_pane
class track_controller;

#include "../views/track_list_pane.hpp"

class track_controller {
public:
  track_controller(track_list_pane *track_list_view);

  void select_track(int id) {}
  void play() {}
  void stop() {}
  void clear() {}

  void on_changed_volume() {}

private:
  track_list_pane *track_list_view;
  int selected_track_id = -1;

  void update_album_cover() {}
};
