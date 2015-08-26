# Custom cmake module for kiui to find glfw

include(FindPackageHandleStandardArgs)

if (UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
else ()
    set(GLFW_INCLUDE_PATHS
            /usr/local/include
            /usr/X11/include
            /usr/include
            /opt/local/include
    )

    set(GLFW_LIB_PATHS
            /usr/local
            /usr/X11
            /usr
    )

    if (WIN32)
        set(GLFW_INCLUDE_PATHS $ENV{PROGRAMFILES}/GLFW/include ${GLFW_INCLUDE_PATHS})
        set(GLFW_LIB_PATHS $ENV{PROGRAMFILES}/GLFW/lib ${GLFW_LIB_PATHS})
    endif ()


    find_path(GLFW_INCLUDE_DIR NAMES GLFW/glfw3.h PATHS ${GLFW_INCLUDE_PATHS} NO_DEFAULT_PATH)

    find_library(GLFW_LIBRARIES NAMES glfw glfw3 PATHS ${GLFW_LIB_PATHS} PATH_SUFFIXES a lib64 lib NO_DEFAULT_PATH)

    find_package_handle_standard_args(GLFW DEFAULT_MSG GLFW_LIBRARIES GLFW_INCLUDE_DIR)
endif ()