# ---------------
# .rst: DartConfig
# ---------------
#
# CMake config file for DART
#
# This sets the following variables:
#
# * ``MEMORYCHECK_COMMAND`` Location of memory checking utility
# * ``MEMORYCHECK_COMMAND_OPTIONS`` Variable used to pass commands to valgrind

find_program(MEMORYCHECK_COMMAND valgrind
             DOC "Memory checking utility (valgrind by default)")

set(
  MEMORYCHECK_COMMAND_OPTIONS
  "--tool=memcheck --track-origins=yes --leak-check=full --show-leak-kinds=all --verbose"
  CACHE STRING "Commands passed to valgrind")


file(GLOB DartConfig_SUPP_FILES
          ${CMAKE_CURRENT_SOURCE_DIR}/tests/ci/valgrind/*.supp)
foreach(supp_file IN LISTS DartConfig_SUPP_FILES)
  set(DartConfig_SUPP_LIST
      "${DartConfig_SUPP_LIST} --suppressions=${supp_file}")
endforeach()
set(MEMORYCHECK_COMMAND_OPTIONS
    "${MEMORYCHECK_COMMAND_OPTIONS} ${DartConfig_SUPP_LIST}")
unset(DartConfig_SUPP_FILES)
unset(DartConfig_SUPP_LIST)
