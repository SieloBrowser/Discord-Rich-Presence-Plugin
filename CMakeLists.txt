cmake_minimum_required(VERSION 3.6)
project(DiscordRichPresence)

# user config config file
include("${CMAKE_SOURCE_DIR}/user-config.cmake")

include_directories(${SIELO_DIR}/Core)
include_directories(${DRP_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/includes)

link_directories(${SIELO_LIBS_DIR})
link_directories(${DRP_LIBS_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(
    GLOB_RECURSE
    SOURCE_FILES
    includes/*.hpp
    sources/*.cpp
)

file(
	GLOB_RECURSE
	QRC_FILES
	*.qrc
)

find_package(Qt5Widgets 5.12.0 REQUIRED)
find_package(Qt5WebEngine 5.12.0 REQUIRED)
find_package(Qt5Sql 5.12.0 REQUIRED)
find_package(Qt5WebEngineWidgets 5.12.0 REQUIRED)

set(SIELO_LIBS discord-rpc SieloCore Qt5::Widgets Qt5::WebEngine Qt5::Sql Qt5::WebEngineWidgets)

add_library(DiscordRichPresence MODULE ${SOURCE_FILES} ${QRC_FILES})

target_link_libraries(DiscordRichPresence PRIVATE ${SIELO_LIBS})