#!/bin/bash

# Check if lv_i18n is installed
if npm list -g lv_i18n >/dev/null 2>&1; then
    echo "lv_i18n is already installed."
else
    echo "lv_i18n is not installed. Please run the following command to install it:"
    echo "npm install -g littlevgl/lv_i18n"
    exit -1
fi

lv_i18n extract -s '../../**/*.+(c|cpp|h|hpp)' -t './translations/*.yml'
lv_i18n compile -t './translations/*.yml' -o './'
