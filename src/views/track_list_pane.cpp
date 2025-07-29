#include "track_list_pane.hpp"
#include "gst/gstclock.h"
#include "gst/gstformat.h"
#include "gst/gstmessage.h"
#include "gst/gstutils.h"
#include "gtkmm/progressbar.h"
#include "iostream"
#include "sigc++/functors/mem_fun.h"
#include <format>
#include <string>

track_list_pane::track_list_pane(const Glib::RefPtr<Gtk::Builder> &builder) {
  track_list_view = builder->get_widget<Gtk::ColumnView>("track_list_view");
  track_model = Gio::ListStore<track_item>::create();

  if (track_list_view) {
    Glib::RefPtr<Gtk::SingleSelection> selection_model =
        Gtk::SingleSelection::create(track_model);
    track_list_view->set_model(selection_model);
    update();
  }
  controller = std::make_shared<track_controller>(this);
  play_button = builder->get_widget<Gtk::Button>("play_button");
  play_button->signal_clicked().connect(
      sigc::mem_fun(*controller, &track_controller::play));
  stop_button = builder->get_widget<Gtk::Button>("stop_button");
  stop_button->signal_clicked().connect(
      sigc::mem_fun(*controller, &track_controller::stop));

  slider = builder->get_widget<Gtk::Scale>("volume_slider");
  slider->set_range(0, 1);
  slider->set_value(0.5);

  volume_label = builder->get_widget<Gtk::Label>("volume_label");
  progress_bar = builder->get_widget<Gtk::ProgressBar>("progress_bar");
  progress_label = builder->get_widget<Gtk::Label>("timestamp_label");

  messages_timeout = Glib::signal_timeout().connect(
      sigc::mem_fun(*this, &track_list_pane::msg_timeout), 10);
  progress_bar_timeout = Glib::signal_timeout().connect(
      sigc::mem_fun(*this, &track_list_pane::progress_bar_pos_timeout), 500);

  slider->signal_value_changed().connect(sigc::mem_fun(
      *this, &track_list_pane::on_volume_changed_sync_volume_level_label));
  slider->signal_value_changed().connect(
      sigc::mem_fun(*controller, &track_controller::on_changed_volume));
}
bool track_list_pane::msg_timeout() {
  // Update progress bar
  controller->msg = gst_bus_timed_pop_filtered(
      controller->bus, 0 * GST_MSECOND,
      GstMessageType(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR |
                     GST_MESSAGE_EOS));
  if (controller->msg != NULL) {
    controller->handle_message(&controller->elements, controller->msg);
  }
  return true;
}

bool track_list_pane::progress_bar_pos_timeout() {
  gint64 current = 0;

  gst_element_query_duration(controller->elements.pipeline, GST_FORMAT_TIME,
                             &controller->elements.duration);
  gst_element_query_position(controller->elements.pipeline, GST_FORMAT_TIME,
                             &current);

  if (controller->column_path.size()>0 ) {
    progress_bar->set_fraction(current);
    progress_bar->set_fraction(
        (float(current) / (float(controller->elements.duration))));

    if (controller->stopped_state) {
      progress_bar->set_fraction(0);
    }
  }
  std::string current_minutes =
      std::format("{:02}", ((int)(current / GST_SECOND / 60)));

  std::string current_seconds =
      std::format("{:02}", ((int)(current / GST_SECOND % 60)));
  std::string track_minutes = std::format(
      "{:02}", ((int)(controller->elements.duration / GST_SECOND / 60)));
  std::string track_seconds = std::format(
      "{:02}", ((int)(controller->elements.duration / GST_SECOND % 60)));
  progress_label->set_label(current_minutes + ":" + current_seconds + " - " + track_minutes +
                            ":" + track_seconds);
  return true;
}

void track_list_pane::on_volume_changed_sync_volume_level_label() {
  std::string porcen =
      "🔊 " + std::to_string((int)(slider->get_value() * 100)) + "%";
  volume_label->set_label(porcen);
}

