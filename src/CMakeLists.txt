add_library(ticketcounter_core
    AppLogic.cpp
    TicketStorage.cpp
    TimeService.cpp
)

target_include_directories(ticketcounter_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_executable(TicketCounter
    MainApp.cpp
    ui/MainPage.cpp
    ui/StatsPage.cpp
    ui/GraphPage.cpp
    ui/SettingsPage.cpp
    ui/HotkeyBinder.cpp
)

target_include_directories(TicketCounter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(TicketCounter PRIVATE
    ticketcounter_core
    PkgConfig::GTKMM
)

set_target_properties(TicketCounter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(ticketcounter_core PUBLIC nlohmann_json::nlohmann_json)

install(TARGETS TicketCounter RUNTIME DESTINATION bin)


