cmake_minimum_required(VERSION 3.14)
project(fp)
set(CMAKE_CXX_STANDARD 17)
set(SOURCE_FILES main.cpp FPEngine.cpp FPEngine.h Cart.cpp Cart.h SirByzler.cpp SirByzler.h)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Windows with MinGW Installations
if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND MINGW )
    # if working on Windows but not in the lab
    # update the include directory location
    include_directories("C:/mingw64/include")
    # update the lib directory location
    target_link_directories(${PROJECT_NAME} PUBLIC "C:/mingw64/lib")
    target_link_libraries(${PROJECT_NAME} opengl32 glfw3 glad gdi32)
# OS X Installations
elseif( APPLE AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    # update the include directory location
    include_directories("/usr/local/include")
    # update the lib directory location
    target_link_directories(${PROJECT_NAME} PUBLIC "/usr/local/lib")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" glfw3 glad)
# Blanket *nix Installations
elseif( UNIX AND ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    # update the include directory location
    include_directories("/usr/local/include")
    # update the lib directory location
    target_link_directories(${PROJECT_NAME} PUBLIC "/usr/local/lib")
    target_link_libraries(${PROJECT_NAME} GL glfw glad)
endif()