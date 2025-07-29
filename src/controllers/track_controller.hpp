#pragma once
#include "glib.h"
#include "glibmm/refptr.h"
#include "gst/gstbus.h"
#include "gst/gstelement.h"
#include "gst/gstelementfactory.h"
#include "gst/gstpipeline.h"
#include <gst/gst.h>
#include <vector>

#include <gtkmm.h>

// Forward declaration to solve circular dependency between
// views/track_list_pane
class track_controller;

#include "../views/track_list_pane.hpp"

class track_controller {
public:
  std::vector<std::string> column_path;
  track_controller(track_list_pane *track_list_view);
  bool playing_state = false;
  bool paused_state = true;
  bool stopped_state = true;

  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;
  void select_track(int id) {}
  void play();
  void stop();
  void clear() {}

  void on_changed_volume();
  void add_track(const std::string &path);
  struct player {
    GstElement *pipeline, *source, *sink, *convert, *resample, *volume;
    gboolean playing, terminate, seek_enabled, seek_done;

    gint64 duration;
    gint64 current;
  };

  player elements;
  static void handle_message(player *data, GstMessage *msg);

private:
  track_list_pane *track_list_view;
  int playing_track_index = -1;
  static void pad_added_handler(GstElement *src, GstPad *pad, player *data);
  void update_playing_buttons();
  void update_album_cover() {}  
  void on_track_selected();
  void on_column_selected(guint pos);
  std::string get_path_of_column(int index);
  void test(bool testi);
  void playing_state_label();
};
