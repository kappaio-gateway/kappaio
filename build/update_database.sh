#! /bin/sh
die () {
    echo >&2 "$@"
    exit 1
}
./json_encode.lua ../src/widgetLib.js ../files/widget.json
./js_encode.lua ../src/widgetObjLib.js ../files/widgetObj.json
scp ../files/*.json ${1}:/usr/lib/rsserial
