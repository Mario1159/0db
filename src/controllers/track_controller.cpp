#include "track_controller.hpp"
#include "glib-object.h"
#include "glib.h"
#include "gst/gstelement.h"
#include "gst/gstelementfactory.h"
#include "gst/gstmessage.h"
#include "gst/gstpad.h"
#include "gst/gstpipeline.h"
#include "gst/gststructure.h"
#include "gst/gstutils.h"
#include "sigc++/functors/mem_fun.h"

#include <format>
#include <iostream>
#include <string>
#include <taglib/audioproperties.h>
#include <taglib/fileref.h>
#include <vector>

track_controller::track_controller(track_list_pane *track_list_view)
    : track_list_view(track_list_view) {

  elements.playing = false;
  elements.terminate = false;
  elements.seek_enabled = false;
  elements.seek_done = false;
  elements.duration = GST_CLOCK_TIME_NONE;

  elements.source = gst_element_factory_make("uridecodebin", "audio_src");
  elements.convert = gst_element_factory_make("audioconvert", "audio_convert");
  elements.resample =
      gst_element_factory_make("audioresample", "audio_resample");
  elements.sink = gst_element_factory_make("pulsesink", "audio_sink");
  elements.volume = gst_element_factory_make("volume", "volume");

  elements.pipeline = gst_pipeline_new("pipeline");
  if (!elements.pipeline || !elements.source || !elements.sink) {
    g_printerr("Couldn't create all elements");
  }

  gst_bin_add_many(GST_BIN(elements.pipeline), elements.source,
                   elements.convert, elements.resample, elements.sink,
                   elements.volume, nullptr);
  gst_element_link_many(elements.convert, elements.resample, elements.volume,
                        elements.sink, nullptr);
  g_signal_connect(elements.source, "pad-added", G_CALLBACK(pad_added_handler),
                   &elements);

  g_object_set(elements.volume, "volume", 0.5, nullptr);
  gst_element_set_state(elements.pipeline, GST_STATE_READY);

  bus = gst_element_get_bus(elements.pipeline);

  track_list_view->track_list_view->signal_activate().connect(
      sigc::mem_fun(*this, &track_controller::on_column_selected));
}

void track_controller::handle_message(player *data, GstMessage *msg) {
  GError *err;
  gchar *debug_info;

  switch (GST_MESSAGE_TYPE(msg)) {

  case GST_MESSAGE_ERROR:
    gst_message_parse_error(msg, &err, &debug_info);
    g_printerr("Error received from element %s: %s\n",
               GST_OBJECT_NAME(msg->src), err->message);
    g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
    g_clear_error(&err);
    g_free(debug_info);
    data->terminate = TRUE;
    break;
  case GST_MESSAGE_EOS:
    g_print("\nEnd-Of-Stream reached.\n");
    data->terminate = TRUE;
    break;
  case GST_MESSAGE_DURATION:
    /* The duration has changed, mark the current one as invalid */
    data->duration = GST_CLOCK_TIME_NONE;
    break;
  case GST_MESSAGE_STATE_CHANGED: {

    GstState old_state, new_state, pending_state;
    gst_message_parse_state_changed(msg, &old_state, &new_state,
                                    &pending_state);
    if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data->pipeline)) {
      g_print("Pipeline state changed from %s to %s:\n",
              gst_element_state_get_name(old_state),
              gst_element_state_get_name(new_state));

      data->playing = (new_state == GST_STATE_PLAYING);
    
    }

  } break;
  }
}

void track_controller::pad_added_handler(GstElement *src, GstPad *new_pad,
                                         player *data) {
  GstPad *sink_pad = gst_element_get_static_pad(data->convert, "sink");
  GstPadLinkReturn ret;
  GstCaps *new_pad_caps = nullptr;
  GstStructure *new_pad_struct = nullptr;
  const gchar *new_pad_type = nullptr;
  g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad),
          GST_ELEMENT_NAME(src));

  new_pad_caps = gst_pad_get_current_caps(new_pad);
  new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
  new_pad_type = gst_structure_get_name(new_pad_struct);
  if (!g_str_has_prefix(new_pad_type, "audio/x-raw")) {
    g_print("It has type '%s' which is not raw audio. Ignoring.\n",
            new_pad_type);
    return;
  }

  ret = gst_pad_link(new_pad, sink_pad);
  if (GST_PAD_LINK_FAILED(ret)) {
    g_print("Type is '%s' but link failed.\n", new_pad_type);
  } else {
    g_print("Link succeeded (type '%s').\n", new_pad_type);
  }
}

