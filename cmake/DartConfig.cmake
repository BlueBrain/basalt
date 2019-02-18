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

find_program(MEMORYCHECK_COMMAND valgrind DOC "Memory checking utility (valgrind by default)")

set(
  MEMORYCHECK_COMMAND_OPTIONS
  "--tool=memcheck --track-origins=yes --leak-check=full --show-leak-kinds=all --verbose --gen-suppressions=all"
  CACHE STRING "Commands passed to valgrind")

# Look for suppressions files
file(GLOB dartconfig_supp_files ${CMAKE_SOURCE_DIR}/tests/memorycheck/*.supp)
foreach(supp_file IN LISTS dartconfig_supp_files)
  set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --suppressions=${supp_file}")
endforeach()
unset(dartconfig_supp_files)
