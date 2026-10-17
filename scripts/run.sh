#!/usr/bin/env bash

meson setup --reconfigure build --prefix=$(pwd)/deploy
#meson setup --reconfigure -Db_sanitize=address build
meson configure build -Dbuildtype=debug
#meson configure build -Dbuildtype=release
cd build
meson compile
meson install --no-rebuild --only-changed --tags deploy
cd ..
cd deploy
./harbinger-testbed