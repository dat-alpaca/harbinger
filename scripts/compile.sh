#!/usr/bin/env bash

glslc -O -fshader-stage=vert -o ../res/tilemap_vert.spv ../res/tilemap_vert.glsl
glslc -O -fshader-stage=frag -o ../res/tilemap_frag.spv ../res/tilemap_frag.glsl