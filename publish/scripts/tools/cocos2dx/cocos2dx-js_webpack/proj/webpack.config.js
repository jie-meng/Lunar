const _ = require('lodash');
const webpack = require('webpack');
const path = require('path');

const SRC_DIR = path.resolve(__dirname, 'src');
const NODE_MODULES_DIR = path.resolve(__dirname, 'node_modules');

var npmModules = require('./package.json').dependencies;
var vendorLibs = [];
if (npmModules) {
    _.each(npmModules,function (item, key) {
        vendorLibs.push(key);
    });
}

module.exports = {
    entry: {
        app: `${SRC_DIR}/init.js`,
        vendor: vendorLibs
    },
    output: {
        path: SRC_DIR,
        filename: "bundle.js"
    },
    plugins: [
        new webpack.optimize.CommonsChunkPlugin({ name: 'vendor', filename: 'vendor.js' })
    ],
    devtool: 'eval',
    resolve: {
        extensions: ['.js', '.json'],
        modules: [
            SRC_DIR,
            NODE_MODULES_DIR
        ]
    }
};
