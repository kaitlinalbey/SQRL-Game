cmake_minimum_required(VERSION 3.21)
project(CppSdl2Box2dTinyxml2Starter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Create executable
add_executable(
    demo 
    src/main.cpp 
    src/Game.h
    src/Game.cpp
    src/Graphics.h
    src/Graphics.cpp
    src/Input.h
    src/Input.cpp
    src/View.h
    src/View.cpp
    src/Component.h
    src/GameObject.h
    src/GameObject.cpp
    src/BodyComponent.h
    src/BodyComponent.cpp
    src/SpriteComponent.h
    src/SpriteComponent.cpp
    src/ControllerComponent.h
    src/ControllerComponent.cpp
    src/BehaviorComponent.h
    src/BehaviorComponent.cpp
    src/ObjectFactory.h
    src/ObjectFactory.cpp
)

# Link libraries
target_link_libraries(demo PRIVATE
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
    SDL2_image::SDL2_image
    tinyxml2::tinyxml2
)

# Define SDL_MAIN_HANDLED for MinGW
target_compile_definitions(demo PRIVATE SDL_MAIN_HANDLED)

# Copy assets and DLLs
add_custom_command(TARGET demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:demo>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:demo> $<TARGET_FILE_DIR:demo>
    COMMAND_EXPAND_LISTS
)
