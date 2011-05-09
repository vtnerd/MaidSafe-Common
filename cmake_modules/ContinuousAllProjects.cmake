SET(SCRIPT_VERSION 1)
FILE(STRINGS "${CTEST_SCRIPT_NAME}" INSTALLED_VERSION_INFO LIMIT_COUNT 1)
STRING(REPLACE " " ";" INSTALLED_VERSION_INFO ${INSTALLED_VERSION_INFO})
LIST(GET INSTALLED_VERSION_INFO 1 INSTALLED_VERSION)
STRING(REPLACE ")" "" INSTALLED_VERSION ${INSTALLED_VERSION})
IF(NOT ${SCRIPT_VERSION} EQUAL ${INSTALLED_VERSION})
   MESSAGE("New installed Version: ${INSTALLED_VERSION}")
   MESSAGE("Current running version: ${SCRIPT_VERSION}")
   MESSAGE(FATAL_ERROR "This script is updated. Start the script again !!!")
ENDIF()
MESSAGE("Starting Script version: ${SCRIPT_VERSION}")

###############################################################################
#Pre-requirement:                                                             #
# All modules present under one directory.                                    #
# All modules installed under MaidSafe-Common\installed                       #
# This Script run from MaidSafe-Common\installed\share\maidsafe\cmake_modules #
###############################################################################

###############################################################################
# List of Modules and Paths                                                   #
###############################################################################
#Append/comment modules name here in order of dependency starting with MAIDSAFE_COMMON
#Test will run for below modules
SET(ALL_MODULE_LIST
    "MAIDSAFE_COMMON"
    "MAIDSAFE_DHT"
    "MAIDSAFE_ENCRYPT"
#   "PKI"
#   "PASSPORT"
#   "MAIDSAFE_PD"
#   "FILE_BROWSER"
#   "LIFESTUFF"
#   "LIFESTUFF_GUI"
#   "SIGMOID_CORE"
#   "DEDUPLICATOR_GAUGE"
    )

MESSAGE("================================================================================")
MESSAGE("Modules Selected For the test:")
FOREACH(EACH_MODULE ${ALL_MODULE_LIST})
  MESSAGE("${EACH_MODULE}")
ENDFOREACH()
MESSAGE("================================================================================")

###############################################################################
# Test configurations                                                         #
###############################################################################
#Module folder-name/path relative to TEST_ROOT_DIRECTORY
SET(MAIDSAFE_COMMON "MaidSafe-Common/maidsafe_common_lib")
SET(MAIDSAFE_DHT "MaidSafe-DHT")
SET(MAIDSAFE_ENCRYPT "MaidSafe-Encrypt")
SET(PKI PKI)
SET(PASSPORT Passport)
SET(MAIDSAFE_PD MaidSafe-PD)
SET(FILE_BROWSER File-Browser)
SET(LIFESTUFF LifeStuff)
SET(LIFESTUFF_GUI LifeStuff-GUI)
SET(SIGMOID_CORE SigmoidCore)
SET(DEDUPLICATOR_GAUGE Deduplicator-Gauge)

