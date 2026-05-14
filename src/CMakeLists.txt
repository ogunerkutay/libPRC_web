add_subdirectory( asymptote )
add_subdirectory( libPRC )
add_subdirectory( wasm )

if( OPENSCENEGRAPH_FOUND )
    add_subdirectory( osgdb_prc )
endif()

add_subdirectory( tools )
