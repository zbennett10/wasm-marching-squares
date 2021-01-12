# wasm-marching-squares
This library is a direct port of rveciana's [raster-marching-squares](https://github.com/rveciana/raster-marching-squares) to Web Assembly.
The algorithm is implemented in C and currently generates GeoJSON isolines given raw data from a GeoTIFF file. Isobands will be supported in the future.

# Usage


# Build
```bash
./build
```

# Test

## C Library Unit Testing

```bash
cd test && ./test
```

## Functional Testing

- Start a webserver and serve the `test` directory
- Navigate to the `index.html` and click `Test GeoJSON`
- You can test the `vardah.tif` file or the `sfctmp.tif` file by altering the `test/map.js` file.