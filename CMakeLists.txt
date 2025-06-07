# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.15)

# CPLEX configuration: allow manual override, otherwise try auto-detect
if (NOT DEFINED CPLEX_ROOT)
    file(GLOB CPLEX_STUDIO_DIRS
        "C:/Program Files/IBM/ILOG/CPLEX_Studio*"
        "C:/IBM/ILOG/CPLEX_Studio*"
    )
    if (CPLEX_STUDIO_DIRS)
        list(SORT CPLEX_STUDIO_DIRS)
        list(REVERSE CPLEX_STUDIO_DIRS)
        list(GET CPLEX_STUDIO_DIRS 0 CPLEX_ROOT)
        message(STATUS "Auto-detected CPLEX Studio at: ${CPLEX_ROOT}")
    else()
        message(FATAL_ERROR "CPLEX Studio not found. Set -DCPLEX_ROOT=... to specify manually.")
    endif()
else()
    message(STATUS "Using user-specified CPLEX_ROOT: ${CPLEX_ROOT}")
endif()

# Extract CPLEX version from CPLEX_ROOT path (e.g., CPLEX_Studio2212)
string(REGEX MATCH "CPLEX_Studio([0-9]+)" _ver_match "${CPLEX_ROOT}")
if(_ver_match)
    string(REGEX REPLACE "CPLEX_Studio" "" CPLEX_VERSION "${_ver_match}")
    message(STATUS "Detected CPLEX version: ${CPLEX_VERSION}")
else()
    message(WARNING "Could not extract CPLEX version from CPLEX_ROOT path.")
    set(CPLEX_VERSION "")
endif()

# Export CPLEX version for subdirectories
set(CPLEX_VERSION ${CPLEX_VERSION} CACHE INTERNAL "CPLEX Version")

if (NOT CPLEX_VERSION)
    message(FATAL_ERROR "CPLEX_VERSION is not set due to detection error.")
endif()

# CPLEX include/lib directories
set(CPLEX_INCLUDE_DIR "${CPLEX_ROOT}/cplex/include")
set(CONCERT_INCLUDE_DIR "${CPLEX_ROOT}/concert/include")
set(CPLEX_LIB_DIR "${CPLEX_ROOT}/cplex/lib/x64_windows_msvc14/stat_mda")
set(CONCERT_LIB_DIR "${CPLEX_ROOT}/concert/lib/x64_windows_msvc14/stat_mda")

# Apply include/lib settings
include_directories("${CPLEX_INCLUDE_DIR}" "${CONCERT_INCLUDE_DIR}")
link_directories("${CPLEX_LIB_DIR}" "${CONCERT_LIB_DIR}")

# Add CDT from submodule
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CDT/CDT/CMakeLists.txt")
    add_subdirectory(./CDT/CDT CDT)
    message(STATUS "CDT submodule found and included.")
else()
    message(WARNING "
        CDT submodule not found. Please initialize it using:
          git submodule update --init --recursive
        Then re-run CMake.
    ")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("graph-solvers-template")

include(CTest)
enable_testing()

# Include sub-projects.
add_subdirectory ("graph-solvers-template")

set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1)
