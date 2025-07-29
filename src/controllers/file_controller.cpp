#include "file_controller.hpp"
#include "giomm/file.h"
#include "giomm/fileenumerator.h"
#include "glibmm/refptr.h"
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
  Glib::RefPtr<const Gio::File> archivo = file->open_finish(resultado);
  file_path = archivo->get_uri();
  track_control->column_path.push_back(file_path);
  std::string path = archivo->get_path();
  track_control->add_track(path);
}

void file_controller::on_folder_selected(
    const Glib::RefPtr<Gio::AsyncResult> &result_folder,
    const Glib::RefPtr<Gtk::FileDialog> &folder) {

  Glib::RefPtr<Gio::File> file = folder->select_folder_finish(result_folder);
  directory_path = file->get_path();

  // Get all the audio files from the directory selected
  Glib::RefPtr<Gio::FileEnumerator> files_enum = file->enumerate_children();
  Glib::RefPtr<Gio::FileInfo> file_i = files_enum->next_file();
  Glib::RefPtr<Gio::File> file_name = file->get_child(file_i->get_name());

  while (file_i != nullptr) {
    file_name = file->get_child(file_i->get_name());
    // If file[i] has MIME type audio/*
    if (file_i->get_content_type().find("audio/") != std::string::npos) {
      // Adds the track to the track list
      track_control->column_path.push_back(file_name->get_uri());
      track_control->add_track(file_name->get_path());
      std::cout << file_i->get_content_type() << std::endl;
    }
    file_i = files_enum->next_file();
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

void file_controller::on_open_folder() {
  Glib::RefPtr<Gtk::FileDialog> file_dialog_folder = Gtk::FileDialog::create();

  file_dialog_folder->set_title("Select folder");
  file_dialog_folder->select_folder(
      sigc::bind(sigc::mem_fun(*this, &file_controller::on_folder_selected),
                 file_dialog_folder));
}
