cmake_minimum_required(VERSION 3.13)

# Enable CMAKE_MSVC_RUNTIME_LIBRARY, requires CMake >=3.15
cmake_policy(SET CMP0091 NEW)

project(System3 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)

# In msys, `pkg-config --libs` returns `-lmingw32` but find_library() cannot
# find it in default search paths.
if (MSYS)
  list(APPEND CMAKE_LIBRARY_PATH $ENV{MINGW_PREFIX}/$ENV{MINGW_CHOST}/lib)
endif ()

if (MSVC)
  # Use statically-linked runtime library.
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

add_library(ymfm
  ../deps/ymfm/src/ymfm_opn.cpp
  ../deps/ymfm/src/ymfm_adpcm.cpp
  ../deps/ymfm/src/ymfm_ssg.cpp)
target_include_directories(ymfm PUBLIC ../deps/ymfm/src)


if (ANDROID)
  add_library(system3 SHARED)

  target_sources(system3 PRIVATE
    android/nact_android.cpp
    android/mako.cpp
    sys/mako_midi.cpp
    )
  target_link_libraries(system3 PRIVATE SDL2 SDL2_ttf ymfm)

elseif (EMSCRIPTEN)
  add_executable(system3)

  target_sources(system3 PRIVATE
    emscripten/nact_emscripten.cpp
    emscripten/mako.cpp
    )
  set(LIBS "SHELL:-s USE_SDL=2" "SHELL:-s USE_SDL_TTF=2")
  target_compile_options(system3 PRIVATE ${LIBS})
  target_link_options(system3 PRIVATE ${LIBS})
  target_link_libraries(system3 PRIVATE idbfs.js ymfm)

  # Without optimizations, Asyncify generates very large code.
  list(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG "-O1")

  target_link_options(system3 PRIVATE
    "SHELL:-s ENVIRONMENT=web"
    "SHELL:-s ASYNCIFY=1 -s ASYNCIFY_IGNORE_INDIRECT=1"
    "SHELL:-s ASYNCIFY_REMOVE=SDL_Delay"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:-s NO_EXIT_RUNTIME=1"
    "SHELL:-s ASYNCIFY_ADD=NACT::execute()"
    "SHELL:-s EXPORTED_RUNTIME_METHODS=getValue,addRunDependency,removeRunDependency")

