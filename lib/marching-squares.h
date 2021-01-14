#include <stdbool.h>
#include <jansson.h>

#ifndef MARCHING_SQUARES
#define MARCHING_SQUARES

#define GEN_4_BIT_CONTOUR_STATE(a, b, c, d) a * 1 + b * 2 + c * 4 + d * 8

#define INTERPOLATE_CONTOUR_POS(threshold, y0, y1) ((threshold - y0) / (y1 - y0))

#define INVALID_RASTER_DATA(corners) (isnan(corners.top_left) || isnan(corners.top_right) || isnan(corners.bottom_right) || isnan(corners.bottom_left))

#define IS_SADDLE(cell_value_a, cell_value_b) (cell_value_a == 5 || cell_value_b == 10 ? true : false)

#define CELL_NO_EDGES(contour_state) (contour_state == 0 || contour_state == 15 ? true : false)

#define CELL_MISSING_DATA(cell) (cell.contour_state == 0 && cell.top == 0 && cell.left == 0 && cell.bottom == 0 && cell.right == 0)

#define EMPTY_POINT_VALUE -999

#define EMPTY_POINT (Point){EMPTY_POINT_VALUE, EMPTY_POINT_VALUE} 

#define EMPTY_CELL (ContourGridCell){ -999999999, -1, false, 0, 0, 0, 0 }

typedef struct {
    double x;
    double y;
} Point;

typedef struct MultiLine {
    Point point;
    struct MultiLine *next;
    bool mergable;
} MultiLine;

typedef struct {
    double threshold;
    int contour_state;
    bool is_saddle;
    double top;
    double bottom;
    double left;
    double right;
} ContourGridCell;

typedef struct {
    double top_left;
    double top_right;
    double bottom_left;
    double bottom_right;
} RasterCellCorners;

char* generate_isolines_geojson(
    int raster_data_rows,
    int raster_data_cols,
    double raster_data[raster_data_rows][raster_data_cols],
    double geotransform[6],
    int interval_count,
    double intervals[interval_count]
);

char* generate_isolines_geojson_(
    int raster_data_rows,
    int raster_data_cols,
    double raster_data[raster_data_rows][raster_data_cols],
    double geotransform[6],
    int interval_count,
    double intervals[interval_count]
);

MultiLine* gen_path(int grid_cols, int row_count, ContourGridCell (*cells)[grid_cols], int row, int col); 

Point gen_point_from_edge(ContourGridCell *cell, char* edge);

Point apply_geo_transformation_to_pixel_coordinates(double x, double y, double geotransform[6]);

MultiLine* get_segment_at_index(MultiLine *path, int i);

void add_multilines_to_geojson(int path_count, MultiLine **paths, json_t *coordinates, double geotransform[6]);

int get_line_count(MultiLine *path);

void print_path(MultiLine *path, double geotransform[6]);

char* determine_edge(ContourGridCell *cell, int contour_state, int prev_contour_state);

void determine_delta_xy(int *delta_xy, ContourGridCell *cell, int contour_state, int prev_contour_state);

int calculate_cell_contour_state(RasterCellCorners corners, double threshold);

bool is_empty_cell(ContourGridCell *cell);


#endif
