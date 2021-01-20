import Module from './wasm-marching-squares.js';

const loadWasmMarchingSquaresModule = function() {
    return (async function() {
                const wasmModule = await Module();

                return {
                    generateIsolines: function(rasterData, intervals, transform) {
                        let  rasterDataPointer, geoTransformPointer, intervalsPointer, isolinesPointer;

                        try {
                            const raster_data_rows = rasterData.length,
                                raster_data_cols = rasterData[0].length;
                            
                            const bytesPerElement = wasmModule.HEAPF64.BYTES_PER_ELEMENT;
                            rasterDataPointer = wasmModule._malloc(bytesPerElement * raster_data_rows * raster_data_cols);
                            for(let i = 0; i < raster_data_rows; i++) {
                                const row = rasterData[i];
                                wasmModule.HEAPF64.set(Float64Array.of(...row), (rasterDataPointer / bytesPerElement) + (i*raster_data_cols));
                            }

                            geoTransformPointer = wasmModule._malloc(bytesPerElement * 6);
                            wasmModule.HEAPF64.set(transform, (geoTransformPointer / bytesPerElement));

                            intervalsPointer = wasmModule._malloc(bytesPerElement * intervals.length);
                            wasmModule.HEAPF64.set(intervals, (intervalsPointer / bytesPerElement));

                            isolinesPointer = wasmModule.ccall(
                                'generate_isolines_geojson_',
                                'number',	// Return the raw pointer - make sure to free this!
                                ['number', 'number', 'number', 'number', 'number', 'number'],	// argument types
                                [raster_data_rows, raster_data_cols, rasterDataPointer, geoTransformPointer, intervals.length, intervalsPointer ]	// arguments
                            );

                            return wasmModule.UTF8ToString(isolinesPointer); // Convert the pointer to the string we need

                        } catch (error) {
                            console.log('[WasmMarchingSquares]: Error generating isolines: ', error);
                        } finally {
                            [rasterDataPointer, geoTransformPointer, intervalsPointer, isolinesPointer].forEach(pointer => {
                                if (pointer) wasmModule._free(pointer);
                            })
                        }

                    }
                }
            })();
}

export default loadWasmMarchingSquaresModule;
