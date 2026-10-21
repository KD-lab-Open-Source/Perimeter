# ExternalProject may repeat its patch step after a reconfigure.
execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${DXVK_SOURCE_DIR}"
    apply --reverse --check "${PATCH_FILE}" RESULT_VARIABLE already_applied
    OUTPUT_QUIET ERROR_QUIET)
if(already_applied EQUAL 0)
    return()
endif()
execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${DXVK_SOURCE_DIR}"
    apply --check "${PATCH_FILE}" RESULT_VARIABLE check_result)
if(NOT check_result EQUAL 0)
    message(FATAL_ERROR "The macOS shader patch does not match this DXVK source tree")
endif()
execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${DXVK_SOURCE_DIR}"
    apply "${PATCH_FILE}" RESULT_VARIABLE apply_result)
if(NOT apply_result EQUAL 0)
    message(FATAL_ERROR "Could not apply the macOS DXVK shader patch")
endif()