void track_controller::on_changed_volume() {
  g_object_set(elements.volume, "volume", track_list_view->slider->get_value(),
               nullptr);
}
void track_controller::update_playing_buttons() {
  if (elements.playing)
    track_list_view->play_button->set_label("⏸︎");
  else
    track_list_view->play_button->set_label("▶");

  track_list_view->update();
}
void track_controller::play() {
  std::shared_ptr<Gtk::SingleSelection> ss =
      std::dynamic_pointer_cast<Gtk::SingleSelection>(
          track_list_view->track_list_view->get_model());

  if (ss->get_selected() != playing_track_index) {
    playing_track_index = ss->get_selected();
    std::string path_with_index = get_path_of_column(ss->get_selected());
    g_object_set(elements.source, "uri", path_with_index.c_str(), NULL);
    gst_element_set_state(elements.pipeline, GST_STATE_NULL);
    gst_element_set_state(elements.pipeline, GST_STATE_PLAYING);
  }
  if (!elements.playing) {
    elements.playing = true;
    gst_element_set_state(elements.pipeline, GST_STATE_PLAYING);
  } else {
    elements.playing = false;
    gst_element_set_state(elements.pipeline, GST_STATE_PAUSED);
  }

  playing_state_label();
  update_playing_buttons();

  stopped_state = false;
}
void track_controller::stop() {
  gst_element_set_state(elements.pipeline, GST_STATE_NULL);

  stopped_state = true;
  elements.playing = false;

  track_list_view->update();
  playing_state_label();

  update_playing_buttons();
}
void track_controller::add_track(const std::string &path) {
  TagLib::FileRef file_add(path.c_str());

  // Fix to 8bit for a better conversion that allows UTF-8 characters
  Glib::ustring artist = file_add.tag()->artist().to8Bit();
  Glib::ustring song_name = file_add.tag()->title().to8Bit();
  Glib::ustring album = file_add.tag()->album().to8Bit();
  Glib::ustring track_id = std::to_string(file_add.tag()->track());

  std::string new_file_path = "file://" + path;
  g_object_set(elements.source, "uri", new_file_path.c_str(), NULL);

  std::string duration_mins = std::format(
      "{:02}", (file_add.file()->audioProperties()->lengthInSeconds() / 60));
  std::string duration_secs = std::format(
      "{:02}", (file_add.file()->audioProperties()->lengthInSeconds() % 60));

  // Setting the info in the column view
  track_list_view->track_model->append(
      track_item::create("", track_id, artist, album, song_name,
                         duration_mins + ":" + duration_secs));
}

void track_controller::on_track_selected() {}

std::string track_controller::get_path_of_column(int index) {
  return column_path[index];
}
void track_controller::on_column_selected(guint pos) {
  std::shared_ptr<Gtk::SingleSelection> ss =
      std::dynamic_pointer_cast<Gtk::SingleSelection>(
          track_list_view->track_list_view->get_model());

  playing_track_index = ss->get_selected();
  std::string path_with_index = get_path_of_column(playing_track_index);
  g_object_set(elements.source, "uri", path_with_index.c_str(), NULL);

  gst_element_set_state(elements.pipeline, GST_STATE_NULL);
  gst_element_set_state(elements.pipeline, GST_STATE_PLAYING);
  elements.playing = true;
  stopped_state = false;

  update_playing_buttons();
  playing_state_label();
}

void track_controller::playing_state_label() {
  track_list_view->update();
  std::shared_ptr<Gtk::SingleSelection> st =
      std::dynamic_pointer_cast<Gtk::SingleSelection>(
          track_list_view->track_list_view->get_model());

  for (int i = 0; i < column_path.size(); i++) {
    track_list_view->track_model->get_item(i)->playing = "";
    track_list_view->update();
  }
  if (track_list_view->track_model->get_item(0)) {

    if (!elements.playing) {
      track_list_view->track_model->get_item(st->get_selected())->playing = "▶";
      track_list_view->update();
    } else {
      track_list_view->track_model->get_item(st->get_selected())->playing = "⏸︎";
      track_list_view->update();
    }
  } else {
    std::cout << "No File Selected" << std::endl;
  }
};
