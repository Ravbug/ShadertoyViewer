cmake_minimum_required(VERSION 3.16)
project(ShadertoyViewer)

set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${DEPS_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

# LTO on release and profile
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE CACHE INTERNAL "")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE CACHE INTERNAL "")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE CACHE INTERNAL "")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_PROFILE TRUE CACHE INTERNAL "")

# program & properties
file(GLOB SOURCES "src/*.cpp" "src/*.hpp")
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
source_group("Shaders" FILES ${shaders})

find_package(OpenGL REQUIRED)

# libraries
# turn off SDL features we don't need
set(SDL_HAPTIC OFF)
set(SDL_AUDIO OFF)
set(SDL_3DNOW OFF)
set(SDL_HIDAPI OFF)
set(SDL_HIDAPI_JOYSTICK OFF)
set(SDL_VULKAN OFF)
set(SDL_RENDER_METAL OFF)
set(SDL_RENDER_D3D OFF)
set(SDL_ASSEMBLY OFF)
set(SDL_ALTIVEC OFF)
set(SDL_DIRECTX OFF)
set(SDL_DISKAUDIO OFF)
set(SDL_DUMMYAUDIO OFF)
set(SDL_DUMMYVIDEO OFF)
set(SDL_METAL OFF)
set(SDL_MMX OFF)
set(SDL_VIRTUAL_JOYSTICK OFF)
set(SDL_WASAPI OFF)
set(SDL_SSE OFF)
add_subdirectory(deps/SDL2 EXCLUDE_FROM_ALL)
add_subdirectory(deps/glew EXCLUDE_FROM_ALL)
add_subdirectory(deps/fmt EXCLUDE_FROM_ALL)


# libpng
add_subdirectory(deps/libpng-cmake EXCLUDE_FROM_ALL)

# libcurl
if (MSVC)
    # use Windows native SSL instead of OpenSSL or MbedTLS
    set(CURL_USE_SCHANNEL ON CACHE INTERNAL "")
else()
    # for curl SSL
    set(ENABLE_TESTING OFF CACHE INTERNAL "")
    set(ENABLE_PROGRAMS OFF CACHE INTERNAL "")
    set(CURL_USE_MBEDTLS ON CACHE INTERNAL "")
    add_subdirectory(deps/mbedtls EXCLUDE_FROM_ALL)
    set(MBEDTLS_LIBRARIES "mbedtls")   
endif()
            
set(BUILD_TESTING OFF)
set(USE_OPENSSL OFF)
set(CURL_USE_LIBSSH2 OFF)
set(CURL_DISABLE_FTP ON)      # disable unused features & protocols
set(CURL_DISABLE_FILE ON)
set(CURL_DISABLE_TELNET ON)
set(CURL_DISABLE_LDAP ON)
set(CURL_DISABLE_DICT ON)
set(CURL_DISABLE_TFTP ON)
set(CURL_DISABLE_GOPHER ON)
set(CURL_DISABLE_IMAP ON)
set(CURL_DISABLE_SMB ON)
set(CURL_DISABLE_SMTP ON)
set(CURL_DISABLE_RTSP ON)
set(CURL_DISABLE_MQTT ON)
set(CURL_DISABLE_POP3 ON)
add_subdirectory(deps/curl EXCLUDE_FROM_ALL)

# imgui
add_subdirectory(deps/imgui-cmake)

# stb_image
add_subdirectory(deps/stb_image)

# link
target_link_libraries(${PROJECT_NAME} PUBLIC 
    SDL2main 
    "SDL2-static"
    glew
    fmt
    png_static
    imgui
    OpenGL::GL
    CURL::libcurl
    stb_image
)

target_include_directories(${PROJECT_NAME} PRIVATE "deps/glew/include" "deps/libpng-cmake/libpng/" "${CMAKE_BINARY_DIR}/deps/libpng-cmake/libpng" "deps/jsoncpp/include")
set_target_properties(${PROJECT_NAME} PROPERTIES 
    MACOSX_BUNDLE TRUE
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>"
    XCODE_GENERATE_SCHEME ON	# create a scheme in Xcode
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})