#List the Module if needs to be installed
SET(MAIDSAFE_COMMON_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(MAIDSAFE_DHT_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(MAIDSAFE_ENCRYPT_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(PKI_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(PASSPORT_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(PKI_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(MAIDSAFE_PD_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(FILE_BROWSER_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(LIFESTUFF_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
##############################################################################
#Variable(s) determined after running cmake                                  #
##############################################################################
SET(CTEST_CMAKE_GENERATOR "@CMAKE_GENERATOR@")

##############################################################################

SET(CTEST_BUILD_CONFIGURATION "Debug")
#SET(CTEST_BUILD_OPTIONS "-DWITH_SSH1=ON -WITH_SFTP=ON -DWITH_SERVER=ON -DWITH_ZLIB=ON -DWITH_PCAP=ON -DWITH_GCRYPT=OFF")
SET(CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)

###############################################################################
# Finding Modules Paths                                                       #
###############################################################################
## TEST_ROOT_DIRECTORY ##
SET(MAIDSAFE_TEST_ROOT_DIRECTORY ${CTEST_SCRIPT_DIRECTORY}/../../../../../)

FOREACH(EACH_MODULE ${ALL_MODULE_LIST})
  SET(${EACH_MODULE}_SOURCE_DIRECTORY ${MAIDSAFE_TEST_ROOT_DIRECTORY}/${${EACH_MODULE}})
  IF(WIN32)
    SET(${EACH_MODULE}_BINARY_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}/build/Win_MSVC)
  ELSEIF(UNIX)
    SET(${EACH_MODULE}_BINARY_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}/build/Linux/${CTEST_BUILD_CONFIGURATION})
  ENDIF()
  IF(EXISTS ${${EACH_MODULE}_SOURCE_DIRECTORY})
    MESSAGE("Found ${EACH_MODULE} at "${${EACH_MODULE}_SOURCE_DIRECTORY})
  ELSE()
    MESSAGE(FATAL_ERROR "Unable to find ${EACH_MODULE} source directry: ${${EACH_MODULE}_SOURCE_DIRECTORY}")
  ENDIF()

  IF(EXISTS ${${EACH_MODULE}_BINARY_DIRECTORY})
    MESSAGE("Found ${EACH_MODULE} Binary Directory at "${${EACH_MODULE}_BINARY_DIRECTORY})
  ELSE()
    MESSAGE(FATAL_ERROR "Unable to find ${EACH_MODULE} binary directry")
  ENDIF()
ENDFOREACH()

MESSAGE("================================================================================")

###############################################################################
#Finding hostname                                                             #
###############################################################################
FIND_PROGRAM(HOSTNAME_CMD NAMES hostname)
EXEC_PROGRAM(${HOSTNAME_CMD} ARGS OUTPUT_VARIABLE HOSTNAME)
SET(CTEST_SITE "${HOSTNAME}")
MESSAGE("Hostname: " ${HOSTNAME})

###############################################################################
# Finding Programs & Commands                                                 #
###############################################################################
FIND_PROGRAM(CTEST_CMAKE_COMMAND NAMES cmake)
IF(NOT CTEST_CMAKE_COMMAND)
  MESSAGE(FATAL_ERROR "Couldn't find cmake executable.Specify path of Ctest executable. \n e.g. -DCTEST_CMAKE_COMMAND=C:/Program Files/CMake 2.8/bin/cmake.exe")
ENDIF()

IF(WIN32)
  FIND_PROGRAM(CTEST_GIT_COMMAND NAMES git.cmd PATHS "D:/Program Files/Git/cmd" "C:/Program Files/Git/cmd")
ELSEIF(UNIX)
  FIND_PROGRAM(CTEST_GIT_COMMAND NAMES git)
ENDIF()
IF(NOT CTEST_GIT_COMMAND)
  MESSAGE(FATAL_ERROR "Couldn't find Git executable. Specify path as -DCTEST_GIT_COMMAND=C:/Program Files/Git/cmd/git.cmd")
ENDIF()

MESSAGE("Found Ctest executable at " ${CTEST_CMAKE_COMMAND})
MESSAGE("Found Git executable at " ${CTEST_GIT_COMMAND})

SET(CTEST_UPDATE_COMMAND "${CTEST_GIT_COMMAND}")

MESSAGE("================================================================================")

###############################################################################
# Utility Functions                                                           #
###############################################################################
FUNCTION(CHECK_UPDATE_STATUS_FOR_MODULE MODULE_NAME)
  SET(MODULE_SOURCE_DIRECTORY ${${MODULE_NAME}_SOURCE_DIRECTORY})
  SET(MODULE_BINARY_DIRECTORY ${${MODULE_NAME}_BINARY_DIRECTORY})
  MESSAGE("Checking updates for " ${MODULE_NAME})
  SET(CTEST_SOURCE_DIRECTORY ${MODULE_SOURCE_DIRECTORY})
  SET(CTEST_BINARY_DIRECTORY ${MODULE_BINARY_DIRECTORY})
  CTEST_START("Continuous")
  CTEST_UPDATE(SOURCE ${MODULE_SOURCE_DIRECTORY} RETURN_VALUE UPDATED_COUNT)
  SET(${MODULE_NAME}_UPDATED ${UPDATED_COUNT} PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(RUN_CONTINUOUS_ONCE MODULE_NAME)
  IF(${${MODULE_NAME}_UPDATED} GREATER 0)
    MESSAGE("${MODULE_NAME} repository changed.")
  ELSE()
    MESSAGE("No new updates for ${MODULE_NAME}.  Skipping test run.")
    RETURN()
  ENDIF()

  SET(MODULE_SOURCE_DIRECTORY ${${MODULE_NAME}_SOURCE_DIRECTORY})
  SET(MODULE_BINARY_DIRECTORY ${${MODULE_NAME}_BINARY_DIRECTORY})
  MESSAGE("Running Test for " ${MODULE_NAME})
  SET(CTEST_SOURCE_DIRECTORY ${MODULE_SOURCE_DIRECTORY})
  SET(CTEST_BINARY_DIRECTORY ${MODULE_BINARY_DIRECTORY})

  SET(CTEST_CONFIGURE_COMMAND "${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=${CTEST_BUILD_CONFIGURATION}")
  SET(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} -DWITH_TESTING:BOOL=ON ${CTEST_BUILD_OPTIONS}")
  SET(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} \"-G${CTEST_CMAKE_GENERATOR}\"")
  SET(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} \"${CTEST_SOURCE_DIRECTORY}\"")

#  EXECUTE_PROCESS(WORKING_DIRECTORY ${MODULE_BINARY_DIRECTORY}
#      COMMAND ${CTEST_CMAKE_COMMAND} ${MODULE_SOURCE_DIRECTORY}
#      RESULT_VARIABLE ret_var
#      OUTPUT_VARIABLE out_var
#      )
#  IF(NOT ${ret_var} EQUAL 0)
#    MESSAGE(FATAL_ERROR "  Running cmake for ${MODULE_NAME} returned ${ret_var}. Output : ${out_var}")
#  ELSE()
#    MESSAGE("  Ran cmake for ${MODULE_NAME}.")
#  ENDIF()

  CTEST_START("Continuous")
  CTEST_UPDATE(RETURN_VALUE UPDATED_COUNT)
  SET(${EACH_MODULE}_UPDATED ${UPDATED_COUNT} PARENT_SCOPE)
  IF(${${EACH_MODULE}_UPDATED} GREATER 0)
    RETURN()
  ENDIF()

#clean up module
  EXECUTE_PROCESS(WORKING_DIRECTORY ${MODULE_BINARY_DIRECTORY}
      COMMAND ${CTEST_CMAKE_COMMAND} --build . --target clean --config ${CTEST_BUILD_CONFIGURATION}
      RESULT_VARIABLE ret_var
      OUTPUT_VARIABLE out_var
      )
  IF(NOT ${ret_var} EQUAL 0)
    MESSAGE(FATAL_ERROR "  Cleaning ${MODULE_NAME} returned ${ret_var}.Output:${out_var}")
  ELSE()
    MESSAGE("  Cleaned ${MODULE_NAME}.")
  ENDIF()

  CTEST_CONFIGURE(RETURN_VALUE ret)
  IF(NOT ${ret} EQUAL 0)
    MESSAGE(FATAL_ERROR " CTEST_CONFIGURE failed ret:${ret_var}.")
  ENDIF()

  CTEST_BUILD(RETURN_VALUE ret)
  IF(NOT ${ret} EQUAL 0)
    MESSAGE(FATAL_ERROR " CTEST_BUILD failed ret:${ret_var}.")
  ENDIF()
  MESSAGE("  Built ${MODULE_NAME}.")

  CTEST_TEST()
  MESSAGE("  Ran all tests for ${MODULE_NAME}.")

  #Submitting test results to cdash
  CTEST_SUBMIT()

  #Installing if module needs installation
  IF(${MODULE_NAME}_SHOULD_BE_INSTALLED_AFTER_UPDATE)
    EXECUTE_PROCESS(WORKING_DIRECTORY ${MODULE_BINARY_DIRECTORY}
        COMMAND ${CTEST_CMAKE_COMMAND} --build . --config ${CTEST_BUILD_CONFIGURATION} --target install
        RESULT_VARIABLE ret_var
        OUTPUT_VARIABLE out_var
        )
    IF(NOT ${ret_var} EQUAL 0)
      MESSAGE(FATAL_ERROR "  Installing ${MODULE_NAME} returned ${ret_var} Output:${out_var}")
    ELSE()
      MESSAGE("  Installed ${MODULE_NAME}.")
    ENDIF()
  ENDIF()
  SET(${MODULE_NAME}_UPDATED 0 PARENT_SCOPE)
ENDFUNCTION()

###############################################################################
# TEST                                                                        #
###############################################################################
WHILE(1)
  FOREACH(EACH_MODULE ${ALL_MODULE_LIST})
    CHECK_UPDATE_STATUS_FOR_MODULE(${EACH_MODULE})
    IF(${${EACH_MODULE}_UPDATED} GREATER 0)
      MESSAGE("${EACH_MODULE} has changed; starting over again...")
      BREAK()
    ENDIF()
    RUN_CONTINUOUS_ONCE(${EACH_MODULE})
    IF(${${EACH_MODULE}_UPDATED} GREATER 0)
      MESSAGE("${EACH_MODULE} has changed; starting over again...")
      BREAK()
    ENDIF()
  ENDFOREACH()
  CTEST_SLEEP(60)
ENDWHILE()