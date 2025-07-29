#pragma once
#include "track_controller.hpp"
#include <gtkmm.h>

class file_controller {
public:
  std::string file_path;
  std::string directory_path;
  std::shared_ptr<track_controller> track_control;
  file_controller(std::shared_ptr<track_controller> &track_control);
  void on_file_selected(const Glib::RefPtr<Gio::AsyncResult> &,
                        const Glib::RefPtr<Gtk::FileDialog> &);
  void on_folder_selected(const Glib::RefPtr<Gio::AsyncResult> &result_folder,
                          const Glib::RefPtr<Gtk::FileDialog> &folder);
  void on_open_file();
  void on_open_folder();
};
