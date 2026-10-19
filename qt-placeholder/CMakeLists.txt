cmake_minimum_required(VERSION 3.16)
project(LangTrainer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(LangTrainer
    main.cpp
    verbdata.cpp
    verbdata.h
    verbselectiondialog.cpp
    verbselectiondialog.h
    verbexercisedialog.cpp
    verbexercisedialog.h
    resources.qrc
)
target_link_libraries(LangTrainer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
