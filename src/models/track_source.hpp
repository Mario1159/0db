#include <gtkmm.h>
#include <vector>

// Model that store the file list data.
// This currently is not a model for a single item, but rather a wrapper for
// different models that control the tree view.
class track_source_model {
public:
  track_source_model(const Glib::ustring &library_label_str = "",
                     const Glib::ustring &playlist_label_str = "",
                     const Glib::ustring &streaming_label_str = "",
                     const Glib::ustring &all_songs_label_str = "",
                     const Glib::ustring &artists_label_str = "",
                     const Glib::ustring &albums_label_str = "",
                     const Glib::ustring &favorites_playlists_label_str = "",
                     const Glib::ustring &live_radio_label_str = "",
                     const Glib::ustring &web_streaming_label_str = "");

  Glib::RefPtr<Gtk::MultiSelection> selection_model;

  // Loads a audio source from a file path to the model
  void add_audio_source(Glib::ustring path) {}
  void create_playlist(std::vector<int> source_ids) {}

  void connect_to_db(Glib::ustring path) {}
  void sync_db() {}

  // Clear the library contents
  void clear() {}

private:
  Glib::RefPtr<Gio::ListModel> root_model;
  Glib::RefPtr<Gtk::TreeListModel> tree_list_model;

  Glib::RefPtr<Gio::ListModel>
  create_list_model(const Glib::RefPtr<Glib::ObjectBase> &item = {});

  // The StringObject can be changed to a custom model that represent the files
  // in a more descriptive way.
  Glib::RefPtr<Gio::ListStore<Gtk::StringObject>> song_list;
  Glib::RefPtr<Gio::ListStore<Gtk::StringObject>> playlist_list;
  Glib::RefPtr<Gio::ListStore<Gtk::StringObject>> artist_list;

  const Glib::ustring library_label_str;
  const Glib::ustring playlist_label_str;
  const Glib::ustring streaming_label_str;
  const Glib::ustring all_songs_label_str;
  const Glib::ustring artists_label_str;
  const Glib::ustring albums_label_str;
  const Glib::ustring favorite_playlists_label_str;
  const Glib::ustring live_radio_label_str;
  const Glib::ustring web_streaming_label_str;
};
