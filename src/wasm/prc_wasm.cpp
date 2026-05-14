#include <oPRCFile.h>

#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#define PRC_WASM_KEEPALIVE EMSCRIPTEN_KEEPALIVE
#else
#define PRC_WASM_KEEPALIVE
#endif

namespace {
std::vector<uint8_t> g_output;
std::string g_error;
const double kDefaultCreaseAngleDegrees = 25.8419;

void setError(const char* message)
{
    g_output.clear();
    g_error = message;
}
}

extern "C" {

PRC_WASM_KEEPALIVE int prc_generate_triangle_mesh(
    const double* vertices_xyz,
    uint32_t vertex_count,
    const uint32_t* triangle_indices,
    uint32_t triangle_count)
{
    g_output.clear();
    g_error.clear();

    if( vertices_xyz == nullptr )
    {
        setError( "vertices_xyz is null." );
        return 0;
    }
    if( triangle_indices == nullptr )
    {
        setError( "triangle_indices is null." );
        return 0;
    }
    if( vertex_count == 0 )
    {
        setError( "vertex_count must be greater than zero." );
        return 0;
    }
    if( triangle_count == 0 )
    {
        setError( "triangle_count must be greater than zero." );
        return 0;
    }
    if( vertex_count > ( std::numeric_limits<uint32_t>::max() / 3u ) )
    {
        setError( "vertex_count overflow." );
        return 0;
    }

    const uint64_t index_count = static_cast<uint64_t>( triangle_count ) * 3u;
    for( uint64_t i = 0; i < index_count; ++i )
    {
        if( triangle_indices[i] >= vertex_count )
        {
            setError( "triangle index out of range." );
            return 0;
        }
    }

    const double (*points)[3] = reinterpret_cast<const double (*)[3]>( vertices_xyz );
    const uint32_t (*triangles)[3] = reinterpret_cast<const uint32_t (*)[3]>( triangle_indices );

    std::ostringstream output( std::ios::out | std::ios::binary );
    oPRCFile file( output );
    const PRCmaterial material(
        RGBAColour( 0.1, 0.1, 0.1, 1.0 ),
        RGBAColour( 0.7, 0.7, 0.7, 1.0 ),
        RGBAColour( 0.0, 0.0, 0.0, 1.0 ),
        RGBAColour( 0.2, 0.2, 0.2, 1.0 ),
        1.0,
        0.05 );

    file.addTriangles(
        vertex_count, points,
        triangle_count, triangles,
        material,
        0, nullptr, nullptr,
        0, nullptr, nullptr,
        0, nullptr, nullptr,
        0, nullptr, nullptr,
        kDefaultCreaseAngleDegrees );

    if( !file.finish() )
    {
        setError( "PRC generation failed." );
        return 0;
    }

    const std::string data = output.str();
    g_output.assign( data.begin(), data.end() );

    if( g_output.empty() )
    {
        setError( "Generated PRC is empty." );
        return 0;
    }

    return 1;
}

PRC_WASM_KEEPALIVE const uint8_t* prc_get_output_ptr()
{
    if( g_output.empty() )
        return nullptr;
    return g_output.data();
}

PRC_WASM_KEEPALIVE uint32_t prc_get_output_size()
{
    return static_cast<uint32_t>( g_output.size() );
}

PRC_WASM_KEEPALIVE const char* prc_get_last_error()
{
    return g_error.c_str();
}

PRC_WASM_KEEPALIVE void prc_clear_output()
{
    g_output.clear();
    g_error.clear();
}

}
