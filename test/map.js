import marchingSquaresWasmModule from './wasm-marching-squares.js';

var map = L.map('map').setView([30, 0], 2);

L.tileLayer('http://{s}.tile.stamen.com/toner/{z}/{x}/{y}.png', {
    attribution: 'Map tiles by <a href="http://stamen.com">Stamen Design</a>, under <a href="http://creativecommons.org/licenses/by/3.0">CC BY 3.0</a>. Data  &copy; <a href="http://openstreetmap.org">OpenStreetMap</a>, under <a href="http://www.openstreetmap.org/copyright">ODbL</a>'
}).addTo(map);

(async function() {
    const WasmMarchingSquares = await marchingSquaresWasmModule();

    const response = await fetch('tiffs/vardah.tif');
    const arrayBuffer = await response.arrayBuffer();
    const tiff = await window.GeoTIFF.fromArrayBuffer(arrayBuffer);
    const image = await tiff.getImage();
    const rasters = await image.readRasters();

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

    document.querySelector('.mybutton')
            .addEventListener('click', () => {
                if (isolinesLayer) {
                    map.removeLayer(isolinesLayer)
                }

                try {
                    const raster_data_rows = transformedRasterData.length,
                        raster_data_cols = transformedRasterData[0].length;
                    
                    const bytesPerElement = WasmMarchingSquares.HEAPF64.BYTES_PER_ELEMENT;
                    rasterDataPointer = WasmMarchingSquares._malloc(bytesPerElement * raster_data_rows * raster_data_cols);
                    for(let i = 0; i < raster_data_rows; i++) {
                        const row = transformedRasterData[i];
                        WasmMarchingSquares.HEAPF64.set(Float64Array.of(...row), (rasterDataPointer / bytesPerElement) + (i*raster_data_cols));
                    }

                    geoTransformPointer = WasmMarchingSquares._malloc(bytesPerElement * 6);
                    WasmMarchingSquares.HEAPF64.set(transform, (geoTransformPointer / bytesPerElement));

                    intervalsPointer = WasmMarchingSquares._malloc(bytesPerElement * intervals.length);
                    WasmMarchingSquares.HEAPF64.set(intervals, (intervalsPointer / bytesPerElement));

                    const pressIsolinesPointer = WasmMarchingSquares.ccall(
                        'generate_isolines_geojson_',	// name of C function
                        'number',	// Get the raw pointer - make sure to free this!
                        ['number', 'number', 'number', 'number', 'number', 'number'],	// argument types
                        [raster_data_rows, raster_data_cols, rasterDataPointer, geoTransformPointer, intervals.length, intervalsPointer ]	// arguments
                    );

                    const pressIsolines = WasmMarchingSquares.UTF8ToString(pressIsolinesPointer); // Convert the pointer to the string we need
                    
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
                        if (pointer) WasmMarchingSquares._free(pointer);
                    })
                }
        });

})();
