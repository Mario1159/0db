#pragma once

#include <gtkmm.h>

// Track model item for GtkColumnView
class track_item : public Glib::Object {
public:
  Glib::ustring playing;
  Glib::ustring track_id;
  Glib::ustring artist;
  Glib::ustring album;
  Glib::ustring name;
  Glib::ustring duration;

  track_item(const Glib::ustring &p, const Glib::ustring &id,
             const Glib::ustring &a, const Glib::ustring &al,
             const Glib::ustring &n, const Glib::ustring &d)
      : playing(p), track_id(id), artist(a), album(al), name(n), duration(d) {}

  static Glib::RefPtr<track_item>
  create(const Glib::ustring &p, const Glib::ustring &id,
         const Glib::ustring &a, const Glib::ustring &al,
         const Glib::ustring &n, const Glib::ustring &d);
};
