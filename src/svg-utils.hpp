#pragma once

#include <obs-module.h>

gs_texture_t* gsTextureFromSvgPath(const char* path, int width, int height, int scale_by);
gs_texture_t* gsTextureFromSvg(const char* svg, int width, int height, int scale_by);
uint32_t nextPowerOf2(uint32_t n);