else()  # NOT (ANDROID OR EMSCRIPTEN)
  add_executable(system3)
  target_link_libraries(system3 PRIVATE ymfm)

  if (MSVC)
    if (CMAKE_VS_PLATFORM_NAME STREQUAL "Win32")
      set(ARCH "x86")
    else()
      set(ARCH "x64")
    endif()

    include(FetchContent)
    FetchContent_Declare(sdl2
      URL https://github.com/libsdl-org/SDL/releases/download/release-2.0.22/SDL2-devel-2.0.22-VC.zip
      URL_HASH SHA1=efa040633c4faf8b006c0c1e552456ca4e5a3a53)
    FetchContent_GetProperties(sdl2)
    if (NOT sdl2_POPULATED)
      FetchContent_Populate(sdl2)
    endif()
    set(SDL2_LIB_DIR "${sdl2_SOURCE_DIR}/lib/${ARCH}")
    add_library(SDL2 SHARED IMPORTED)
    target_include_directories(SDL2 INTERFACE ${sdl2_SOURCE_DIR}/include)
    set_target_properties(SDL2 PROPERTIES
      IMPORTED_LOCATION ${SDL2_LIB_DIR}/SDL2.dll
      IMPORTED_IMPLIB ${SDL2_LIB_DIR}/SDL2.lib
      )
    target_link_libraries(SDL2 INTERFACE ${SDL2_LIB_DIR}/SDL2main.lib)

    FetchContent_Declare(sdl2_ttf
      URL https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.0.18/SDL2_ttf-devel-2.0.18-VC.zip
      URL_HASH SHA1=a421d47e9336ab722eac4ba107fab7f7b080eb4e)
    FetchContent_GetProperties(sdl2_ttf)
    if (NOT sdl2_ttf_POPULATED)
      FetchContent_Populate(sdl2_ttf)
    endif()
    set(SDL2TTF_LIB_DIR "${sdl2_ttf_SOURCE_DIR}/lib/${ARCH}")
    add_library(SDL2TTF SHARED IMPORTED)
    target_include_directories(SDL2TTF INTERFACE ${sdl2_ttf_SOURCE_DIR}/include)
    set_target_properties(SDL2TTF PROPERTIES
      IMPORTED_LOCATION ${SDL2TTF_LIB_DIR}/SDL2_ttf.dll
      IMPORTED_IMPLIB ${SDL2TTF_LIB_DIR}/SDL2_ttf.lib
      )

    target_compile_definitions(system3 PRIVATE _CRT_NONSTDC_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(system3 PRIVATE /utf-8 /wd4244)
    target_link_libraries(system3 PRIVATE SDL2 SDL2TTF)
    set_target_properties(system3 PROPERTIES
      WIN32_EXECUTABLE TRUE
      VS_DEBUGGER_ENVIRONMENT "PATH=${SDL2_LIB_DIR}$<SEMICOLON>${SDL2TTF_LIB_DIR}$<SEMICOLON>$ENV{PATH}")
    set_directory_properties(PROPERTIES VS_STARTUP_PROJECT system3)

    install(FILES
      ${SDL2_LIB_DIR}/SDL2.dll
      ${SDL2TTF_LIB_DIR}/SDL2_ttf.dll
      DESTINATION .)
    install(TARGETS system3 RUNTIME DESTINATION .)
  else()
    include(FindPkgConfig)
    pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
    pkg_check_modules(SDL2TTF REQUIRED IMPORTED_TARGET SDL2_ttf)
    target_link_libraries(system3 PRIVATE PkgConfig::SDL2 PkgConfig::SDL2TTF)
  endif()

  if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(system3 PRIVATE
      win/nact_win.cpp
      win/mako.cpp
      win/resource.rc
      sys/mako_midi.cpp
      )
    target_link_libraries(system3 PRIVATE winmm)
    if (MSYS)
      set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
    endif()
  else()
    pkg_check_modules(SDL2MIXER REQUIRED IMPORTED_TARGET SDL2_mixer)
    target_sources(system3 PRIVATE
      linux/nact_linux.cpp
      linux/mako.cpp
      sys/mako_midi.cpp
      )
    target_link_libraries(system3 PRIVATE PkgConfig::SDL2MIXER)
    set(RESOURCE_PATH ${CMAKE_INSTALL_PREFIX}/share/system3/)
    install(TARGETS system3 RUNTIME DESTINATION bin)
    install(DIRECTORY ../resources/
      DESTINATION share/system3
      FILES_MATCHING PATTERN *.MDA PATTERN *.ttf)
  endif()
endif()

target_sources(system3 PRIVATE
  sdlmain.cpp
  config.cpp
  fileio.cpp
  resource.cpp
  encoding.cpp
  texthook.cpp
  fm/makofm.cpp
  fm/mako_ymfm.cpp
  sys/ags.cpp
  sys/ags_bmp.cpp
  sys/ags_cursor.cpp
  sys/ags_draw.cpp
  sys/ags_gl3.cpp
  sys/ags_gm3.cpp
  sys/ags_pms.cpp
  sys/ags_text.cpp
  sys/ags_vsp.cpp
  sys/ags_vsp2l.cpp
  sys/ags_window.cpp
  sys/dri.cpp
  sys/msgskip.cpp
  sys/nact.cpp
  sys/nact_crc32.cpp
  sys/nact_input.cpp
  sys/nact_sys1.cpp
  sys/nact_sys2.cpp
  sys/nact_sys3.cpp
  )

if(NINTENDO_SWITCH)
  set(RESOURCE_PATH romfs:/)

  nx_generate_nacp(system3.nacp
    NAME "system3"
    AUTHOR "kichikuou"
    VERSION "0.8.0"
  )

  nx_create_nro(system3
    NACP system3.nacp
    ICON "../switch/icon.jpg"
    ROMFS "../resources"
  )
endif()

target_compile_definitions(system3 PRIVATE
  $<$<CONFIG:Debug>:_DEBUG_CONSOLE>
  RESOURCE_PATH="${RESOURCE_PATH}")
target_include_directories(system3 PRIVATE . sys)
