#pragma once
#include <string>
#include <obs-module.h>

gs_texrender_t* createOrResetTexrender(gs_texrender_t* render);
void setBlendingParameters();
gs_effect_t* loadShaderEffect(std::string effectFilePath, gs_effect_t* effect);
std::string loadShaderFromFile(std::string file_name);

void setting_visibility(const char *prop_name, bool visible,
			       obs_properties_t *props);