void track_list_pane::update() {

  // This can be refactored with a for loop, but due to the amount of lambdas
  // and templates arguments it won't be trivial to make something clean and
  // maintainable.

  Glib::RefPtr<Gtk::SignalListItemFactory> playing_factory =
      Gtk::SignalListItemFactory::create();
  playing_factory->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
        list_item->set_child(*label);
      });
  playing_factory->signal_bind().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Glib::RefPtr<track_item> item =
            std::dynamic_pointer_cast<track_item>(list_item->get_item());
        if (!item)
          return;
        Gtk::Label *label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        if (label)
          label->set_text(item->playing);
      });
  Glib::RefPtr<Gtk::ColumnViewColumn> col0 =
      std::dynamic_pointer_cast<Gtk::ColumnViewColumn>(
          track_list_view->get_columns()->get_object(0));
  if (col0)
    col0->set_factory(playing_factory);

  Glib::RefPtr<Gtk::SignalListItemFactory> track_id_factory =
      Gtk::SignalListItemFactory::create();
  track_id_factory->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
        list_item->set_child(*label);
      });
  track_id_factory->signal_bind().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Glib::RefPtr<track_item> item =
            std::dynamic_pointer_cast<track_item>(list_item->get_item());
        if (!item)
          return;
        Gtk::Label *label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        if (label)
          label->set_text(item->track_id);
      });
  Glib::RefPtr<Gtk::ColumnViewColumn> col1 =
      std::dynamic_pointer_cast<Gtk::ColumnViewColumn>(
          track_list_view->get_columns()->get_object(1));
  if (col1)
    col1->set_factory(track_id_factory);

  Glib::RefPtr<Gtk::SignalListItemFactory> artist_factory =
      Gtk::SignalListItemFactory::create();
  artist_factory->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
        list_item->set_child(*label);
      });
  artist_factory->signal_bind().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Glib::RefPtr<track_item> item =
            std::dynamic_pointer_cast<track_item>(list_item->get_item());
        if (!item)
          return;
        Gtk::Label *label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        if (label)
          label->set_text(item->artist);
      });
  Glib::RefPtr<Gtk::ColumnViewColumn> col2 =
      std::dynamic_pointer_cast<Gtk::ColumnViewColumn>(
          track_list_view->get_columns()->get_object(2));
  if (col2)
    col2->set_factory(artist_factory);

  Glib::RefPtr<Gtk::SignalListItemFactory> album_factory =
      Gtk::SignalListItemFactory::create();
  album_factory->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
        list_item->set_child(*label);
      });
  album_factory->signal_bind().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Glib::RefPtr<track_item> item =
            std::dynamic_pointer_cast<track_item>(list_item->get_item());
        if (!item)
          return;
        Gtk::Label *label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        if (label)
          label->set_text(item->album);
      });
  Glib::RefPtr<Gtk::ColumnViewColumn> col3 =
      std::dynamic_pointer_cast<Gtk::ColumnViewColumn>(
          track_list_view->get_columns()->get_object(3));
  if (col3)
    col3->set_factory(album_factory);

  Glib::RefPtr<Gtk::SignalListItemFactory> name_factory =
      Gtk::SignalListItemFactory::create();
  name_factory->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
        list_item->set_child(*label);
      });
  name_factory->signal_bind().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Glib::RefPtr<track_item> item =
            std::dynamic_pointer_cast<track_item>(list_item->get_item());
        if (!item)
          return;
        Gtk::Label *label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        if (label)
          label->set_text(item->name);
      });
  Glib::RefPtr<Gtk::ColumnViewColumn> col4 =
      std::dynamic_pointer_cast<Gtk::ColumnViewColumn>(
          track_list_view->get_columns()->get_object(4));
  if (col4)
    col4->set_factory(name_factory);

  Glib::RefPtr<Gtk::SignalListItemFactory> duration_factory =
      Gtk::SignalListItemFactory::create();
  duration_factory->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
        list_item->set_child(*label);
      });
  duration_factory->signal_bind().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Glib::RefPtr<track_item> item =
            std::dynamic_pointer_cast<track_item>(list_item->get_item());
        if (!item)
          return;
        Gtk::Label *label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        if (label)
          label->set_text(item->duration);
      });
  Glib::RefPtr<Gtk::ColumnViewColumn> col5 =
      std::dynamic_pointer_cast<Gtk::ColumnViewColumn>(
          track_list_view->get_columns()->get_object(5));
  if (col5)
    col5->set_factory(duration_factory);
}
