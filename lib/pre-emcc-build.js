Module['locateFile'] = function(filename) {
    return ENVIRONMENT_IS_NODE ? __dirname+'/'+filename : filename;
  }
  