#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "jansson.h"
#include "math.h"
#include "marching-squares.h"

static short dx_contour[16]  = { 0, 0, 1, 1, 0, 0, 0, 0, -1, 0, 1, 1, -1, 0, -1, 0 };
static short dy_contour[16]  = { 0, -1, 0, 0, 1, 1, 1, 1, 0, -1, 0, 0, 0, -1, 0, 0 };
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
      json_t *feature       = json_object(),
             *geometry      = json_object(),
             *properties    = json_object(),
             *coordinates   = json_array(); 

      json_object_set_new(feature, "type", json_string("Feature"));
      json_object_set_new(feature, "geometry", geometry);

      // TODO: can implement "properties" array - need to add raster date to properties
      json_object_set_new(feature, "properties", properties);

      json_array_append_new(features, feature);

      json_object_set_new(geometry, "type", json_string("MultiLineString"));
      json_object_set_new(geometry, "coordinates", coordinates);


        double threshold = intervals[threshold_idx];
        int grid_rows = raster_data_rows - 1;
        int grid_cols = raster_data_cols - 1;
        ContourGridCell (*cells)[grid_cols] = malloc( sizeof(ContourGridCell[grid_rows][grid_cols]) );

        // Generate a grid of cells each containing a binary digit dependent upon the given threshold
        for (int i = 0; i < grid_rows; i++) {
            for (int j = 0; j < grid_cols; j++) {
                double top_left     = raster_data[i+1][j], 
                       top_right    = raster_data[i+1][j+1],
                       bottom_right = raster_data[i][j+1],
                       bottom_left  = raster_data[i][j];

                if(isnan(top_left) || isnan(top_right) || isnan(bottom_right) || isnan(bottom_left)){
                  continue;
                }

                int contour_state = 0;

                contour_state |= ((top_left >= threshold)     ? 8 : 0);
                contour_state |= ((top_right >= threshold)    ? 4 : 0);
                contour_state |= ((bottom_right >= threshold) ? 2 : 0);
                contour_state |= ((bottom_left >= threshold)  ? 1 : 0);
                bool is_saddle = false;

                if(contour_state == 5 || contour_state == 10){
                    int average = (top_left + top_right + bottom_right + bottom_left) / 4;
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
                        left    = 1 - interpolate(threshold, top_left, bottom_left);
                        bottom  = 1 - interpolate(threshold, bottom_right, bottom_left);
                    } else if(contour_state == 2){
                        bottom  = interpolate(threshold, bottom_left, bottom_right);
                        right   = 1 - interpolate(threshold, top_right, bottom_right);
                    } else if(contour_state == 3){
                        left    = 1 - interpolate(threshold, top_left, bottom_left);
                        right   = 1 - interpolate(threshold, top_right, bottom_right);
                    } else if(contour_state == 4){
                        top     = interpolate(threshold, top_left, top_right);
                        right   = interpolate(threshold, bottom_right, top_right);
                    } else if(contour_state == 5){
                        top     = interpolate(threshold, top_left, top_right);
                        right   = interpolate(threshold, bottom_right, top_right);
                        bottom  = 1 - interpolate(threshold, bottom_right, bottom_left);
                        left    = 1 - interpolate(threshold, top_left, bottom_left);
                    } else if(contour_state == 6){
                        bottom  = interpolate(threshold, bottom_left, bottom_right);
                        top     = interpolate(threshold, top_left, top_right);
                    } else if(contour_state == 7){
                        left    = 1 - interpolate(threshold, top_left, bottom_left);
                        top     = interpolate(threshold, top_left, top_right);
                    } else if(contour_state == 8){
                        left    = interpolate(threshold, bottom_left, top_left);
                        top     = 1 - interpolate(threshold, top_right, top_left);
                    } else if(contour_state == 9){
                        bottom  = 1 - interpolate(threshold, bottom_right, bottom_left);
                        top     = 1 - interpolate(threshold, top_right, top_left);
                    } else if(contour_state == 10){
                        top     = 1 - interpolate(threshold, top_right, top_left);
                        right   = 1 - interpolate(threshold, top_right, bottom_right);
                        bottom  = interpolate(threshold, bottom_left, bottom_right);
                        left    = interpolate(threshold, bottom_left, top_left);
                    } else if(contour_state == 11){
                        top     = 1 - interpolate(threshold, top_right, top_left);
                        right   = 1 - interpolate(threshold, top_right, bottom_right);
                    } else if(contour_state == 12){
                        left    = interpolate(threshold, bottom_left, top_left);
                        right   = interpolate(threshold, bottom_right, top_right);
                    } else if(contour_state == 13){
                        bottom  = 1 - interpolate(threshold, bottom_right, bottom_left);
                        right   = interpolate(threshold, bottom_right, top_right);
                    } else if(contour_state == 14){
                        left    = interpolate(threshold, bottom_left, top_left);
                        bottom  = interpolate(threshold, bottom_left, bottom_right);
                    } else {
                        printf("Illegal contour_state detected: %d\n",contour_state);
                    }

                    // TODO: Let's malloc this...
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
                }

            }
        }

        // Take our grid of cells containing interpolated contour positions and trace coordinate paths - from which, we will create GeoJSON
        MultiLine **paths = NULL;
        int path_count = 0;
        int path_idx = 0;
        double epsilon = 1e-7;

        for (int i = 0; i < grid_rows; i++) {
            for (int j = 0; j < grid_cols; j++) {
                ContourGridCell cell = cells[i][j];

                if(!cell.is_saddle && !CELL_NO_EDGES(cell.contour_state)) {
                    MultiLine *path = gen_path(grid_cols, cells, i, j, grid_rows);
                    if (paths == NULL) {
                      paths = malloc(sizeof(MultiLine));
                    }

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
                      if (path_count > 0) {
                        paths = realloc(paths, (path_count + 1) * sizeof(MultiLine));
                      }
                      paths[path_idx++] = path;
                      path_count++;
                    }
                }

            }
        }

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

        free(paths);
        free(cells);

    }

    return json_dumps(isolines, JSON_COMPACT);
}

