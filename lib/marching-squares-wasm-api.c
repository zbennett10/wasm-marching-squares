#include <stdio.h>
#include <string.h>
#include <emscripten/emscripten.h>
#include "marching-squares.h"

/**
 * Exported WASM API defined below
 **/

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
