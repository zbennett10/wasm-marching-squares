#include <stdio.h>
#include <string.h>
#include <emscripten/emscripten.h>
#include "marching-squares.h"

/*
 * To compile jansson C JSON library using emscripten
 * Remove jansson CMakeCache.txt
 * emcmake cmake -DCMAKE_INSTALL_PREFIX:PATH=~/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1/emcc-lib .
 * emmake make VERBOSE=1
 * emmake make check
 * emmake make install
 * 
 * To compile with emscripten:
 * emcc marching-squares.c ./include/jansson-2.13.1/emcc-lib/lib/libjansson.a -Iinclude/jansson-2.13.1/emcc-lib/include -s EXPORTED_FUNCTIONS='["_generate_isolines_geojson", "_main"]' -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']" -s WASM=1 -s NO_EXIT_RUNTIME=1 -s LLD_REPORT_UNDEFINED -o geojson.html 
 */


EMSCRIPTEN_KEEPALIVE char* generate_isolines_geojson_(
    int raster_data_rows,
    int raster_data_cols,
    double raster_data[raster_data_rows][raster_data_cols],
    double geotransform[6],
    int interval_count,
    double intervals[interval_count]
) {
    return generate_isolines_geojson(
        raster_data_rows,
        raster_data_cols,
        raster_data,
        geotransform,
        interval_count,
        intervals
    );
}
