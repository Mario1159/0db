cmake_minimum_required(VERSION 3.16)
project(0db VERSION 0.1.0 LANGUAGES CXX)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-4.0 REQUIRED)
pkg_check_modules(GST
  gstreamer-1.0>=1.6
  gstreamer-plugins-base-1.0>=1.6
  REQUIRED
)
pkg_check_modules(TAG taglib REQUIRED)
pkg_check_modules(SQLITE sqlite3 REQUIRED)

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

find_package(Threads REQUIRED)

if(WIN32)
  set(VAR_INCLUDE_DIRS
    ${GTKMM_STATIC_INCLUDE_DIRS}
    ${GST_STATIC_INCLUDE_DIRS}
    ${TAG_STATIC_INSTALL_DIR}
    ${SQLITE_STATIC_INCLUDE_DIRS}
  )

  set(VAR_LIBRARY_DIRS
    ${GTKMM_STATIC_LIBRARY_DIRS}
    ${GST_STATIC_LIBRARY_DIRS}
    ${TAG_STATIC_LIBRARY_DIRS}
    ${SQLITE_STATIC_LIBRARY_DIRS}
  )

  set(VAR_LIBRARIES
    ${GTKMM_STATIC_LIBRARIES}
    ${GST_STATIC_LIBRARIES}
    ${TAG_STATIC_LIBRARIES}
    ${SQLITE_STATIC_LIBRARIES}
  )
else()
  set(VAR_INCLUDE_DIRS
    ${GTKMM_INCLUDE_DIRS}
    ${GST_INCLUDE_DIRS}
    ${TAG_INSTALL_DIR}
    ${SQLITE_INCLUDE_DIRS}
  )

  set(VAR_LIBRARY_DIRS
    ${GTKMM_LIBRARY_DIRS}
    ${GST_LIBRARY_DIRS}
    ${TAG_LIBRARY_DIRS}
    ${SQLITE_LIBRARY_DIRS}
  )

  set(VAR_LIBRARIES
    ${GTKMM_LIBRARIES}
    ${GST_LIBRARIES}
    ${TAG_LIBRARIES}
    ${SQLITE_LIBRARIES}
  )
endif()

target_include_directories(0db PRIVATE ${VAR_INCLUDE_DIRS})
target_link_directories(0db PRIVATE ${VAR_LIBRARY_DIRS})
target_link_libraries(0db PRIVATE ${VAR_LIBRARIES} Threads::Threads)


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
install(TARGETS 0db RUNTIME DESTINATION bin)

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "0db")
set(CPACK_PACKAGE_VENDOR "1159cl")
set(CPACK_PACKAGE_CONTACT "mario@1159.cl")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "0db Audio Player")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "0db")
set(CPACK_PACKAGE_EXECUTABLES "0db" "0db")

set(CPACK_GENERATOR "NSIS")
set(CPACK_NSIS_DISPLAY_NAME "0db Audio Player")
set(CPACK_NSIS_PACKAGE_NAME "0db")
set(CPACK_NSIS_CONTACT "mario@1159.cl")
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\0db.exe")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

if(WIN32 AND DEFINED WIN_DLL_PATH)
  message(STATUS "Installing required DLLs from: ${WIN_DLL_PATH}")

  set(REQUIRED_DLLS
    libfreetype-6.dll
    libgio-2.0-0.dll
    libgiomm-2.68-1.dll
    libgstbase-1.0-0.dll
    libgstreamer-1.0-0.dll
    libgtkmm-4.0-0.dll
    libstdc++-6.dll
    libtag-2.dll
    zlib1.dll
  )

  foreach(dll_name IN LISTS REQUIRED_DLLS)
    set(dll_path "${WIN_DLL_PATH}/${dll_name}")
    if(EXISTS "${dll_path}")
      install(FILES "${dll_path}" DESTINATION bin)
    else()
      message(WARNING "DLL not found: ${dll_path}")
    endif()
  endforeach()
endif()

include(CPack)
