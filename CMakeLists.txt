cmake_minimum_required(VERSION 3.16)

project(gpg-text LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Qml Quick Widgets)

qt5_add_resources(QML_RESOURCES
    qml.qrc
)

add_executable(gpg-text
    main.cpp
    GpgBackend.cpp
    GpgBackend.h
    ${QML_RESOURCES}
)

target_link_libraries(gpg-text
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::Widgets
)

install(TARGETS gpg-text
    RUNTIME DESTINATION .
)

install(FILES
    assets/gpg-text.svg
    DESTINATION assets
)

install(FILES
    manifest.json
    gpg-text.desktop
    gpg-text.apparmor
    gpg-text-contenthub.json
    DESTINATION .
)
