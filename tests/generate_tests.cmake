cmake_policy(SET CMP0007 NEW)
file(READ "${INPUT_FILE}" test_list)
string(REGEX REPLACE "\r\n" "\n" test_list "${test_list}")
string(REGEX REPLACE "\n" ";" test_list "${test_list}")
string(REPLACE "\r" "\n" test_list "${test_list}")
list(REMOVE_ITEM test_list "")

file(WRITE ${OUTPUT_FILE}  "# Dynamically generated tests\n")
foreach(test_name IN LISTS test_list)
    file(APPEND "${OUTPUT_FILE}"
        "add_test(NAME ${test_name} COMMAND \"${TEST_EXECUTABLE}\" ${test_name})\n"
    )
endforeach()
