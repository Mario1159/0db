#include "file_list_pane.hpp"

file_list_pane::file_list_pane(const Glib::RefPtr<Gtk::Builder> &builder,std::shared_ptr<track_controller> &track_control):file_control(track_control) {
  model = std::make_unique<track_source_model>(
      "Library", "Playlists", "Streaming", "All Songs", "Artists", "Albums",
      "Favorites", "Live Radio", "Web Streaming");

  Gtk::ListView *list_view =
      builder->get_widget<Gtk::ListView>(builder_ui_path);
  if (list_view && model->selection_model) {
    list_view->set_model(model->selection_model);

    Glib::RefPtr<Gtk::SignalListItemFactory> factory =
        Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
          Gtk::TreeExpander *expander = Gtk::make_managed<Gtk::TreeExpander>();
          Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
          label->set_halign(Gtk::Align::START);
          expander->set_child(*label);
          list_item->set_child(*expander);
        });

    factory->signal_bind().connect(
        [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
          Glib::RefPtr<Gtk::TreeListRow> row =
              std::dynamic_pointer_cast<Gtk::TreeListRow>(
                  list_item->get_item());
          if (!row)
            return;
          Glib::RefPtr<Gtk::StringObject> item =
              std::dynamic_pointer_cast<Gtk::StringObject>(row->get_item());
          if (!item)
            return;
          Gtk::TreeExpander *expander =
              dynamic_cast<Gtk::TreeExpander *>(list_item->get_child());
          if (!expander)
            return;
          expander->set_list_row(row);
          Gtk::Label *label = dynamic_cast<Gtk::Label *>(expander->get_child());
          if (label && item)
            label->set_text(item->get_string());
        });

    list_view->set_factory(factory);
  }
}
