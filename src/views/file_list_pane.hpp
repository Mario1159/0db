#pragma once
#include "../controllers/file_controller.hpp"
#include <gtkmm.h>
#include <memory>

#include "../models/track_source.hpp"
#include "glibmm/refptr.h"

class file_list_pane {
public:
  std::shared_ptr<track_controller> track_control;
  file_controller file_control;
  const Glib::ustring builder_ui_path = "file_list_view";

  file_list_pane(const Glib::RefPtr<Gtk::Builder> &builder,
                 std::shared_ptr<track_controller> &track_control);

private:
  std::unique_ptr<track_source_model> model;
};
