#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <string.h>
#include <stdbool.h>
#include "../include/greatest.h"
#include "../lib/marching-squares.h"

TEST marching_squares_lib_should_get_path_line_count(void) {
    Point start = { 10, 12 }, end = {11, 13};
    MultiLine path = { .mergable = false, .point = start, .next = NULL };
    int first_path_count = get_line_count(&path);

    ASSERT_EQ(1, first_path_count);

    MultiLine segment_two = { .mergable = false, .point = end, .next = NULL};
    path.next = &segment_two;

    int second_path_count = get_line_count(&path);

    ASSERT_EQ(2, second_path_count);

    PASS();
}

TEST marching_squares_lib_should_get_segment_at_index(void) {
    Point start = { 10, 12 }, middle = {50, 40}, end = {11, 13};
    MultiLine segment_end = { .mergable = false, .point = end, .next = NULL};
    MultiLine segment_middle = { .mergable = false, .point = middle, .next = &segment_end};
    MultiLine path = { .mergable = false, .point = start, .next = &segment_middle };

    ASSERT_EQ(50, get_segment_at_index(&path, 1)->point.x);
    ASSERT_EQ(12, get_segment_at_index(&path, 0)->point.y);
    ASSERT_EQ(13, get_segment_at_index(&path, 2)->point.y);

    PASS();
}

TEST marching_squares_lib_should_generate_geojson_isolines_for_simple_data(void) {
    double raster_data[3][3] = { {0,0,0}, {0,1,0}, {0,0,0} };
    double geotransform[6] = { 10, 1, 0, 10, 0, -1 };
    double geotransform_2[6] = { 20, 1, 0, 10, 0, -1 };
    double intervals[1] = { 0.5 };

    char *geojson = generate_isolines_geojson(
        3,
        3,
        raster_data,
        geotransform,
        1,
        intervals
    );

    char* expected = "{\"type\":\"FeatureCollection\",\"features\":[{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiLineString\",\"coordinates\":[[[11.0,9.5],[10.5,9.0],[11.0,8.5],[11.5,9.0],[11.0,9.5]]]},\"properties\":{}}]}"; 
    printf("Expected: %s\n\n", geojson);
    printf("Got: %s\n", expected);
    ASSERT(!strcmp(geojson, expected));

    char *geojson_2 = generate_isolines_geojson(
        3,
        3,
        raster_data,
        geotransform_2,
        1,
        intervals
    );

    char* expected_2 = "{\"type\":\"FeatureCollection\",\"features\":[{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiLineString\",\"coordinates\":[[[21.0,9.5],[20.5,9.0],[21.0,8.5],[21.5,9.0],[21.0,9.5]]]},\"properties\":{}}]}"; 
    printf("Expected: %s\n\n", expected_2);
    printf("Got: %s\n", geojson_2);
    ASSERT(!strcmp(geojson_2, expected_2));

    free(geojson);
    free(geojson_2);

    PASS();
}


TEST marching_squares_lib_should_generate_geojson_isolines_for_more_complex_data(void) {
    double raster_data[7][7] = {
        {5,5,5,5,5,5,5},
        {5,12,12,12,12,12,5},
        {5,12,5,5,5,12,5},
        {5,12,5,18,5,12,5},
        {5,12,5,5,5,12,5},
        {5,12,12,12,12,12,5},
        {5,5,5,5,5,5,5},
    };
    double geotransform[6] = { 10, 1, 0, 10, 0, -1 };
    double intervals[1] = { 9 };

    char *geojson = generate_isolines_geojson(
        7,
        7,
        raster_data,
        geotransform,
        1,
        intervals
    );

    char* expected = "{\"type\":\"FeatureCollection\",\"features\":[{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiLineString\",\"coordinates\":[[[11.0,9.4285714285714288],[10.571428571428571,9.0],[10.571428571428571,8.0],[10.571428571428571,7.0],[10.571428571428571,6.0],[10.571428571428571,5.0],[11.0,4.5714285714285712],[12.0,4.5714285714285712],[13.0,4.5714285714285712],[14.0,4.5714285714285712],[15.0,4.5714285714285712],[15.428571428571429,5.0],[15.428571428571429,6.0],[15.428571428571429,7.0],[15.428571428571429,8.0],[15.428571428571429,9.0],[15.0,9.4285714285714288],[14.0,9.4285714285714288],[13.0,9.4285714285714288],[12.0,9.4285714285714288],[11.0,9.4285714285714288]],[[11.428571428571429,8.0],[12.0,8.5714285714285712],[13.0,8.5714285714285712],[14.0,8.5714285714285712],[14.571428571428571,8.0],[14.571428571428571,7.0],[14.571428571428571,6.0],[14.0,5.4285714285714288],[13.0,5.4285714285714288],[12.0,5.4285714285714288],[11.428571428571429,6.0],[11.428571428571429,7.0],[11.428571428571429,8.0]],[[13.0,7.6923076923076925],[12.307692307692307,7.0],[13.0,6.3076923076923075],[13.692307692307693,7.0],[13.0,7.6923076923076925]]]},\"properties\":{}}]}"; 
    printf("Expected: %s\n\n", expected);
    printf("Got: %s\n", geojson);
    ASSERT(!strcmp(geojson, expected));

    free(geojson);

    PASS();
}

TEST marching_squares_lib_should_generate_geojson_isolines_for_unclosed_paths(void) {
    double raster_data[3][3] = { {0,0,1}, {0,0,0}, {0,0,0} };
    double geotransform[6] = { 10, 1, 0, 10, 0, -1 };
    double intervals[1] = { 0.5 };

    char *geojson = generate_isolines_geojson(
        3,
        3,
        raster_data,
        geotransform,
        1,
        intervals
    );

    char* expected = "{\"type\":\"FeatureCollection\",\"features\":[{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiLineString\",\"coordinates\":[[[11.5,10.0],[12.0,9.5]]]},\"properties\":{}}]}"; 
    printf("Expected: %s\n\n", expected);
    printf("Got: %s\n", geojson);
    ASSERT(!strcmp(geojson, expected));

    free(geojson);

    PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(MARCHING_SQUARES_ISOLINE) {
    RUN_TEST(marching_squares_lib_should_get_path_line_count);
    RUN_TEST(marching_squares_lib_should_get_segment_at_index);
    RUN_TEST(marching_squares_lib_should_generate_geojson_isolines_for_simple_data);
    RUN_TEST(marching_squares_lib_should_generate_geojson_isolines_for_more_complex_data);
    RUN_TEST(marching_squares_lib_should_generate_geojson_isolines_for_unclosed_paths);
}

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char* argv[]) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(MARCHING_SQUARES_ISOLINE);
    GREATEST_MAIN_END();
}
