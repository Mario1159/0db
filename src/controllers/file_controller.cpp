#include "file_controller.hpp"

void file_controller::on_file_selected(const Glib::RefPtr<Gio::AsyncResult> &,
                                       const Glib::RefPtr<Gtk::FileDialog> &) {}

void file_controller::on_open_file() {
  Glib::RefPtr<Gtk::FileDialog> file_dialog = Gtk::FileDialog::create();
  file_dialog->set_title("Open Audio File");
  file_dialog->open(sigc::bind(sigc::mem_fun(*this, &file_controller::on_file_selected), file_dialog));
}
