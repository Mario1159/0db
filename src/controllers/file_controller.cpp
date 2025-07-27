#include "file_controller.hpp"
#include "giomm/file.h"
#include "giomm/fileenumerator.h"
#include "glib-object.h"
#include "glibmm/refptr.h"
#include "gst/gstelement.h"
#include "gtkmm/filefilter.h"
#include "sigc++/adaptors/bind.h"
#include "sigc++/functors/mem_fun.h"
#include "track_controller.hpp"
#include <iostream>
#include <memory>
#include <taglib/audioproperties.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>

file_controller::file_controller(
    std::shared_ptr<track_controller> &track_control)
    : track_control(track_control) {}

void file_controller::on_file_selected(
    const Glib::RefPtr<Gio::AsyncResult> &resultado,
    const Glib::RefPtr<Gtk::FileDialog> &file) {

  // const Glib::RefPtr<Gio::AsyncResult> result;
  //

  // getting the selected file on the previous filedialog
  Glib::RefPtr<const Gio::File> archivo = file->open_finish(resultado);

  pato_file = archivo->get_uri();

  track_control->column_path.push_back(pato_file);

  std::string path = archivo->get_path();

  track_control->add_track(path);
}
void file_controller::on_folder_selected(
    const Glib::RefPtr<Gio::AsyncResult> &result_folder,
    const Glib::RefPtr<Gtk::FileDialog> &folder) {

  Glib::RefPtr<Gio::File> asd = folder->select_folder_finish(result_folder);
  pato_folder = asd->get_path();
  Glib::RefPtr<Gio::FileEnumerator> files_enum = asd->enumerate_children();
  Glib::RefPtr<Gio::FileInfo> file_1 = files_enum->next_file();
  Glib::RefPtr<Gio::File> file_name = asd->get_child(file_1->get_name());
  std::cout << file_name->get_path() << std::endl;

  while (file_1 != nullptr) {

    file_name = asd->get_child(file_1->get_name());
    if (file_1->get_content_type().find("audio/") != std::string::npos) {
      track_control->column_path.push_back(file_name->get_uri());
      track_control->add_track(file_name->get_path());
      std::cout << file_1->get_content_type() << std::endl;
    }
    file_1 = files_enum->next_file();
  }
  files_enum->close();
}

void file_controller::on_open_file() {
  Glib::RefPtr<Gtk::FileDialog> file_dialog = Gtk::FileDialog::create();
  file_dialog->set_title("Open Audio File");
  Glib::RefPtr<Gio::ListStore<Gtk::FileFilter>> file_filters =
      Gio::ListStore<Gtk::FileFilter>::create();
  Glib::RefPtr<Gtk::FileFilter> filter_audio = Gtk::FileFilter::create();

  filter_audio->set_name("Audio Files");
  filter_audio->add_mime_type("audio/*");
  file_filters->append(filter_audio);

  file_dialog->set_filters(file_filters);
  file_dialog->open(sigc::bind(
      sigc::mem_fun(*this, &file_controller::on_file_selected), file_dialog));
}

// THIS IS GIVING A PROBLEM !!!!! 30/06 check main window view cpp LINE 63
// NOT GIVING A PROBLEM ANYMORE!
void file_controller::on_open_folder() {
  Glib::RefPtr<Gtk::FileDialog> file_dialog_folder = Gtk::FileDialog::create();

  file_dialog_folder->set_title("Select folder");
  file_dialog_folder->select_folder(
      sigc::bind(sigc::mem_fun(*this, &file_controller::on_folder_selected),
                 file_dialog_folder));
}
