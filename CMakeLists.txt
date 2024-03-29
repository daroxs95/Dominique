cmake_minimum_required(VERSION 3.0.0)

project(Dominique VERSION 0.0.1)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set this definitions per terget and not global
add_definitions( -DAPP_VERSION="${CMAKE_PROJECT_VERSION}" )
add_definitions( -DCOMPILER_SPEC="${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}" )
add_definitions( -DAPP_NAME="${PROJECT_NAME}")
add_definitions( -DSDL_MAIN_HANDLED )#fix use of SDL, not fully tested on all platforms compilers, just on windows with MSVC and mingw

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(DENGINE_SHARED      off CACHE BOOL "Build shared library")
set(DENGINE_EXAMPLES    on CACHE BOOL "Build examples")

set(DX11_LINK_OPTIONS "")
set(GL_LINK_OPTIONS "")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(WIN32)
        message(INFO " detected GNU compiler in windows, passing flag -mconsole")
        set(CMAKE_EXE_LINKER_FLAGS "-mconsole")#console mode app// unrecognized by msvc
    endif()
endif()


include("collector/collector.cmake")#need to be after project()


include(CTest)

enable_testing()

set (CMAKE_CXX_STANDARD 17)


set(DOMINIQUE_SOURCES
    "src/dengine/core.cpp"
    "src/dengine/utils/fileLoader.cpp"
    "src/dengine/utils/uuid.cpp"
    "src/dengine/openGL/coreGL.cpp"
    "src/dengine/openGL/renderer/renderer.cpp"
    "src/dengine/spdlog_helper.cpp"
    "src/dengine/sdl_helpers.cpp"

    "3rdParty/cgltf/cgltf.cpp"
    "3rdParty/tinyobjloader/tiny_obj_loader.cc"
)

add_library(dominique_static STATIC
    ${DOMINIQUE_SOURCES}
)

if(DENGINE_SHARED)
    add_library(dominique SHARED
        ${DOMINIQUE_SOURCES}
    )
    target_compile_definitions(dominique PUBLIC -DDENGINE_EXPORTS)#this is not working TODO
endif(DENGINE_SHARED)

include_directories("3rdParty")
include_directories("src")

set(GLAD_INSTALL ON)
set(DIRECTX OFF)
append_var_to_cmake_args(DIRECTX)
append_var_to_cmake_args(GLAD_INSTALL)
collect(        "https://github.com/Dav1dde/glad"              "v0.1.34"           dominique_static )
collect(        "https://github.com/PotatoBite/xgetopt"        "v1.0.0"            dominique_static )
collect(        "https://github.com/ThePhD/sol2"               "v3.2.2"            dominique_static )
collect_src(    "https://github.com/g-truc/glm.git"            "0.9.9.8"           dominique_static )

collect_and_find( spdlog "https://github.com/gabime/spdlog"            "v1.8.5"    dominique_static )
collect_and_find( SDL2 "https://github.com/libsdl-org/SDL"      "release-2.0.14"   dominique_static )

set(GL_LINK_OPTIONS glad)

if(${SDL2_FOUND})
    if (WIN32)
        target_link_libraries (dominique_static PRIVATE SDL2::SDL2main SDL2::SDL2-static ${GL_LINK_OPTIONS} spdlog::spdlog ${DX11_LINK_OPTIONS} shell32)#for sdl_opengl3 by find package
    elseif(UNIX AND NOT APPLE)
        target_link_libraries (dominique_static PRIVATE SDL2 ${CMAKE_DL_LIBS} glad spdlog::spdlog)#for sdl_opengl3 by find package
    endif ()
endif()

if(DENGINE_SHARED AND ${SDL2_FOUND})
    if (WIN32)
        target_link_libraries (dominique PRIVATE SDL2::SDL2main SDL2::SDL2 ${GL_LINK_OPTIONS} spdlog::spdlog ${DX11_LINK_OPTIONS} shell32)#for sdl_opengl3 by find package
    elseif(UNIX AND NOT APPLE)
        target_link_libraries (dominique PRIVATE SDL2 ${CMAKE_DL_LIBS} glad spdlog::spdlog)#for sdl_opengl3 by find package
    endif ()
endif()



### Add some usable files to project build folder
set (RequiredExtras
    "TODO.txt"
)



include(GNUInstallDirs)

if(DENGINE_SHARED)
    set_target_properties(dominique
                        PROPERTIES 
                        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                        PDB_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/bin"
                        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    #preparing install step
    set_target_properties(dominique PROPERTIES RESOURCE "${RequiredExtras}")

    install(TARGETS dominique
        RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
        RESOURCE DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
        OBJECTS DESTINATION  ${CMAKE_INSTALL_LIBDIR}
    )

endif(DENGINE_SHARED)


set_target_properties(dominique_static
                      PROPERTIES 
                      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      PDB_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/bin"
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS dominique_static
    RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
    RESOURCE DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
    OBJECTS DESTINATION  ${CMAKE_INSTALL_LIBDIR}
)


#installing headers
install(DIRECTORY "./src/" # source directory
        DESTINATION "include" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.h" # select header files
		#PATTERN "*.c" # select header files
		#PATTERN "*.cpp" # select header files
)


if(DENGINE_EXAMPLES)
    # add examples
    include("examples/examples.cmake")
endif(DENGINE_EXAMPLES)