MultiLine* gen_path(int grid_cols, ContourGridCell (*cells)[grid_cols], int row, int col, int row_count) {
    MultiLine *path = (MultiLine*)malloc(sizeof(MultiLine));

    ContourGridCell *current_cell = &cells[row][col];
    int dx, dy;
    int contour_state = current_cell->contour_state;
    char* edge = start_edges[contour_state];

    Point pt = gen_point_from_edge(*current_cell, edge);

    /* push initial segment */
    path->point = (Point) { .x = col + pt.x, .y = row + pt.y };
    edge = next_edges[contour_state];
    pt = gen_point_from_edge(*current_cell, edge);
    Point initial_end = (Point){.x = col + pt.x, .y = row + pt.y };
    MultiLine *next_segment = (MultiLine*)malloc(sizeof(MultiLine));
    next_segment->point = initial_end;
    next_segment->next = NULL;
    path->next = next_segment;

    if((!CELL_NO_EDGES(contour_state)) && (contour_state != 5) && (contour_state != 10)){
      current_cell->contour_state = 15;
    }

    // /* now walk arround the enclosed area in clockwise-direction */
    int k = col + dx_contour[contour_state];
    int l = row + dy_contour[contour_state];
    int prev_contour_state = contour_state;

    MultiLine *last_segment = path->next;

    while((k >= 0) && (l >= 0) && (l < row_count) && (k != col || l != row)) {
      current_cell = &cells[l][k];

      contour_state = current_cell->contour_state;

      if(contour_state == 0 || contour_state == 15) {
        path->mergable = true;
        return path;
      }

      edge  = next_edges[contour_state];
      dx    = dx_contour[contour_state];
      dy    = dy_contour[contour_state];
      // Handle saddles
      if(contour_state == 5 || contour_state == 10){
         /* select upper or lower band, depending on previous cells cval */
        if(contour_state == 5){
          if(current_cell->is_saddle){ /* this is actually a flipped case 10 */
            if(dy_contour[prev_contour_state] == -1){
              edge  = "left";
              dx    = -1;
              dy    = 0;
            } else {
              edge  = "right";
              dx    = 1;
              dy    = 0;
            }
          } else { /* real case 5 */
            if(dx_contour[prev_contour_state] == -1){
              edge  = "bottom";
              dx    = 0;
              dy    = -1;
            }
          }
        } else if(contour_state == 10){
          if(current_cell->is_saddle){ /* this is actually a flipped case 5 */
            if(dx_contour[prev_contour_state] == -1){
              edge  = "top";
              dx    = 0;
              dy    = 1;
            } else {
              edge  = "bottom";
              dx    = 0;
              dy    = -1;
            }
          } else {  /* real case 10 */
            if(dy_contour[prev_contour_state] == 1){
              edge  = "left";
              dx    = -1;
              dy    = 0;
            }
          }
        }
      }

      pt = gen_point_from_edge(*current_cell, edge);

      MultiLine *new_last_segment = (MultiLine*)malloc(sizeof(MultiLine));
      new_last_segment->point = (Point){.x = k + pt.x, .y = l + pt.y };
      new_last_segment->next = NULL;
      last_segment->next = new_last_segment;
      last_segment = new_last_segment;

      if((!CELL_NO_EDGES(contour_state)) && (contour_state != 5) && (contour_state != 10)){
        current_cell->contour_state = 15;
      }

      k += dx;
      l += dy;
      prev_contour_state = contour_state;

    }

    return path;
  }

Point gen_point_from_edge(ContourGridCell cell, char* edge) {
    Point pt = { .x = 0.0, .y = 0.0 };

    if(!strcmp(edge, "top")) {
        pt.x = cell.top;
        pt.y = 1.0;
    } else if(!strcmp(edge, "bottom")) {
        pt.x = cell.bottom;
        pt.y = 0.0;
    } else if(!strcmp(edge, "right")) {
        pt.x = 1.0;
        pt.y = cell.right;
    } else if(!strcmp(edge, "left")) {
        pt.x = 0.0;
        pt.y = cell.left;
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
