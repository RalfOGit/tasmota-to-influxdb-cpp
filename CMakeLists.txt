project(tasmota-to-influxdb-cpp)

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

# This CMake configuration assumes the following directory layout
#
# - tasmota-to-influxdb-cpp
#   - libtasmota
#   - <relative build path>
# - libcurl (windows only; for linux it is installed in /usr/lib64)
#   - include
#   - lib
# - influxdb-cxx
#   - include
#   - lib
#   - <relative install path>

# determine relative build and install paths
message("CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}")
message("CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}")
message("CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}")

string(REPLACE "${CMAKE_SOURCE_DIR}/" "" RELATIVE_BUILD_PATH "${CMAKE_BINARY_DIR}")
message("RELATIVE_BUILD_PATH ${RELATIVE_BUILD_PATH}")
string(REPLACE "${CMAKE_SOURCE_DIR}/" "" RELATIVE_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}")
message("RELATIVE_INSTALL_PATH ${RELATIVE_INSTALL_PATH}")

# path definitions for curl (adapt to your needs)
if (MSVC)
set(CURL_PROJECT_DIR "${CMAKE_SOURCE_DIR}/../libcurl")
set(CURL_LIBRARY_DIR "${CURL_PROJECT_DIR}/lib")
set(CURL_LIBRARY     "${CURL_LIBRARY_DIR}/libcurl.dll.a")
set(CURL_INCLUDE_DIR "${CURL_PROJECT_DIR}/include")
else()
set(CURL_LIBRARY_DIR "/usr/lib64")
set(CURL_LIBRARY     "/usr/lib64/libcurl.so")
set(CURL_INCLUDE_DIR "/usr/include/curl")
endif()
find_package(CURL REQUIRED MODULE)

# path definitions for InfluxDB (adapt to your needs)
set(INFLUX_PROJECT_DIR "${CMAKE_SOURCE_DIR}/../influxdb-cxx")
set(INFLUX_INSTALL_DIR "${INFLUX_PROJECT_DIR}/${RELATIVE_INSTALL_PATH}")
set(INFLUX_INCLUDE_DIR "${INFLUX_INSTALL_DIR}/include")
set(INFLUX_LIBRARY_DIR "${INFLUX_INSTALL_DIR}/lib")
#set(InfluxDB_DIR       "${INFLUX_INSTALL_DIR}/lib/cmake/InfluxDB/")
#find_package(InfluxDB)

# path definitions for libspeedwire (adapt to your needs)
set(TASMOTA_DIR         "${CMAKE_SOURCE_DIR}/libtasmota")
set(TASMOTA_INCLUDE_DIR "${TASMOTA_DIR}/include")
set(TASMOTA_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/${RELATIVE_BUILD_PATH}/libtasmota")
add_subdirectory("libtasmota" EXCLUDE_FROM_ALL)

# project sources and include path
set(PROJECT_SOURCES
    src/InfluxDBProducer.cpp
    src/main.cpp
)
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# build configuration
link_directories(${TASMOTA_LIBRARY_DIR} ${INFLUX_LIBRARY_DIR} ${CURL_LIBRARY_DIR})

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
add_dependencies(${PROJECT_NAME} tasmota)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDE_DIR} ${TASMOTA_INCLUDE_DIR} ${INFLUX_INCLUDE_DIR})
target_include_directories(tasmota PUBLIC ${TASMOTA_INCLUDE_DIR})

if (MSVC)
target_link_libraries(${PROJECT_NAME} tasmota.lib InfluxDB.lib ${CURL_LIBRARY} ws2_32.lib Iphlpapi.lib)
else()
target_link_libraries(${PROJECT_NAME} libtasmota.a libInfluxDB.so)
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES OUTPUT_NAME ${PROJECT_NAME}
)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
