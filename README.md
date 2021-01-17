# wasm-marching-squares
This library is a direct port of rveciana's [raster-marching-squares](https://github.com/rveciana/raster-marching-squares) to WebAssembly by way of C.
The algorithm is implemented in C and currently generates GeoJSON isolines given raw data from a GeoTIFF file. Isobands will be supported in the future.

# Usage

*WIP*

For now, you can check out the test folder and see how the `map.js` file is using the WASM module. In the future, the external API will be much easier to use.

# Caveats
- Need to add a cleaner external API for ease of use.
- Isoband generation is not implemented *yet*

# Build
To properly build, you will need to download the Emscripten SDK.

1. Compile the Jansson C JSON library using `emcc`
    - Remove CMakeCache.txt from `include/jansson-2.13.1 if it exists
    - `cd include/jansson-2.13.1 && emcmake cmake -DCMAKE_INSTALL_PREFIX:PATH=./emcc-lib .`
    - `emmake make VERBOSE=1`
    - `emmake make check`
    - `emmake make install`

2. Generate the WASM code and JavaScript 'glue' code
```bash
./build
```

# Test

## C Library Unit Testing
1. Compile the Jansson C JSON library using cmake
    - Remove CMakeCache.txt from `include/jansson-2.13.1 if it exists
    - `cd include/jansson-2.13.1 && cmake .`
    - `emmake make VERBOSE=1`
    - `emmake make check`
    - `emmake make install`

2. Compile and run the unit tests
```bash
cd test && ./test
```

## Functional Testing

- Start a webserver and serve the `test` directory
- Navigate to the `index.html` and click `Test GeoJSON`
- You can test the `vardah.tif` file or the `sfctmp.tif` file by altering the `test/map.js` file.

## TODO
- Implement Isoband generation
- Speed up lookups using a Quadtree