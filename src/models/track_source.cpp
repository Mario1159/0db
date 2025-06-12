#include "track_source.hpp"

track_source_model::track_source_model() {

  root_model = track_source_model::create_list_model();
  tree_list_model = Gtk::TreeListModel::create(
      root_model, sigc::mem_fun(*this, &track_source_model::create_list_model));
  selection_model = Gtk::MultiSelection::create(tree_list_model);
}

Glib::RefPtr<Gio::ListModel> track_source_model::create_list_model(
    const Glib::RefPtr<Glib::ObjectBase> &item) {

  Glib::RefPtr<Gio::ListStore<Gtk::StringObject>> result =
      Gio::ListStore<Gtk::StringObject>::create();

  // This should be refactored into the view
  if (!item) {
    result->append(Gtk::StringObject::create("Library"));
    result->append(Gtk::StringObject::create("Playlists"));
    result->append(Gtk::StringObject::create("Streaming"));
  } else {
    Glib::RefPtr<Gtk::StringObject> parent =
        std::dynamic_pointer_cast<Gtk::StringObject>(item);
    if (!parent)
      return Glib::RefPtr<Gio::ListModel>();

    if (parent->get_string() == "Library") {
      result->append(Gtk::StringObject::create("All Songs"));
      result->append(Gtk::StringObject::create("Artists"));
      result->append(Gtk::StringObject::create("Albums"));
    } else if (parent->get_string() == "Playlists") {
      result->append(Gtk::StringObject::create("Favorites"));
    } else if (parent->get_string() == "Streaming") {
      result->append(Gtk::StringObject::create("Live Radio"));
      result->append(Gtk::StringObject::create("Web Streaming"));
    }
  }

  if (result->get_n_items() > 0) {
    return result;
  } else {
    return Glib::RefPtr<Gio::ListModel>();
  }
}
