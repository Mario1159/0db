#include "track_source.hpp"

#include <iostream>
#include <filesystem>

track_source_model::track_source_model(
    const Glib::ustring &library_label_str,
    const Glib::ustring &playlist_label_str,
    const Glib::ustring &streaming_label_str,
    const Glib::ustring &all_songs_label_str,
    const Glib::ustring &artists_label_str,
    const Glib::ustring &albums_label_str,
    const Glib::ustring &favorite_playlists_label_str,
    const Glib::ustring &live_radio_label_str,
    const Glib::ustring &web_streaming_label_str)
    : library_label_str(library_label_str),
      playlist_label_str(playlist_label_str),
      streaming_label_str(streaming_label_str),
      all_songs_label_str(all_songs_label_str),
      artists_label_str(artists_label_str), albums_label_str(albums_label_str),
      favorite_playlists_label_str(favorite_playlists_label_str),
      live_radio_label_str(live_radio_label_str),
      web_streaming_label_str(web_streaming_label_str),
      song_list(Gio::ListStore<Gtk::StringObject>::create()),
      playlist_list(Gio::ListStore<Gtk::StringObject>::create()),
      artist_list(Gio::ListStore<Gtk::StringObject>::create()) {

  root_model = track_source_model::create_list_model();
  tree_list_model = Gtk::TreeListModel::create(
      root_model, sigc::mem_fun(*this, &track_source_model::create_list_model));
  selection_model = Gtk::MultiSelection::create(tree_list_model);
}

Glib::RefPtr<Gio::ListModel> track_source_model::create_list_model(
    const Glib::RefPtr<Glib::ObjectBase> &item) {

  Glib::RefPtr<Gio::ListStore<Gtk::StringObject>> result =
      Gio::ListStore<Gtk::StringObject>::create();

  if (!item) {
    result->append(Gtk::StringObject::create(library_label_str));
    result->append(Gtk::StringObject::create(playlist_label_str));
    result->append(Gtk::StringObject::create(streaming_label_str));
  } else {
    Glib::RefPtr<Gtk::StringObject> parent =
        std::dynamic_pointer_cast<Gtk::StringObject>(item);
    if (!parent)
      return Glib::RefPtr<Gio::ListModel>();

    if (parent->get_string() == library_label_str) {
      result->append(Gtk::StringObject::create(all_songs_label_str));
      result->append(Gtk::StringObject::create(artists_label_str));
      result->append(Gtk::StringObject::create(albums_label_str));
    } else if (parent->get_string() == playlist_label_str) {
      result->append(Gtk::StringObject::create(favorite_playlists_label_str));
    } else if (parent->get_string() == streaming_label_str) {
      result->append(Gtk::StringObject::create(live_radio_label_str));
      result->append(Gtk::StringObject::create(web_streaming_label_str));
    }
  }

  if (result->get_n_items() > 0) {
    return result;
  } else {
    return Glib::RefPtr<Gio::ListModel>();
  }
}

void track_source_model::connect_to_db(Glib::ustring path) {
  db_path = path;

  if (sqlite3_open(db_path.c_str(), &db)) {
    std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    db = nullptr;
  } else {
    initialize_schema();
  }
}

void track_source_model::initialize_schema() {
  const char *sql_create_tables = R"sql(
    CREATE TABLE IF NOT EXISTS songs (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      path TEXT UNIQUE NOT NULL,
      name TEXT,
      artist TEXT,
      duration INTEGER,
      timestamp_added INTEGER
    );

    CREATE TABLE IF NOT EXISTS playlists (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      name TEXT UNIQUE NOT NULL
    );

    CREATE TABLE IF NOT EXISTS playlist_tracks (
      playlist_id INTEGER,
      track_id INTEGER,
      PRIMARY KEY (playlist_id, track_id),
      FOREIGN KEY (playlist_id) REFERENCES playlists(id),
      FOREIGN KEY (track_id) REFERENCES songs(id)
    );
  )sql";

  char *errmsg = nullptr;
  if (sqlite3_exec(db, sql_create_tables, nullptr, nullptr, &errmsg) !=
      SQLITE_OK) {
    std::cerr << "Failed to create tables: " << errmsg << std::endl;
    sqlite3_free(errmsg);
  }
}

void track_source_model::add_audio_source(Glib::ustring path) {
  if (!db)
    return;

  const std::string insert_sql = R"sql(
    INSERT OR IGNORE INTO songs (path, name, artist, duration, timestamp_added)
    VALUES (?, ?, ?, ?, ?);
  )sql";

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db, insert_sql.c_str(), -1, &stmt, nullptr) ==
      SQLITE_OK) {
    std::string filename = std::filesystem::path(path.raw()).filename().string();
    int timestamp = static_cast<int>(std::time(nullptr));

    sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, filename.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, "", -1,
                      SQLITE_TRANSIENT); // Placeholder for artist
    sqlite3_bind_int(stmt, 4, 0);        // Placeholder for duration
    sqlite3_bind_int(stmt, 5, timestamp);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
      std::cerr << "Failed to insert song: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
  }
}

void track_source_model::create_playlist(std::vector<int> source_ids) {
  if (!db) return;

  const char* insert_playlist = "INSERT OR IGNORE INTO playlists (name) VALUES (?);";
  const char* insert_track = "INSERT OR IGNORE INTO playlist_tracks (playlist_id, track_id) VALUES (?, ?);";

  std::string playlist_name = "New Playlist"; // Ideally ask user

  // Insert playlist
  sqlite3_stmt* stmt = nullptr;
  sqlite3_prepare_v2(db, insert_playlist, -1, &stmt, nullptr);
  sqlite3_bind_text(stmt, 1, playlist_name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  // Get the playlist ID
  int playlist_id = 0;
  sqlite3_prepare_v2(db, "SELECT id FROM playlists WHERE name = ?;", -1, &stmt, nullptr);
  sqlite3_bind_text(stmt, 1, playlist_name.c_str(), -1, SQLITE_TRANSIENT);
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    playlist_id = sqlite3_column_int(stmt, 0);
  }
  sqlite3_finalize(stmt);

  // Insert tracks
  for (int track_id : source_ids) {
    sqlite3_prepare_v2(db, insert_track, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, playlist_id);
    sqlite3_bind_int(stmt, 2, track_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
  }
}

void track_source_model::sync_db() {
  if (!db) return;

  song_list = Gio::ListStore<Gtk::StringObject>::create();

  const char* query = "SELECT name FROM songs ORDER BY timestamp_added DESC;";
  sqlite3_stmt* stmt = nullptr;

  if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      const unsigned char* name = sqlite3_column_text(stmt, 0);
      song_list->append(Gtk::StringObject::create(reinterpret_cast<const char*>(name)));
    }
    sqlite3_finalize(stmt);
  }
}

