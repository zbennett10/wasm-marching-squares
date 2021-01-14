#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "jansson.h"
#include "math.h"
#include "marching-squares.h"

static short delta_x_contour[16]  = { 0, 0, 1, 1, 0, 0, 0, 0, -1, 0, 1, 1, -1, 0, -1, 0 };
static short delta_y_contour[16]  = { 0, -1, 0, 0, 1, 1, 1, 1, 0, -1, 0, 0, 0, -1, 0, 0 };
static char* start_edges[16] = { "none", "left", "bottom", "left", "right", "none", "bottom", "left", "top", "top", "none", "top", "right", "right", "bottom", "none" };
static char* next_edges[16]  = { "none", "bottom", "right", "right", "top", "top", "top", "top", "left", "bottom", "right", "right", "left", "bottom", "left", "none" };

char* generate_isolines_geojson(
    int raster_data_rows,
    int raster_data_cols,
    double raster_data[raster_data_rows][raster_data_cols],
    double geotransform[6],
    int interval_count,
    double intervals[interval_count]
) {
    json_t *isolines      = json_object(),
            *features      = json_array();

    json_object_set_new(isolines, "type", json_string("FeatureCollection"));
    json_object_set_new(isolines, "features", features);

    // Implement marching squares for each threshold boundary
    for (int threshold_idx = 0; threshold_idx < interval_count; threshold_idx++) {
      double threshold = intervals[threshold_idx];
      int grid_rows = raster_data_rows - 1;
      int grid_cols = raster_data_cols - 1;
      ContourGridCell (*cells)[grid_cols] = malloc( sizeof(ContourGridCell[grid_rows][grid_cols]) );
      json_t *feature       = json_object(),
             *geometry      = json_object(),
             *properties    = json_object(),
             *coordinates   = json_array(); 

      json_array_append_new(features, feature);
      json_object_set_new(feature, "type", json_string("Feature"));
      json_object_set_new(feature, "geometry", geometry);
      json_object_set_new(feature, "properties", properties);
      json_object_set_new(geometry, "type", json_string("MultiLineString"));
      json_object_set_new(geometry, "coordinates", coordinates);
      json_object_set_new(properties, "threshold", json_real(threshold));

        // Generate a grid of cells each containing a binary digit dependent upon the given threshold
        for (int i = 0; i < grid_rows; i++) {
            for (int j = 0; j < grid_cols; j++) {
                RasterCellCorners corners = {
                  .top_left     = raster_data[i+1][j], 
                  .top_right    = raster_data[i+1][j+1],
                  .bottom_right = raster_data[i][j+1],
                  .bottom_left  = raster_data[i][j]
                };

                if(INVALID_RASTER_DATA(corners))
                  continue;

                int contour_state = calculate_cell_contour_state(corners, threshold);
                bool is_saddle = false;

                if(contour_state == 5 || contour_state == 10){
                    int average = (corners.top_left + corners.top_right + corners.bottom_right + corners.bottom_left) / 4;
                    if(contour_state == 5 && (average < threshold)){
                        contour_state = 10;
                        is_saddle = true;
                    } else if(contour_state == 10 && (average < threshold)){
                        contour_state = 5;
                        is_saddle = true;
                    }
                }

                // Cases 0 and 15 do not include any edges 
                if (contour_state != 0 && contour_state != 15) {
                    double top, bottom, left, right;
                    top = bottom = left = right = 0.5;

                    if(contour_state == 1){
                        left    = 1 - interpolate(threshold, corners.top_left, corners.bottom_left);
                        bottom  = 1 - interpolate(threshold, corners.bottom_right, corners.bottom_left);
                    } else if(contour_state == 2){
                        bottom  = interpolate(threshold, corners.bottom_left, corners.bottom_right);
                        right   = 1 - interpolate(threshold, corners.top_right, corners.bottom_right);
                    } else if(contour_state == 3){
                        left    = 1 - interpolate(threshold, corners.top_left, corners.bottom_left);
                        right   = 1 - interpolate(threshold, corners.top_right, corners.bottom_right);
                    } else if(contour_state == 4){
                        top     = interpolate(threshold, corners.top_left, corners.top_right);
                        right   = interpolate(threshold, corners.bottom_right, corners.top_right);
                    } else if(contour_state == 5){
                        top     = interpolate(threshold, corners.top_left, corners.top_right);
                        right   = interpolate(threshold, corners.bottom_right, corners.top_right);
                        bottom  = 1 - interpolate(threshold, corners.bottom_right, corners.bottom_left);
                        left    = 1 - interpolate(threshold, corners.top_left, corners.bottom_left);
                    } else if(contour_state == 6){
                        bottom  = interpolate(threshold, corners.bottom_left, corners.bottom_right);
                        top     = interpolate(threshold, corners.top_left, corners.top_right);
                    } else if(contour_state == 7){
                        left    = 1 - interpolate(threshold, corners.top_left, corners.bottom_left);
                        top     = interpolate(threshold, corners.top_left, corners.top_right);
                    } else if(contour_state == 8){
                        left    = interpolate(threshold, corners.bottom_left, corners.top_left);
                        top     = 1 - interpolate(threshold, corners.top_right, corners.top_left);
                    } else if(contour_state == 9){
                        bottom  = 1 - interpolate(threshold, corners.bottom_right, corners.bottom_left);
                        top     = 1 - interpolate(threshold, corners.top_right, corners.top_left);
                    } else if(contour_state == 10){
                        top     = 1 - interpolate(threshold, corners.top_right, corners.top_left);
                        right   = 1 - interpolate(threshold, corners.top_right, corners.bottom_right);
                        bottom  = interpolate(threshold, corners.bottom_left, corners.bottom_right);
                        left    = interpolate(threshold, corners.bottom_left, corners.top_left);
                    } else if(contour_state == 11){
                        top     = 1 - interpolate(threshold, corners.top_right, corners.top_left);
                        right   = 1 - interpolate(threshold, corners.top_right, corners.bottom_right);
                    } else if(contour_state == 12){
                        left    = interpolate(threshold, corners.bottom_left, corners.top_left);
                        right   = interpolate(threshold, corners.bottom_right, corners.top_right);
                    } else if(contour_state == 13){
                        bottom  = 1 - interpolate(threshold, corners.bottom_right, corners.bottom_left);
                        right   = interpolate(threshold, corners.bottom_right, corners.top_right);
                    } else if(contour_state == 14){
                        left    = interpolate(threshold, corners.bottom_left, corners.top_left);
                        bottom  = interpolate(threshold, corners.bottom_left, corners.bottom_right);
                    } else {
                        printf("Illegal contour_state detected: %d\n",contour_state);
                    }

                    ContourGridCell cell = {
                        .threshold = threshold,
                        .contour_state = contour_state,
                        .is_saddle = is_saddle,
                        .top = top,
                        .bottom = bottom,
                        .left = left,
                        .right = right
                    }; 

                    cells[i][j] = cell;
                } else {
                    cells[i][j] = EMPTY_CELL;
                }

            }
        }

        // Take our grid of cells containing interpolated contour positions and trace coordinate paths - from which, we will create GeoJSON
        MultiLine **paths = malloc(sizeof(MultiLine *) * grid_rows * grid_cols);
        int path_count = 0;
        int path_idx = 0;
        double epsilon = 1e-7;

        for (int i = 0; i < grid_rows; i++) {
            for (int j = 0; j < grid_cols; j++) {
                ContourGridCell cell = cells[i][j];
                if (is_empty_cell(&cell)) {
                  continue;
                }

                if(!cell.is_saddle && !CELL_NO_EDGES(cell.contour_state)) {
                    MultiLine *path = gen_path(grid_cols, grid_rows, cells, i, j);
                    bool merged = false;
                    /* we may try to merge paths at this point */
                    if(path->mergable) {
                      /*
                        search backwards through the path array to find an entry
                        that starts with where the current path ends...
                      */
                      MultiLine *tail = path;
                      while(tail->next != NULL) {
                        tail = tail->next;
                      }
                      Point new_path_endpoint = tail->point;

                      // Why path_idx need to be - 3 here..? Original code has it at - 1 but missed lots of merged paths without it.
                      for(int k = path_idx - 1; k >= 0; k--){
                        if((fabs(paths[k]->point.x - new_path_endpoint.x) <= epsilon) && (fabs(paths[k]->point.y - new_path_endpoint.y) <= epsilon)){
                          for(int l = get_line_count(path) - 2; l >= 0; --l){
                              MultiLine *tmp = paths[k];
                              MultiLine *matching_segment = get_segment_at_index(path, l);
                              matching_segment->next = tmp;
                              paths[k] = matching_segment;
                          }
                          merged = true;
                          break;
                        }
                      }
                    }

                    if(!merged) {
                      paths[path_idx++] = path;
                      path_count++;
                    }
                }

            }
        }

        add_multilines_to_geojson(path_count, paths, coordinates, geotransform);

        free(paths);
        free(cells);

    }

    char* geojson = json_dumps(isolines, JSON_COMPACT);

    json_decref(isolines);

    return geojson;
}

