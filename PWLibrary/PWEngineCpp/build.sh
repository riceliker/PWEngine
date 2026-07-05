cd "$(dirname "$0")" || exit 1

sh "../PWLibrary/build.sh"

cp -v "../PWLibrary/build/libPWLibrary.dylib" "./lib/"
cp -rf "../PWLibrary/include/." "./lib/"

xmake build