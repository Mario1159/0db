#include "track_list_pane.hpp"

track_list_pane::track_list_pane(Glib::RefPtr<Gtk::Builder> builder) {
  track_model = Gio::ListStore<track_item>::create();
  track_model->append(track_item::create("▶", "001", "Dummy Artist",
                                         "Dummy Album", "Dummy Song", "3:30"));
  track_list_view = builder->get_widget<Gtk::ColumnView>("track_list_view");
  track_controller::update_track_list(track_list_view, track_model);
}