MultiLine* gen_path(int grid_cols, int row_count, ContourGridCell (*cells)[grid_cols], int row, int col) {
    MultiLine *path = (MultiLine*)malloc(sizeof(MultiLine));

    ContourGridCell *current_cell = &cells[row][col];
    int *delta_xy = (int*)malloc(sizeof(int) * 2);
    int contour_state = current_cell->contour_state;
    char* edge = start_edges[contour_state];

    Point pt = gen_point_from_edge(current_cell, edge);

    /* push initial segment */
    path->point = (Point) { .x = col + pt.x, .y = row + pt.y };
    edge = next_edges[contour_state];
    pt = gen_point_from_edge(current_cell, edge);
    Point initial_end = (Point){.x = col + pt.x, .y = row + pt.y };
    MultiLine *next_segment = (MultiLine*)malloc(sizeof(MultiLine));
    next_segment->point = initial_end;
    next_segment->next = NULL;
    path->next = next_segment;

    if((!CELL_NO_EDGES(contour_state)) && (contour_state != 5) && (contour_state != 10)){
      current_cell->contour_state = 15;
    }

    // /* now walk arround the enclosed area in clockwise-direction */
    int k = col + delta_x_contour[contour_state];
    int l = row + delta_y_contour[contour_state];
    int prev_contour_state = contour_state;

    MultiLine *last_segment = path->next;

    while((k >= 0) && (l >= 0) && (l < row_count) && (k != col || l != row)) {
      current_cell = &cells[l][k];
      if (is_empty_cell(current_cell)) {
        break;
      }

      contour_state = current_cell->contour_state;

      if(contour_state == 0 || contour_state == 15) {
        path->mergable = true;
        return path;
      }

      edge  = determine_edge(current_cell, contour_state, prev_contour_state);
      determine_delta_xy(delta_xy, current_cell, contour_state, prev_contour_state);

      pt = gen_point_from_edge(current_cell, edge);

      MultiLine *new_last_segment = (MultiLine*)malloc(sizeof(MultiLine));
      new_last_segment->point = (Point){.x = k + pt.x, .y = l + pt.y };
      new_last_segment->next = NULL;
      last_segment->next = new_last_segment;
      last_segment = new_last_segment;

      if((!CELL_NO_EDGES(contour_state)) && (contour_state != 5) && (contour_state != 10)){
        current_cell->contour_state = 15;
      }

      k += delta_xy[0];
      l += delta_xy[1];
      prev_contour_state = contour_state;

    }

    free(delta_xy);
    return path;
  }

