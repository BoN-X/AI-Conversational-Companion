cmake_minimum_required(VERSION 3.24)
project(virtual_gf LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6 6.5 COMPONENTS Widgets Network REQUIRED)

set(SRC_UI
  src/ui/qt_main.cpp
  src/ui/ChatWindow.cpp
  src/ui/ChatWindow.h
)
set(SRC_CORE
  src/core/types.h
  src/core/persona_engine.cpp
  src/core/persona_engine.h
)

add_executable(vgf_qt ${SRC_UI} ${SRC_CORE})
target_include_directories(vgf_qt PRIVATE src)
target_link_libraries(vgf_qt PRIVATE Qt6::Widgets Qt6::Network)

# 复制 assets 目录
add_custom_command(TARGET vgf_qt POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:vgf_qt>/assets)

