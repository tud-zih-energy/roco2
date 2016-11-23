file(GLOB CFGS *)

FOREACH(CFG ${CFGS})
    add_subdirectory(${CFG})
ENDFOREACH()
