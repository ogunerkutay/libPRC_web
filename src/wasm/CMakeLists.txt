if( NOT EMSCRIPTEN )
    return()
endif()

add_executable( prc_wasm
    prc_wasm.cpp
)

target_include_directories( prc_wasm PRIVATE
    ${PROJECT_SOURCE_DIR}/src/asymptote
)

target_link_libraries( prc_wasm PRIVATE
    asymptote
    ${ZLIB_LIBRARY}
)

set_target_properties( prc_wasm PROPERTIES
    OUTPUT_NAME "prc_wasm"
    SUFFIX ".js"
)

target_link_options( prc_wasm PRIVATE
    "-sWASM=1"
    "-sMODULARIZE=1"
    "-sEXPORT_ES6=1"
    "-sENVIRONMENT=web,worker"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sEXPORTED_FUNCTIONS=['_malloc','_free','_prc_generate_triangle_mesh','_prc_get_output_ptr','_prc_get_output_size','_prc_get_last_error','_prc_clear_output']"
    "-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap','UTF8ToString','getValue']"
)
