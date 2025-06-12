#include <gtkmm.h>

class track_source_model {
public:
  track_source_model();

  Glib::RefPtr<Gtk::MultiSelection> selection_model;

private:
  Glib::RefPtr<Gio::ListModel> root_model;
  Glib::RefPtr<Gtk::TreeListModel> tree_list_model;

  Glib::RefPtr<Gio::ListModel>
  create_list_model(const Glib::RefPtr<Glib::ObjectBase> &item = {});
};
