#
# Windows prerequisites:
# vcpkg install --triplet x64-windows protobuf
# vcpkg install --triplet x64-windows pdcurses
# mkdir build; cd build; cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\git\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
#
cmake_minimum_required(VERSION 3.28)
set(VERSION_INFO 1.0.1)
project(meshtastic-cli VERSION ${VERSION_INFO})

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
    meshtastic-cli.cpp
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

add_executable(meshtastic-cli meshtastic-cli.cpp)
target_include_directories(meshtastic-cli PRIVATE third-party ${INC_PB} ${GEN})
target_link_libraries(meshtastic-cli PRIVATE meshtastic ${OS_SPECIFIC_LIBS} ${LIB_PB} libmeshtastic-pb ${LIB_CURSES})

#
# Tests
#
add_subdirectory(tests)

# Print config
message("")
message("Options (on- enabled, off- disabled):")
message("")
message("")
message(" Windows: with -DENABLE_SQLITE=on must provide VCPKG options like -DCMAKE_TOOLCHAIN_FILE=C:\\git\\vcpkg\\")
message("   scripts\\buildsystems\\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows")
message("")
