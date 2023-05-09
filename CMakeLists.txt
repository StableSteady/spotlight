cmake_minimum_required(VERSION 3.15)
project(spotlight)

set(CMAKE_SUPPRESS_REGENERATION true)

# Set the C++ standard to the latest version
set(CMAKE_CXX_STANDARD 20)

# Find and include the Qt 6 libraries
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
qt_standard_project_setup()

# Set the include directories for the project
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add the source files to the project
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    res/spotlight.rc
)

# Create the executable
add_executable(spotlight ${SOURCES})

# Link the Qt 6 libraries statically
set(QT_LIBRARIES Qt6::Core Qt6::Widgets)
target_link_libraries(spotlight PRIVATE ${QT_LIBRARIES})

set_target_properties(spotlight PROPERTIES
    WIN32_EXECUTABLE ON
)

install(TARGETS spotlight
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION bin
)

qt_generate_deploy_app_script(
    TARGET spotlight
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})