Point gen_point_from_edge(ContourGridCell *cell, char* edge) {
    Point pt = { .x = 0.0, .y = 0.0 };

    if(!strcmp(edge, "top")) {
        pt.x = cell->top;
        pt.y = 1.0;
    } else if(!strcmp(edge, "bottom")) {
        pt.x = cell->bottom;
        pt.y = 0.0;
    } else if(!strcmp(edge, "right")) {
        pt.x = 1.0;
        pt.y = cell->right;
    } else if(!strcmp(edge, "left")) {
        pt.x = 0.0;
        pt.y = cell->left;
    }

    return pt;
}

Point apply_geo_transformation_to_pixel_coordinates(double x, double y, double geotransform[6]) {
    double georeferenced_x = geotransform[0] + x*geotransform[1] + y*geotransform[2];
    double georeferenced_y = geotransform[3] + x*geotransform[4] + y*geotransform[5];
    return (Point){.x = georeferenced_x, .y = georeferenced_y };
}

double interpolate(double threshold, double y0, double y1) {
    return (threshold - y0) / (y1 - y0);
}

void print_path(MultiLine *path, double geotransform[6]) {
  while(path) {
    Point geopoint = apply_geo_transformation_to_pixel_coordinates(path->point.x, path->point.y, geotransform);
    printf("(%f, %f) =>", geopoint.x, geopoint.y);
    path = path->next;
  }

  printf("\n\n\n");
}

