cmake_minimum_required(VERSION 3.16)
project(0db VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-4.0 REQUIRED)

set(SOURCES
  src/main.cpp
  src/models/track_source.hpp
  src/models/track_source.cpp
  src/models/track_item.hpp
  src/models/track_item.cpp
  src/controllers/file_controller.hpp
  src/controllers/file_controller.cpp
  src/controllers/track_controller.hpp
  src/controllers/track_controller.cpp
  src/views/file_list_pane.hpp
  src/views/file_list_pane.cpp
  src/views/track_list_pane.hpp
  src/views/track_list_pane.cpp
  src/views/main_window_view.hpp
  src/views/main_window_view.cpp
)

set(UI_FILES
  ui/main_window.ui
)

add_executable(0db ${SOURCES})

target_include_directories(0db PRIVATE ${GTKMM_INCLUDE_DIRS})
target_link_libraries(0db PRIVATE ${GTKMM_LIBRARIES})
target_compile_options(0db PRIVATE ${GTKMM_CFLAGS_OTHER})

set(UI_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/ui)
file(MAKE_DIRECTORY ${UI_BUILD_DIR})

foreach(ui_file IN LISTS UI_FILES)
  get_filename_component(ui_filename ${ui_file} NAME)
  add_custom_command(
    TARGET 0db POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/${ui_file}
      ${UI_BUILD_DIR}/${ui_filename}
  )
endforeach()

install(FILES ${UI_FILES} DESTINATION share/0db/ui)
