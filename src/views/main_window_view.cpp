#include "main_window_view.hpp"

#include <iostream>
#include <memory>

main_window_view::main_window_view(Glib::RefPtr<Gtk::Application> app,
                                   Glib::RefPtr<Gtk::Builder> builder) {
  this->app = app;

  try {
    builder->add_from_file("ui/main_window.ui");
  } catch (const Glib::FileError &ex) {
    std::cerr << "FileError: " << ex.what() << std::endl;
    return;
  } catch (const Glib::MarkupError &ex) {
    std::cerr << "MarkupError: " << ex.what() << std::endl;
    return;
  } catch (const Gtk::BuilderError &ex) {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
    return;
  }

  main_window = builder->get_widget<Gtk::Window>("main_window");
  if (!main_window) {
    std::cerr << "Could not get the main window" << std::endl;
    return;
  }

  Gtk::PopoverMenuBar *menu_bar =
      builder->get_widget<Gtk::PopoverMenuBar>("menu_bar");
  Glib::RefPtr<Gio::MenuModel> menu_model =
      builder->get_object<Gio::MenuModel>("menubar");

  if (menu_bar && menu_model)
    menu_bar->set_menu_model(menu_model);

  app->add_action("open",
                  sigc::mem_fun(file_control, &file_controller::on_open_file));
  app->add_action("quit", sigc::mem_fun(*app, &Gtk::Application::quit));

  

  file_list_view = std::make_unique<file_list_pane>(builder);
  track_list_view = std::make_unique<track_list_pane>(builder);
}

void main_window_view::show() {
  app->add_window(*main_window);
  main_window->show();
}
