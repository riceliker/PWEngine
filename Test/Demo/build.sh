#!/bin/zsh

cd "$(dirname "$0")" || exit 1

cp "../../PWEngine/build/libPWEngine.a" "./lib"
cp -rf "../../PWEngine/include" "./"

xmake

