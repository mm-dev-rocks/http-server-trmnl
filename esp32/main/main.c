/*
 * ESP32 entry point — app_main() is called by ESP-IDF after hardware init.
 * On hosted platforms, test_http_parse.c defines main() instead.
 * See test_http_parse.c for the conditional.
 *
 * 'hosted' = with an OS (so filesystem, memory management, stdin/stdout etc)
 * 'freestanding' = without the above (eg esp32)
 */

// Nothing needed here — app_main() is defined in test_http_parse.c
