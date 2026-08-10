add_executable(mini_pos
    main.cpp
    ui/MainWindow.h
    ui/MainWindow.cpp
)

target_link_libraries(mini_pos
    PRIVATE
        Qt5::Widgets
        Qt5::Sql
)

target_include_directories(mini_pos
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if(MINGW)
    target_compile_options(mini_pos PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()