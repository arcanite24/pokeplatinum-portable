cmake_minimum_required(VERSION 3.20)
project(PokemonPlatinum C)

# Project version
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

# Build options
option(BUILD_DS_VERSION "Build for Nintendo DS (requires NitroSDK)" OFF)
option(BUILD_SDL_VERSION "Build SDL3 portable version" ON)

# C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Export compile commands for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =============================================================================
# SDL3 Build Target
# =============================================================================

if(BUILD_SDL_VERSION)
    message(STATUS "Configuring SDL3 portable build")
    
    # Find SDL3
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
    
    # Platform abstraction layer sources
    set(PAL_SDL_SOURCES
        src/platform/sdl/pal_graphics_sdl.c
        src/platform/sdl/pal_input_sdl.c
        src/platform/sdl/pal_audio_sdl.c
        src/platform/sdl/pal_file_sdl.c
        src/platform/sdl/pal_timer_sdl.c
        src/platform/sdl/pal_memory_sdl.c
        src/platform/sdl/pal_background_sdl.c
        src/platform/sdl/pal_sprite_sdl.c
        src/platform/sdl/pal_3d_sdl.c
        src/platform/sdl/main_sdl.c
    )
    
    # Game source files needed for SDL build
    # Adding all essential game system files
    file(GLOB_RECURSE GAME_SOURCES
        "src/*.c"
    )
    
    # Exclude overlay files for now (loaded dynamically on DS)
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/overlay[0-9]+/.*")
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/applications/.*")
    
    # Exclude platform-specific files
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/platform/.*")
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/main.c")

    # Exclude problematic files (NNS dependencies, etc.)
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/easy3d.*")
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/cutscenes/.*")
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/dw_warp/.*")
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/enc_effects.c")
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/error_handling.c")
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/dynamic_map_features.c")
    
    # Exclude Battle System (Phase 4/5) - Too many pointer/int cast issues for now
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/battle/.*")
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/battle_anim/.*")
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/field_battle_data_transfer.c")
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/error_message_reset.c")
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/comm_player_manager.c")
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/evolution.c")
    list(FILTER GAME_SOURCES EXCLUDE REGEX "src/field_comm_manager.c")
    
    # Create SDL executable
    add_executable(pokeplatinum_sdl ${PAL_SDL_SOURCES} ${GAME_SOURCES})
    
    # Include directories
    target_include_directories(pokeplatinum_sdl PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/platform
        ${CMAKE_SOURCE_DIR}/build
        ${CMAKE_SOURCE_DIR}/lib/spl/include
        ${CMAKE_SOURCE_DIR}/lib/gds/include
    )
    
    # Compile definitions
    target_compile_definitions(pokeplatinum_sdl PRIVATE
        PLATFORM_SDL
        TARGET_SDL
        POKEPLATINUM_GENERATED_ENUM
    )
    
    # Compiler warnings - suppress int-conversion for NULL usage
    # The DS codebase uses NULL for integer 0 in many places (147+ instances)
    # This is technically questionable but harmless and pervasive
    target_compile_options(pokeplatinum_sdl PRIVATE
        -w
        -Wno-int-conversion
    )
    
    # Link SDL3
    target_link_libraries(pokeplatinum_sdl PRIVATE SDL3::SDL3)
    
    # Platform-specific settings
    if(WIN32)
        # Windows-specific settings
        target_compile_definitions(pokeplatinum_sdl PRIVATE _CRT_SECURE_NO_WARNINGS)
        # Disable console window for Release builds
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            set_target_properties(pokeplatinum_sdl PROPERTIES WIN32_EXECUTABLE TRUE)
        endif()
    elseif(APPLE)
        # macOS-specific settings
        target_compile_options(pokeplatinum_sdl PRIVATE -Wall -Wextra)
    elseif(UNIX)
        # Linux-specific settings
        target_compile_options(pokeplatinum_sdl PRIVATE -Wall -Wextra)
        target_link_libraries(pokeplatinum_sdl PRIVATE m pthread)
    endif()
    
    # Installation
    install(TARGETS pokeplatinum_sdl DESTINATION bin)
    
    message(STATUS "SDL3 build configured successfully")
    message(STATUS "Executable: pokeplatinum_sdl")
endif()

# =============================================================================
# Nintendo DS Build Target (Placeholder)
# =============================================================================

if(BUILD_DS_VERSION)
    message(STATUS "Nintendo DS build not yet supported via CMake")
    message(STATUS "Please use the existing Meson build system for DS builds")
    message(STATUS "Run: meson setup build && ninja -C build")
endif()

# =============================================================================
# Documentation
# =============================================================================

# Print build configuration
message(STATUS "")
message(STATUS "========================================")
message(STATUS "Pokemon Platinum Build Configuration")
message(STATUS "========================================")
message(STATUS "Version: ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "DS Build: ${BUILD_DS_VERSION}")
message(STATUS "SDL Build: ${BUILD_SDL_VERSION}")
message(STATUS "========================================")
message(STATUS "")