int get_line_count(MultiLine *path) {
  int count = 0;
  MultiLine *current = path;
  while(current) {
    count++;
    current = current->next;
  }

  return count;
}

MultiLine* get_segment_at_index(MultiLine *path, int i) {
  if (i == 0)
    return path;

  MultiLine *current = path;
  while(current && i > 0) {
    current = current->next;
    i--;
  }

  return current;
}

void add_multilines_to_geojson(int path_count, MultiLine **paths, json_t *coordinates, double geotransform[6]) {
  for (int i = 0; i < path_count; i++) {
    MultiLine *path = paths[i];
    json_t *multiline = json_array();
    json_array_append_new(coordinates, multiline);

    MultiLine *current = path;
    while(current != NULL) {
        Point geo_start_point = apply_geo_transformation_to_pixel_coordinates(current->point.x, current->point.y, geotransform);

        json_t *point = json_array();

        json_array_append_new(multiline, point);
        json_array_append_new(point, json_real(geo_start_point.x));
        json_array_append_new(point, json_real(geo_start_point.y));
        
        MultiLine *next = current->next;
        free(current);
        current = next;
    }
  }

}

bool is_empty_cell(ContourGridCell *cell) {
  return cell->contour_state == -1 && cell->top == 0 && cell->bottom == 0 && cell->left == 0 && cell->right == 0;
}

char* determine_edge(ContourGridCell *cell, int contour_state, int prev_contour_state) {
  char* edge  = next_edges[contour_state];

  /* select upper or lower band, depending on previous cells cval */
  if (contour_state == 5 && cell->is_saddle) {
    return delta_y_contour[prev_contour_state] == -1 ? "left" : "right";
  }
  if (contour_state == 5 && !(cell->is_saddle) && delta_x_contour[prev_contour_state] == -1) {
    return "bottom";
  }

  if (contour_state == 10 && cell->is_saddle) {
      return delta_x_contour[prev_contour_state] == -1 ? "top" : "bottom";
  }

  if (contour_state == 10 && !(cell->is_saddle) && delta_y_contour[prev_contour_state] == 1) {
    return "left";
  }

  return edge;
}

void determine_delta_xy(int *delta_xy, ContourGridCell *cell, int contour_state, int prev_contour_state) {
  delta_xy[0]    = delta_x_contour[contour_state];
  delta_xy[1]    = delta_y_contour[contour_state];

  if (contour_state == 5 && cell->is_saddle) {
      if(delta_y_contour[prev_contour_state] == -1) {
        delta_xy[0] = -1;
        delta_xy[1] = 0;
      } else {
        delta_xy[0] = 1;
        delta_xy[1] = 0;
      }
  }
  
  if (contour_state == 5 && !(cell->is_saddle) && delta_x_contour[prev_contour_state] == -1) {
    delta_xy[0] = 0;
    delta_xy[1] = -1;
  }

  if (contour_state == 10 && cell->is_saddle) {
    if(delta_x_contour[prev_contour_state] == -1){
      delta_xy[0] = 0;
      delta_xy[1] = 1;
    } else {
      delta_xy[0] = 0;
      delta_xy[1] = -1;
    }
  }

  if (contour_state == 10 && !(cell->is_saddle) && delta_y_contour[prev_contour_state] == 1) {
        delta_xy[0] = -1;
        delta_xy[1] = 0;
  }
}

int calculate_cell_contour_state(RasterCellCorners corners, double threshold) {
  int contour_state = 0;

  contour_state |= ((corners.top_left     >= threshold) ? 8 : 0);
  contour_state |= ((corners.top_right    >= threshold) ? 4 : 0);
  contour_state |= ((corners.bottom_right >= threshold) ? 2 : 0);
  contour_state |= ((corners.bottom_left  >= threshold) ? 1 : 0);

  return contour_state;
}
