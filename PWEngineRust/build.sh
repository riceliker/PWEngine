cd "$(dirname "$0")" || exit 1

sh "../PWEngineCAPI/build.sh"

cp -v "../PWEngineCAPI/build/libPWEngineCAPI.dylib" "./lib/"
cp -rf "../PWEngineCAPI/include/." "./lib/"