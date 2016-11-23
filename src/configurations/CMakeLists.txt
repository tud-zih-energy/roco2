file(GLOB CFGS *)

FOREACH(CFG ${CFGS})
    if(IS_DIRECTORY ${CFG})
        add_subdirectory(${CFG})
    endif()
ENDFOREACH()
