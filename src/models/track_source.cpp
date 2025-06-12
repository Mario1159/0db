#include "track_source.hpp"

track_source_model::track_source_model(
    const Glib::ustring &library_label_str,
    const Glib::ustring &playlist_label_str,
    const Glib::ustring &streaming_label_str,
    const Glib::ustring &all_songs_label_str,
    const Glib::ustring &artists_label_str,
    const Glib::ustring &albums_label_str,
    const Glib::ustring &favorite_playlists_label_str,
    const Glib::ustring &live_radio_label_str,
    const Glib::ustring &web_streaming_label_str)
    : library_label_str(library_label_str),
      playlist_label_str(playlist_label_str),
      streaming_label_str(streaming_label_str),
      all_songs_label_str(all_songs_label_str),
      artists_label_str(artists_label_str), albums_label_str(albums_label_str),
      favorite_playlists_label_str(favorite_playlists_label_str),
      live_radio_label_str(live_radio_label_str),
      web_streaming_label_str(web_streaming_label_str),
      song_list(Gio::ListStore<Gtk::StringObject>::create()),
      playlist_list(Gio::ListStore<Gtk::StringObject>::create()),
      artist_list(Gio::ListStore<Gtk::StringObject>::create()) {

  root_model = track_source_model::create_list_model();
  tree_list_model = Gtk::TreeListModel::create(
      root_model, sigc::mem_fun(*this, &track_source_model::create_list_model));
  selection_model = Gtk::MultiSelection::create(tree_list_model);
}

Glib::RefPtr<Gio::ListModel> track_source_model::create_list_model(
    const Glib::RefPtr<Glib::ObjectBase> &item) {

  Glib::RefPtr<Gio::ListStore<Gtk::StringObject>> result =
      Gio::ListStore<Gtk::StringObject>::create();

  if (!item) {
    result->append(Gtk::StringObject::create(library_label_str));
    result->append(Gtk::StringObject::create(playlist_label_str));
    result->append(Gtk::StringObject::create(streaming_label_str));
  } else {
    Glib::RefPtr<Gtk::StringObject> parent =
        std::dynamic_pointer_cast<Gtk::StringObject>(item);
    if (!parent)
      return Glib::RefPtr<Gio::ListModel>();

    if (parent->get_string() == library_label_str) {
      result->append(Gtk::StringObject::create(all_songs_label_str));
      result->append(Gtk::StringObject::create(artists_label_str));
      result->append(Gtk::StringObject::create(albums_label_str));
    } else if (parent->get_string() == playlist_label_str) {
      result->append(Gtk::StringObject::create(favorite_playlists_label_str));
    } else if (parent->get_string() == streaming_label_str) {
      result->append(Gtk::StringObject::create(live_radio_label_str));
      result->append(Gtk::StringObject::create(web_streaming_label_str));
    }
  }

  if (result->get_n_items() > 0) {
    return result;
  } else {
    return Glib::RefPtr<Gio::ListModel>();
  }
}
