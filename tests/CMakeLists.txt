cmake_minimum_required(VERSION 3.28)

set(VERSION_INFO 1.0.1)

project("meshtastic-test" VERSION ${VERSION_INFO})

# WinRT requires C++17 standard
set(CMAKE_CXX_STANDARD 17)

set(TEST_INCS .. ../third-party ${INC_PB} ${GEN})
set(TEST_LIBS meshtastic ${LIB_PB} libmeshtastic-pb ${LIB_CURSES})

add_executable(test-discover-ble test-discover-ble.cpp)
target_include_directories(test-discover-ble PRIVATE ${TEST_INCS})
target_link_libraries(test-discover-ble PRIVATE ${TEST_LIBS})
add_test(NAME test-discover-ble COMMAND "test-discover-ble")

add_executable(test-discover-serial test-discover-serial.cpp)
target_include_directories(test-discover-serial PRIVATE ${TEST_INCS})
target_link_libraries(test-discover-serial PRIVATE ${TEST_LIBS})
add_test(NAME test-discover-serial COMMAND "test-discover-serial")

add_executable(test-write-serial test-write-serial.cpp)
target_include_directories(test-write-serial PRIVATE ${TEST_INCS})
target_link_libraries(test-write-serial PRIVATE ${TEST_LIBS})
add_test(NAME test-write-serial COMMAND "test-write-serial")

add_executable(test-read-serial test-read-serial.cpp)
target_include_directories(test-read-serial PRIVATE ${TEST_INCS})
target_link_libraries(test-read-serial PRIVATE ${TEST_LIBS})
add_test(NAME test-read-serial COMMAND "test-read-serial")

add_executable(test-read-ble test-read-ble.cpp)
target_include_directories(test-read-ble PRIVATE ${TEST_INCS})
target_link_libraries(test-read-ble PRIVATE ${TEST_LIBS})
add_test(NAME test-read-ble COMMAND "test-read-ble")

add_executable(test-pair-ble test-pair-ble.cpp)
target_include_directories(test-pair-ble PRIVATE ${TEST_INCS})
target_link_libraries(test-pair-ble PRIVATE ${TEST_LIBS})
add_test(NAME test-pair-ble COMMAND "test-pair-ble")

add_executable(test-unpair-ble test-unpair-ble.cpp)
target_include_directories(test-unpair-ble PRIVATE ${TEST_INCS})
target_link_libraries(test-unpair-ble PRIVATE ${TEST_LIBS})
add_test(NAME test-unpair-ble COMMAND "test-unpair-ble")

add_executable(test-is-serial-meshtatsic test-is-serial-meshtastic.cpp)
target_include_directories(test-is-serial-meshtatsic PRIVATE ${TEST_INCS})
target_link_libraries(test-is-serial-meshtatsic PRIVATE ${TEST_LIBS})
add_test(NAME test-is-serial-meshtatsic COMMAND "test-discover-serial")

if (ENABLE_WINTOAST)
#   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:wmainCRTStartup")
    add_executable(test-wintoast test-wintoast.cpp)
    target_include_directories(test-wintoast PRIVATE ${TEST_INCS})
    target_link_libraries(test-wintoast PRIVATE ${TEST_LIBS} ${LIB_WINTOAST})
    add_test(NAME test-wintoast COMMAND "test-wintoast")
endif()
