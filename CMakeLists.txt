#
#
# Windows prerequisites:
#
# vcpkg install --triplet x64-windows protobuf
# vcpkg install --triplet x64-windows pdcurses
# vcpkg install --triplet x64-windows wintoast
#
# mkdir build; cd build; cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\git\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
#
cmake_minimum_required(VERSION 3.28)
set(VERSION_INFO 1.0.1)
project(meshtastic-cli VERSION ${VERSION_INFO})

option(ENABLE_WINTOAST "Build with wintoast" OFF)

set(ARGTABLE3_SRC third-party/argtable3/argtable3.c)
set(DAEMONIZE_FILE_HELPER_SRC third-party/daemonize.cpp third-party/file-helper.cpp)

set(GEN ${CMAKE_CURRENT_SOURCE_DIR}/gen)
set(GEN_MESH_DIR ${GEN}/meshtastic)
set(GEN_SRC
    ${GEN_MESH_DIR}/admin.pb.cc
    ${GEN_MESH_DIR}/apponly.pb.cc
    ${GEN_MESH_DIR}/atak.pb.cc
    ${GEN_MESH_DIR}/cannedmessages.pb.cc
    ${GEN_MESH_DIR}/channel.pb.cc
    ${GEN_MESH_DIR}/clientonly.pb.cc
    ${GEN_MESH_DIR}/config.pb.cc
    ${GEN_MESH_DIR}/connection_status.pb.cc
    ${GEN_MESH_DIR}/deviceonly.pb.cc
    ${GEN_MESH_DIR}/device_ui.pb.cc
    ${GEN_MESH_DIR}/interdevice.pb.cc
    ${GEN_MESH_DIR}/localonly.pb.cc
    ${GEN_MESH_DIR}/mesh.pb.cc
    ${GEN_MESH_DIR}/module_config.pb.cc
    ${GEN_MESH_DIR}/mqtt.pb.cc
    ${GEN_MESH_DIR}/paxcount.pb.cc
    ${GEN_MESH_DIR}/portnums.pb.cc
    ${GEN_MESH_DIR}/powermon.pb.cc
    ${GEN_MESH_DIR}/remote_hardware.pb.cc
    ${GEN_MESH_DIR}/rtttl.pb.cc
    ${GEN_MESH_DIR}/storeforward.pb.cc
    ${GEN_MESH_DIR}/telemetry.pb.cc
    ${GEN_MESH_DIR}/xmodem.pb.cc
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # WinRT requires C++17 standard
    set(CMAKE_CXX_STANDARD 17)

    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

    find_package(unofficial-pdcurses CONFIG REQUIRED)
    set(LIB_CURSES unofficial::pdcurses::pdcurses)
    set(OS_SPECIFIC_LIBS wsock32 ws2_32 Userenv WindowsApp.lib)
    set(MeshtasticDevices_cpp MeshtasticDeviceBLE-win.cpp MeshtasticDeviceSerial-win.cpp)
    set(MeshtasticOSEnvironment_cpp MeshtasticOSEnvironment-win.cpp)
    set(MeshtasticOSTransports_cpp MeshtasticBLETransport-win.cpp MeshtasticSerialTransport-win.cpp)

    find_package(Protobuf CONFIG REQUIRED)
    set(LIB_PB protobuf::libprotobuf protobuf::libprotobuf-lite)
    set(INC_PB ${PROTOBUF_INCLUDE_DIRS})

    add_custom_command(
        OUTPUT ${GEN_SRC}
        COMMAND powershell ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate-code.ps1 ${CMAKE_CURRENT_SOURCE_DIR}
    )

    if (ENABLE_WINTOAST)
        find_package(unofficial-wintoast CONFIG REQUIRED)
        set(LIB_WINTOAST unofficial::wintoast::wintoast)
    endif()

else()
    set(LIB_PB protobuf)
    find_library(ncurses)
    set(LIB_CURSES ncurses)

    add_custom_command(
        OUTPUT ${GEN_SRC}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate-code ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

add_custom_target(GeneratedProtobuf DEPENDS ${GEN_SRC})

add_library(libmeshtastic-pb STATIC ${GEN_SRC})
add_dependencies(libmeshtastic-pb GeneratedProtobuf)

target_include_directories(libmeshtastic-pb PRIVATE third-party ${INC_PB} ${GEN})

set(SRC_LIB_MESHTASTIC
    MeshtasticClient.cpp
    MeshtasticTypes.cpp
    MeshtasticString.cpp BLEGuid.cpp
    MeshtasticEnvironmentEvent.cpp
    MeshtasticEnvironmentEventHandler.cpp
    MeshtasticEnvironment.cpp
    ${MeshtasticOSEnvironment_cpp}
    MeshtasticTransport.cpp
    ${MeshtasticOSTransports_cpp}
    MeshtasticDevice.cpp
    MeshtasticDeviceContext.cpp
    ${MeshtasticDevices_cpp}
    TransportNDeviceName.cpp
    MeshtasticDiscoveryEvent.cpp
    MeshtasticMessage.cpp
    PinCodeProvider.cpp ConsolePinCodeProvider.cpp
    # CLI
    ConsoleEventHandler.cpp
    # TUI
    DeviceListView.cpp
    TDeviceListView.cpp
    ${DAEMONIZE_FILE_HELPER_SRC}
    ${ARGTABLE3_SRC}
)

add_library(meshtastic STATIC ${SRC_LIB_MESHTASTIC})
# -fPIC
set_property(TARGET meshtastic PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(meshtastic PRIVATE ${TLNS_DEF})
target_include_directories(meshtastic PRIVATE "." "third-party" ${VCPKG_INC} ${INC_PB} ${GEN})
target_link_libraries(meshtastic PRIVATE ${OS_SPECIFIC_LIBS} )

# CLI client
add_executable(mtcli mtcli.cpp)
target_include_directories(mtcli PRIVATE third-party ${INC_PB} ${GEN})
target_link_libraries(mtcli PRIVATE meshtastic ${OS_SPECIFIC_LIBS} ${LIB_PB} ${LIB_WINTOAST} libmeshtastic-pb ${LIB_CURSES})

# logger client
add_executable(mt-log mt-log.cpp)
target_include_directories(mt-log PRIVATE third-party ${INC_PB} ${GEN})
target_link_libraries(mt-log PRIVATE meshtastic ${OS_SPECIFIC_LIBS} ${LIB_PB} ${LIB_WINTOAST} libmeshtastic-pb ${LIB_CURSES})

#
# Tests
#
add_subdirectory(tests)

# Print config
message("")
message("Options (on- enabled, off- disabled):")
message("")
message("-DENABLE_WINTOAST=${ENABLE_WINTOAST} \t enable wintoast")
message("")
message(" Windows: with -DENABLE_SQLITE=on must provide VCPKG options like -DCMAKE_TOOLCHAIN_FILE=C:\\git\\vcpkg\\")
message("   scripts\\buildsystems\\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows")
message("")
