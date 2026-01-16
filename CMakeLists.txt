cmake_minimum_required(VERSION 3.19)
project(LegendaryOS LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets StateMachine)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/panels
)

set(CMAKE_AUTORCC ON)

qt_standard_project_setup()

qt_add_executable(LegendaryOS
    WIN32 MACOSX_BUNDLE
    main.cpp
    resources.qrc
    src/core/system.h
    src/core/system.cpp
    src/core/task.h
    src/core/task.cpp
    src/core/agent.h
    src/core/agent.cpp
    src/elements/fluidbutton.h
    src/elements/fluidbutton.cpp
    src/models/taskmodel.h
    src/models/taskmodel.cpp
    src/dialogs/taskedit.h
    src/dialogs/taskedit.cpp
    ui/dialogs/taskedit.ui
    src/models/agentmodel.h
    src/models/agentmodel.cpp
    src/dialogs/agentedit.h
    src/dialogs/agentedit.cpp
    ui/dialogs/agentedit.ui
    src/panels/mainpanel.h
    src/panels/mainpanel.cpp
    ui/panels/mainpanel.ui
    src/models/loggingmodel.h
    src/models/loggingmodel.cpp
    src/models/readymodel.h
    src/models/readymodel.cpp


)

target_link_libraries(LegendaryOS
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::StateMachine
)

include(GNUInstallDirs)

install(TARGETS LegendaryOS
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

target_include_directories(LegendaryOS
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

qt_generate_deploy_app_script(
    TARGET LegendaryOS
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
