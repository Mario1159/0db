#include "track_item.hpp"

Glib::RefPtr<track_item>
  track_item::create(const Glib::ustring &p, const Glib::ustring &id,
         const Glib::ustring &a, const Glib::ustring &al,
         const Glib::ustring &n, const Glib::ustring &d) {
    return Glib::make_refptr_for_instance<track_item>(
        new track_item(p, id, a, al, n, d));
  }
