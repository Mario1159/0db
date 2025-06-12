#include "track_list_pane.hpp"

track_list_pane::track_list_pane(const Glib::RefPtr<Gtk::Builder> &builder) {
  track_list_view = builder->get_widget<Gtk::ColumnView>("track_list_view");

  track_model = Gio::ListStore<track_item>::create();
  track_model->append(track_item::create("", "001", "Dummy Artist",
                                         "Dummy Album", "Dummy Song", "3:30"));
  track_model->append(track_item::create("▶", "002", "Dummy Artist",
                                         "Dummy Album", "Dummy Song", "3:30"));
  track_model->append(track_item::create("", "003", "Dummy Artist",
                                         "Dummy Album", "Dummy Song", "3:30"));

  controller = std::make_unique<track_controller>(this);

  if (track_list_view) {
    Glib::RefPtr<Gtk::SingleSelection> selection_model =
        Gtk::SingleSelection::create(track_model);
    track_list_view->set_model(selection_model);
    update();
  }
}

void track_list_pane::update() {

  // This can be refactored with a for loop, but due to the amount of lambdas
  // and templates arguments it won't be trivial to make something clean and
  // maintainable.

  Glib::RefPtr<Gtk::SignalListItemFactory> playing_factory =
      Gtk::SignalListItemFactory::create();
  playing_factory->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
        list_item->set_child(*label);
      });
  playing_factory->signal_bind().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Glib::RefPtr<track_item> item =
            std::dynamic_pointer_cast<track_item>(list_item->get_item());
        if (!item)
          return;
        Gtk::Label *label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        if (label)
          label->set_text(item->playing);
      });
  Glib::RefPtr<Gtk::ColumnViewColumn> col0 =
      std::dynamic_pointer_cast<Gtk::ColumnViewColumn>(
          track_list_view->get_columns()->get_object(0));
  if (col0)
    col0->set_factory(playing_factory);

  Glib::RefPtr<Gtk::SignalListItemFactory> track_id_factory =
      Gtk::SignalListItemFactory::create();
  track_id_factory->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
        list_item->set_child(*label);
      });
  track_id_factory->signal_bind().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Glib::RefPtr<track_item> item =
            std::dynamic_pointer_cast<track_item>(list_item->get_item());
        if (!item)
          return;
        Gtk::Label *label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        if (label)
          label->set_text(item->track_id);
      });
  Glib::RefPtr<Gtk::ColumnViewColumn> col1 =
      std::dynamic_pointer_cast<Gtk::ColumnViewColumn>(
          track_list_view->get_columns()->get_object(1));
  if (col1)
    col1->set_factory(track_id_factory);

  Glib::RefPtr<Gtk::SignalListItemFactory> artist_factory =
      Gtk::SignalListItemFactory::create();
  artist_factory->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
        list_item->set_child(*label);
      });
  artist_factory->signal_bind().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Glib::RefPtr<track_item> item =
            std::dynamic_pointer_cast<track_item>(list_item->get_item());
        if (!item)
          return;
        Gtk::Label *label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        if (label)
          label->set_text(item->artist);
      });
  Glib::RefPtr<Gtk::ColumnViewColumn> col2 =
      std::dynamic_pointer_cast<Gtk::ColumnViewColumn>(
          track_list_view->get_columns()->get_object(2));
  if (col2)
    col2->set_factory(artist_factory);

  Glib::RefPtr<Gtk::SignalListItemFactory> album_factory =
      Gtk::SignalListItemFactory::create();
  album_factory->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
        list_item->set_child(*label);
      });
  album_factory->signal_bind().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Glib::RefPtr<track_item> item =
            std::dynamic_pointer_cast<track_item>(list_item->get_item());
        if (!item)
          return;
        Gtk::Label *label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        if (label)
          label->set_text(item->album);
      });
  Glib::RefPtr<Gtk::ColumnViewColumn> col3 =
      std::dynamic_pointer_cast<Gtk::ColumnViewColumn>(
          track_list_view->get_columns()->get_object(3));
  if (col3)
    col3->set_factory(album_factory);

  Glib::RefPtr<Gtk::SignalListItemFactory> name_factory =
      Gtk::SignalListItemFactory::create();
  name_factory->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
        list_item->set_child(*label);
      });
  name_factory->signal_bind().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Glib::RefPtr<track_item> item =
            std::dynamic_pointer_cast<track_item>(list_item->get_item());
        if (!item)
          return;
        Gtk::Label *label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        if (label)
          label->set_text(item->name);
      });
  Glib::RefPtr<Gtk::ColumnViewColumn> col4 =
      std::dynamic_pointer_cast<Gtk::ColumnViewColumn>(
          track_list_view->get_columns()->get_object(4));
  if (col4)
    col4->set_factory(name_factory);

  Glib::RefPtr<Gtk::SignalListItemFactory> duration_factory =
      Gtk::SignalListItemFactory::create();
  duration_factory->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
        list_item->set_child(*label);
      });
  duration_factory->signal_bind().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &list_item) {
        Glib::RefPtr<track_item> item =
            std::dynamic_pointer_cast<track_item>(list_item->get_item());
        if (!item)
          return;
        Gtk::Label *label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        if (label)
          label->set_text(item->duration);
      });
  Glib::RefPtr<Gtk::ColumnViewColumn> col5 =
      std::dynamic_pointer_cast<Gtk::ColumnViewColumn>(
          track_list_view->get_columns()->get_object(5));
  if (col5)
    col5->set_factory(duration_factory);
}
