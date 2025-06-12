#include <gtkmm.h>

class file_controller {
  public:
  void on_file_selected(const Glib::RefPtr<Gio::AsyncResult> &,
                        const Glib::RefPtr<Gtk::FileDialog> &);

  void on_open_file();
};
