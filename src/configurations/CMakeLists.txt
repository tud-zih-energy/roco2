file(GLOB CFGS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *)

message("cfgs: ${CFGS}")

FOREACH(CFG ${CFGS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${CFG}")
        option("BUILD_CFG_${CFG}" "Was weiss ich denn" OFF)
        if(BUILD_CFG_${CFG})
            add_subdirectory(${CFG})
        endif()
    endif()
ENDFOREACH()
