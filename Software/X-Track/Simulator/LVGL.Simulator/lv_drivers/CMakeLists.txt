file(GLOB_RECURSE SOURCES ./*.c)
add_library(lv_drivers STATIC ${SOURCES})
