#include "main_window_view.hpp"
#include "sigc++/functors/mem_fun.h"

#include <iostream>
#include <memory>
#include <string>

main_window_view::main_window_view(const Glib::RefPtr<Gtk::Application> &app,
                                   const Glib::RefPtr<Gtk::Builder> &builder)
    : app(app), builder(builder) {

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
  track_list_view = std::make_unique<track_list_pane>(builder);
  file_list_view =
      std::make_unique<file_list_pane>(builder, track_list_view->controller);

  connect_menu_bar_model();
  bind_signals_and_actions();
}

void main_window_view::show() {
  app->add_window(*main_window);
  main_window->show();
}

// Connect the static popover menu bar items.
// This cannot be bound directly in the .ui file due to gtkmm limitations.
void main_window_view::connect_menu_bar_model() {
  Gtk::PopoverMenuBar *menu_bar =
      builder->get_widget<Gtk::PopoverMenuBar>("menu_bar");
  Glib::RefPtr<Gio::MenuModel> menu_model =
      builder->get_object<Gio::MenuModel>("menubar");

  if (menu_bar && menu_model)
    menu_bar->set_menu_model(menu_model);
}

void main_window_view::bind_signals_and_actions() {
  // Bind actions
  app->add_action("open", sigc::mem_fun(file_list_view->file_control,
                                        &file_controller::on_open_file));

 app->add_action("open_folder",
                 sigc::mem_fun(file_list_view->file_control,
                           &file_controller::on_open_folder));
  app->add_action("quit", sigc::mem_fun(*app, &Gtk::Application::quit));
}
