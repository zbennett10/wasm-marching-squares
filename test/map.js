
var map = L.map('map').setView([30, 0], 2);

L.tileLayer('http://{s}.tile.stamen.com/toner/{z}/{x}/{y}.png', {
    attribution: 'Map tiles by <a href="http://stamen.com">Stamen Design</a>, under <a href="http://creativecommons.org/licenses/by/3.0">CC BY 3.0</a>. Data  &copy; <a href="http://openstreetmap.org">OpenStreetMap</a>, under <a href="http://www.openstreetmap.org/copyright">ODbL</a>'
}).addTo(map);

//var url = "https://stamen.github.io/spatial-dataviz-for-data-scientists/data/world/ne_50m_populated_places_simple.geojson"
// var url = "ne_50m_populated_places_simple.geojson"A

// GREAT TEST CASE IF I CAN REPRODUCE THIS:
// http://bl.ocks.org/rveciana/420a33fd0963e2a6aad16da54725af36
// vardah.tiff

// Download geotiff using local ArrayBuffer
(async function() {
    // const response = await fetch('tiffs/sfctmp.tif');
    const response = await fetch('tiffs/vardah.tif');
    const arrayBuffer = await response.arrayBuffer();
    const tiff = await GeoTIFF.fromArrayBuffer(arrayBuffer);
    const image = await tiff.getImage();
    const rasters = await image.readRasters();

    /**
     * https://stackoverflow.com/questions/27166739/description-of-parameters-of-gdal-setgeotransform
     *
     * Geotransform parameters:
     * [ x_coord of upper left corner of upper left pixel, pixel_width, 0( because north up), y coordinate of upper left corner of upper left pixel, 0 (because north up), pixel_height ]
     * index 0 and 3 above combine to make the origin of the raster)which constitutes the top left corner of the top left pixel of the raster image if (0, 0)
     *
     * Using these parameters you can then do an affine transformation to transform raster pixel coordinates to a given spatial reference system.
     * 
     * 
     * Spatial Reference System
     *      |
     *      |
     *      |
     *      |
     * (0,0)|___________
     * 
     *  Raster
     *      ____________
     * (0,0)|
     *      |
     *      |
     *      |
     * 
     * Affine transformation transforms raster coordinates to spatial coordinates:
     * var applyGeoTransform = function(x, y, geoTransform){
        var xgeo = geoTransform[0] + x*geoTransform[1] + y*geoTransform[2];
        var ygeo = geoTransform[3] + x*geoTransform[4] + y*geoTransform[5];
        return [xgeo, ygeo];
        };
     */
    const rowRotation    = 0,
          columnRotation = 0,
          tiepoint       = image.getTiePoints()[0],
          pixelScale     = image.getFileDirectory().ModelPixelScale;

    var transform = [tiepoint.x, pixelScale[0], 0, tiepoint.y, 0, -1*pixelScale[1]];

    // USE FOR vardah.tif
    var tiffWidth = image.getWidth();
    var tiffHeight = image.getHeight();
    var pressData = new Array(tiffHeight);
    var tempData = new Array(tiffHeight);
    var uData = new Array(tiffHeight);
    var vData = new Array(tiffHeight);
    var spdData = new Array(tiffHeight);
    for (var j = 0; j<tiffHeight; j++){
        pressData[j] = new Array(tiffWidth);
        tempData[j] = new Array(tiffWidth);
        uData[j] = new Array(tiffWidth);
        vData[j] = new Array(tiffWidth);
        spdData[j] = new Array(tiffWidth);
        for (var i = 0; i<tiffWidth; i++){
            pressData[j][i] = rasters[0][i + j*tiffWidth];
            tempData[j][i] = rasters[1][i + j*tiffWidth];
            uData[j][i] = rasters[2][i + j*tiffWidth];
            vData[j][i] = rasters[3][i + j*tiffWidth];
            spdData[j][i] = 1.943844492 * Math.sqrt(uData[j][i]*uData[j][i] + vData[j][i]*vData[j][i]);

        }
    }

    // USE FOR sfctmp.tif
    // var tiffWidth = image.getWidth();
    // var tiffHeight = image.getHeight();
    // var pressData = new Array(tiffHeight);
    // for (var j = 0; j<tiffHeight; j++){
    //     pressData[j] = new Array(tiffWidth);
    //     for (var i = 0; i<tiffWidth; i++){
    //         pressData[j][i] = rasters[0][i + j*tiffWidth];
    //     }
    // }
    
    // var intervalsPress = [30, 40, 50, 60, 70, 80, 90, 100, 110, 120];
    var intervalsPress = [970, 972, 974, 976, 978, 980, 982, 984, 986, 988, 990, 992, 994, 996, 998,
        1000, 1002, 1004, 1006, 1008, 1010, 1012, 1014, 1016, 1018, 1020, 1022, 1024, 1026, 1028];
    const transformedRasterData = pressData;
    const intervals = intervalsPress;

    let rasterDataPointer, geoTransformPointer, intervalsPointer, pressIsolinesPointer;
    let isolinesLayer;

    console.log(transformedRasterData);

    document.querySelector('.mybutton')
            .addEventListener('click', () => {
                if (isolinesLayer) {
                    map.removeLayer(isolinesLayer)
                }

                try {
                    const raster_data_rows = transformedRasterData.length,
                        raster_data_cols = transformedRasterData[0].length;
                
                    const bytesPerElement = Module.HEAPF64.BYTES_PER_ELEMENT;
                    rasterDataPointer = Module._malloc(bytesPerElement * raster_data_rows * raster_data_cols);
                    for(let i = 0; i < raster_data_rows; i++) {
                        const row = transformedRasterData[i];
                        Module.HEAPF64.set(Float64Array.of(...row), (rasterDataPointer / bytesPerElement) + (i*raster_data_cols));
                    }

                    geoTransformPointer = Module._malloc(bytesPerElement * 6);
                    Module.HEAPF64.set(transform, (geoTransformPointer / bytesPerElement));

                    intervalsPointer = Module._malloc(bytesPerElement * intervals.length);
                    Module.HEAPF64.set(intervals, (intervalsPointer / bytesPerElement));

                    const pressIsolinesPointer = Module.ccall(
                        'generate_isolines_geojson_',	// name of C function
                        'number',	// Get the raw pointer - make sure to free this!
                        ['number', 'number', 'number', 'number', 'number', 'number'],	// argument types
                        [raster_data_rows, raster_data_cols, rasterDataPointer, geoTransformPointer, intervals.length, intervalsPointer ]	// arguments
                    );

                    const pressIsolines = Module.UTF8ToString(pressIsolinesPointer); // Convert the pointer to the string we need
                    
                    isolinesLayer = L.geoJson(JSON.parse(pressIsolines), {
                        style: {
                      "color": "red",
                      "weight": 2,
                      "opacity": 0.65
                    }

                 });
                 isolinesLayer.addTo(map);

                } catch (error) {
                    console.log('ccall error generating isolines: ', error);
                } finally {
                    [rasterDataPointer, geoTransformPointer, intervalsPointer, pressIsolinesPointer].forEach(pointer => {
                        if (pointer) Module._free(pointer);
                    })
                }
        });

})();

