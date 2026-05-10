cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(FACTORY_QT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-U_FORTIFY_SOURCE)
endif()

set(QT_QML_GENERATE_QMLLS_INI ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts Quick Qml)
find_package(muparser REQUIRED)

find_package(Curses REQUIRED)

qt_policy(SET QTP0004 NEW)

qt_add_executable(FactoryApp "resources.qrc")

# set_source_files_properties(src/gui/components/theme/Theme.qml
#     PROPERTIES QT_QML_SINGLETON_TYPE TRUE
# )

qt_add_qml_module(FactoryApp
    URI FACTORY_QT
    VERSION 1.0
    OUTPUT_DIRECTORY FACTORY_QT
    RESOURCE_PREFIX "/"
    NO_RESOURCE_TARGET_PATH

    # QML_FILES 
    # src/main.qml
    # src/gui/components/theme/Theme.qml
    # src/gui/components/App.qml
    # src/gui/components/BlockPort.qml
    # src/gui/components/ConnectionPath.qml
    # src/gui/components/PlaceMenu.qml
    # src/gui/components/Port.qml
    # src/gui/components/ExtractionBlock.qml
    # src/gui/components/ProductionBlock.qml
    # src/gui/components/Workspace.qml

    SOURCES
    src/main.cpp

    # CORE
    src/core/managers/AssetManager.h        src/core/managers/AssetManager.cpp
    src/core/managers/CliManager.h          src/core/managers/CliManager.cpp
    src/core/managers/GameLibrary.h         src/core/managers/GameLibrary.cpp
    src/core/managers/SessionManager.h      src/core/managers/SessionManager.cpp
            # src/core/managers/Solver.h              src/core/managers/Solver.cpp

    src/core/nodes/AbstractNode.h           src/core/nodes/AbstractNode.cpp
    src/core/nodes/ExtractionNode.h         src/core/nodes/ExtractionNode.cpp
    src/core/nodes/Factory.h                src/core/nodes/Factory.cpp
    src/core/nodes/FactoryEdgeNode.h        src/core/nodes/FactoryEdgeNode.cpp
    src/core/nodes/FactoryNode.h            src/core/nodes/FactoryNode.cpp
    src/core/nodes/MachineNode.h            src/core/nodes/MachineNode.cpp
    src/core/nodes/Port.h
    src/core/nodes/ProductionNode.h         src/core/nodes/ProductionNode.cpp
    #
    src/core/types/Types.h
    #
    #         # GUI
    src/gui/QmlReloader.h
    
    src/gui/controllers/SceneManager.h      src/gui/controllers/SceneManager.cpp
    src/gui/controllers/SceneModel.h        src/gui/controllers/SceneModel.cpp
    src/gui/controllers/RecipeListModel.h   src/gui/controllers/RecipeListModel.cpp
    #
    #         src/gui/bridges/ItemBridge.h
    #         src/gui/bridges/PortBridge.h
    #         src/gui/bridges/MachineBridge.h
    #         src/gui/bridges/RecipeBridge.h
    #
    #         src/gui/items/AbstractItem.h            src/gui/items/AbstractItem.cpp
    #         src/gui/items/Connection.h              src/gui/items/Connection.cpp
    #         src/gui/items/ExtractionItem.h
    #         src/gui/items/FactoryItem.h             src/gui/items/FactoryItem.cpp
    #         src/gui/items/ProductionItem.h          src/gui/items/ProductionItem.cpp
    #
    #         src/gui/models/RecipeModel.h            src/gui/models/RecipeModel.cpp
    #         src/gui/models/SceneModel.h             src/gui/models/SceneModel.cpp
)


target_link_libraries(FactoryApp PRIVATE
    Qt6::Widgets
    Qt6::Charts
    Qt6::Quick
    Qt6::Qml
    # ${CURSES_LIBRARIES}
    # muparser::muparser
)

target_include_directories(FactoryApp PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core/managers
    ${CMAKE_SOURCE_DIR}/src/core/nodes
    ${CMAKE_SOURCE_DIR}/src/gui/controllers
    # ${CMAKE_SOURCE_DIR}/src/gui/items
    # ${CMAKE_SOURCE_DIR}/src/gui/models
    # ${CMAKE_SOURCE_DIR}/src/gui/bridges
    # ${CURSES_INCLUDE_DIRS}
)

