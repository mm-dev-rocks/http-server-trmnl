# idf_component_register is ESP-IDF's own CMake function (brought in by the include line in the parent file) for registering a component — ESP-IDF thinks of code as components rather than just files.
idf_component_register(
    SRCS
        "main.c"
        "../../http_parse.c"
        "../../test_http_parse.c"
    INCLUDE_DIRS
        # Will find *.h headers here
        "../../"
)
