cmake_minimum_required(VERSION 3.19)
project(LegendaryOS VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/panels
)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Widgets Test)

set(PROJECT_SOURCES
    main.cpp
    src/core/system.h
    src/core/system.cpp
    src/core/task.h
    src/core/task.cpp
    src/core/agent.h
    src/core/agent.cpp
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
    src/models/prioritymodel.cpp
    src/models/prioritymodel.h
    src/core/coordinator.h
    src/core/coordinator.cpp
    src/dialogs/settingsedit.h
    src/dialogs/settingsedit.cpp
    ui/dialogs/settingsedit.ui
    resources.qrc
    src/tests/simpletest.h
    src/tests/simpletest.cpp
    src/tests/dependencytest.h
    src/tests/dependencytest.cpp
    src/tests/stresstest.h
    src/tests/stresstest.cpp
)

qt_add_executable(LegendaryOS
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_include_directories(LegendaryOS
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(LegendaryOS
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
)

set_target_properties(LegendaryOS PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

include(GNUInstallDirs)

install(TARGETS LegendaryOS
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_finalize_executable(LegendaryOS)

# Deploy script generation removed for Qt 6.4 compatibility

# install(SCRIPT ${deploy_script})
