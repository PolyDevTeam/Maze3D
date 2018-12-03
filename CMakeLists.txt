cmake_minimum_required(VERSION 3.0)

option(BUILD_GAME "Build the game" ON)
option(BUILD_DOCS "Build the documentation" OFF)
option(ENABLE_TESTS "Enable the tests" OFF)

project(Maze3D)

if(BUILD_GAME)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_CXX_FLAGS "-Wall -pedantic -std=c++11 -opencv")

    set(SOURCE_FILES
            src/main.cpp
			src/GL.cpp
			src/Box2D.cpp
			src/Bullet3D.cpp
            src/Pos.cpp
            src/Ball.cpp
            src/Map.cpp
            src/Wall.cpp
            src/Game.cpp
            src/Image.cpp
			src/CameraCalibration.cpp)

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
        find_package(OpenCV REQUIRED)
        target_link_libraries(Maze3D PUBLIC ${OpenCV_LIBS})
        #setupLib(Maze3D ${PKG_CONFIG_FOUND} OpenCV opencv ${OpenCV_LIBS} ${OpenCV_INCLUDE_DIRS} _STATIC)
        #setupLib(Maze3D ${PKG_CONFIG_FOUND} OPENCV opencv ${DEPS_PATH}/opencv/lib/opencv.a ${DEPS_PATH}/opencv/include _STATIC)
        setupLib(Maze3D ${PKG_CONFIG_FOUND} GLEW glew ${DEPS_PATH}/glew/lib/libGLEW.a ${DEPS_PATH}/glew/include _STATIC)
        setupLib(Maze3D ${PKG_CONFIG_FOUND} GLFW glfw3 ${DEPS_PATH}/glfw/lib/libglfw3.a ${DEPS_PATH}/glfw/include _STATIC)

        find_package(Bullet)
		include_directories($(BULLET_INCLUDE_DIRS))

        if(Bullet_FOUND)
            setupLib(Maze3D ${PKG_CONFIG_FOUND} BULLET bullet ${BULLET_LIBRARIES} ${Bullet_INCLUDE_DIRS} _STATIC)
#            target_link_libraries(Maze3D ${BULLET_LIBRARIES})
        else()
            message("BULLET NOT FOUND")
        endif()

        if(UNIX AND NOT APPLE)
            setupLib(Maze3D ${PKG_CONFIG_FOUND} GL glu ${DEPS_PATH}/GL/glu.so ${DEPS_PATH}/GL/include "")
            target_include_directories(Maze3D PUBLIC ${PROJECT_SOURCE_DIR}/include)
        endif()

        if(APPLE)
            #list(APPEND ${CMAKE_PREFIX_PATH} "/amuhome/s15022364/Desktop/deps")
            target_include_directories(Maze3D PUBLIC ${PROJECT_SOURCE_DIR}/include System/Library/Frameworks)
            find_library(COCOA_LIBRARY Cocoa)
            find_library(OpenGL_LIBRARY OpenGL)
            set(OSX_LIBS ${COCOA_LIBRARY} ${OpenGL_LIBRARY})
            target_link_libraries(Maze3D PUBLIC "-framework QTKit" "-framework QuartzCore" "-framework AppKit" "-framework IOKit")
            target_link_libraries(Maze3D PUBLIC ${OSX_LIBS})
        endif()

    else()
       set(OpenCV_DIR "C:\\opencv\\build")

       SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
            
       # OpenCV
       list(APPEND ${CMAKE_PREFIX_PATH} "C:\\opencv\\build")
       find_package(OpenCV REQUIRED)
       target_include_directories(Maze3D PUBLIC ${OpenCV_INCLUDE_DIRS})
       target_link_libraries(Maze3D ${OpenCV_LIBS})

	   #GL
	   find_package(OpenGL REQUIRED)
	   target_include_directories(Maze3D PUBLIC ${OPENGL_INCLUDE_DIRS})
	   target_link_libraries(Maze3D ${OPENGL_LIBRARIES})

	   #GL
	   find_package(OpenGL REQUIRED)
	   target_include_directories(Maze3D PUBLIC ${OPENGL_INCLUDE_DIRS})
	   target_link_libraries(Maze3D ${OPENGL_LIBRARIES})


	   #GLFW
	   SET(GLFW_PATH "C:\\glfw")
	   target_include_directories(Maze3D PUBLIC "${GLFW_PATH}\\include")
	   target_link_libraries(Maze3D "${GLFW_PATH}\\lib-vc2015\\glfw3.lib")

	   #BULLETPHYSICS
	   SET(BULLET_PATH "C:\\bulletphysics")
	   target_include_directories(Maze3D PUBLIC "${BULLET_PATH}\\src")
	   target_link_libraries(Maze3D "${BULLET_PATH}\\lib\\Release\\BulletCollision.lib")
	   target_link_libraries(Maze3D "${BULLET_PATH}\\lib\\Release\\BulletDynamics.lib")
	   target_link_libraries(Maze3D "${BULLET_PATH}\\lib\\Release\\LinearMath.lib")




    endif()
endif()

#if(ENABLE_TESTS)
#    add_subdirectory(tests)
#endif()
