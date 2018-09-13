cmake_minimum_required(VERSION 3.0)

option(BUILD_GAME "Build the game" ON)
option(BUILD_DOCS "Build the documentation" OFF)
option(ENABLE_TESTS "Enable the tests" OFF)

project(Maze3D)

if(BUILD_GAME)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_CXX_FLAGS "-Wall -pedantic -std=c++14")

    set(SOURCE_FILES src/main.cpp)

    add_executable(Maze3D ${SOURCE_FILES})

    find_package(PkgConfig)

    set(DEPS_PATH ${CMAKE_SOURCE_DIR}/deps)

    # Search for a lib and set the correct link, includes pathes
    # target : the target's name
    # usePkgConfig : boolean : whether to use pkgconfig or not
    # prefix : the lib's prefix
    # module : the lib's name
    # libPath : the path to the lib
    # includePath : the path to the lib's include folder
    # staticOrShared : string : can be either "_STATIC" or "" depending on which way the libs are to be retrieved
    function(setupLib target usePkgConfig prefix module libPath includePath staticOrShared)
        if(${usePkgConfig})
            # search for the lib using pkgConfig
            pkg_search_module(${prefix} QUIET ${module})

            if(${${prefix}_FOUND})
                target_link_libraries(${target} PUBLIC ${${prefix}${staticOrShared}_LIBRARIES})
                target_include_directories(${target} PUBLIC ${${prefix}_INCLUDE_DIRS})
                target_compile_options(${target} PUBLIC ${${prefix}_CFLAGS_OTHER})
                return()
            else()
                message(WARNING "PkgConfig could not locate " ${prefix} ". Searching for it in deps folder")
            endif()
        endif()

        # if we reached this part, either pkgConfig is not used or the lib was
        # not found. Searching in the deps folder
        add_library(${prefix} STATIC IMPORTED)
        set_target_properties(${prefix} PROPERTIES
                IMPORTED_LOCATION ${libPath}
                INTERFACE_INCLUDE_DIRECTORIES ${includePath})

        target_link_libraries(${target} PUBLIC ${prefix})
        target_include_directories(${target} PUBLIC ${includePath})
    endfunction(setupLib)

    if(NOT WIN32)
        setupLib(Maze3D ${PKG_CONFIG_FOUND} OPENCV opencv ${DEPS_PATH}/opencv/lib/opencv.a ${DEPS_PATH}/opencv/include _STATIC)
#        setupLib(Maze3D ${PKG_CONFIG_FOUND} ASSIMP assimp ${DEPS_PATH}/assimp/lib/assimp.a ${DEPS_PATH}/assimp/include _STATIC)
#        setupLib(Maze3D ${PKG_CONFIG_FOUND} GLEW glew ${DEPS_PATH}/glew/lib/libGLEW.a ${DEPS_PATH}/glew/include _STATIC)
#        setupLib(Maze3D ${PKG_CONFIG_FOUND} GLFW glfw3 ${DEPS_PATH}/glfw/lib/libglfw3.a ${DEPS_PATH}/glfw/include _STATIC)

#        if(NOT EXISTS /usr/local/include/glm)
#            target_include_directories(Maze3D PUBLIC ${DEPS_PATH}/glm/include)
#        endif()
#
#        if(NOT EXISTS /usr/local/include/SOIL)
#            target_include_directories(Maze3D PUBLIC ${DEPS_PATH}/soil/include)
#        endif()

        if(UNIX AND NOT APPLE)
            setupLib(Maze3D ${PKG_CONFIG_FOUND} GL glu ${DEPS_PATH}/GL/glu.so ${DEPS_PATH}/GL/include "")
            target_include_directories(Maze3D PUBLIC ${PROJECT_SOURCE_DIR}/include)
        endif()

        if(APPLE)
            target_include_directories(Maze3D PUBLIC ${PROJECT_SOURCE_DIR}/include System/Library/Frameworks)
            find_library(COCOA_LIBRARY Cocoa)
            find_library(OpenGL_LIBRARY OpenGL)
            target_link_libraries(Maze3D PUBLIC "-framework QTKit" "-framework QuartzCore" "-framework AppKit" "-framework IOKit")

            set(OSX_LIBS ${COCOA_LIBRARY} ${OpenGL_LIBRARY})
        endif()

        # target_link_libraries(Maze3D PUBLIC ${OSX_LIBS} -lSOIL)
    else()
        set(OpenCV_DIR "C:\\opencv\\opencv-3.2.0\\build")

        find_package(OpenCV)
        target_include_directories(Maze3D PUBLIC ${OpenCV_INCLUDE_DIRS})
        target_link_libraries(Maze3D ${OpenCV_LIBS})
    endif()
endif()

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
