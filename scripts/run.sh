#!/usr/bin/env bash

meson setup --reconfigure build --prefix=$(pwd)/deploy
meson configure build -Dbuildtype=release

cd build
meson compile
meson install --no-rebuild --only-changed --tags deploy

cd ..
cd deploy
./harbinger-testbed