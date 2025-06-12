#include <gtkmm.h>
#include <memory>

#include "../models/track_source.hpp"

class file_list_pane {
public:
  const Glib::ustring builder_ui_path = "file_list_view";

  file_list_pane(const Glib::RefPtr<Gtk::Builder> &builder);

  private:
  std::unique_ptr<track_source_model> model;
};
