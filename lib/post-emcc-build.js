function WasmMarchingSquares() {}

// Emscripten will resolve this promise when WASM is instantiated
WasmMarchingSquares.prototype.ready = new Promise(function(resolve, reject) {
  // Queue the promise to resolve within Emscripten's init loop
  addOnPreMain(function() {
    var internalApi = {
      malloc: cwrap('malloc', 'number', ['number']),
      free: cwrap('free', null, ['number']),
      HEAPU8: HEAPU8,
      HEAPF32: HEAPF32,
      HEAPF64: HEAPF64,
      UTF8ToString: UTF8ToString,
      generateIsolines: cwrap('generate_isolines_geojson_', 'number', ['number', 'number', 'number', 'number', 'number', 'number'])
    }

    Object.freeze(internalApi);
    Object.defineProperty(WasmMarchingSquares.prototype, 'internalApi', {value: internalApi});

    resolve();
  });
});

// TODO: Improve the interal API such that we guarantee the freeing up of as much memory as possible in terms of the pointers passed into the 'generate' functions
// TODO: Ensure that there is an external API that is returned when the wasm Module is instantiated

Module['WasmMarchingSquares'] = WasmMarchingSquares;

if (typeof global !== 'undefined' && exports) {
  module.exports.WasmMarchingSquares = WasmMarchingSquares;
}
