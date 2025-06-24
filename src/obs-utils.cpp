#include "obs-utils.hpp"
#include "svg-source-support.h"

#include <util/platform.h>
#include <vector>
#include <sstream>

gs_texrender_t* createOrResetTexrender(gs_texrender_t* render)
{
	if (!render) {
		render = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
	}
	else {
		gs_texrender_reset(render);
	}
	return render;
}

void setBlendingParameters()
{
	gs_blend_state_push();
	gs_reset_blend_state();
	gs_enable_blending(false);
	gs_blend_function(GS_BLEND_ONE, GS_BLEND_ZERO);
}

void setRenderParameters()
{
	// Culling
	gs_set_cull_mode(GS_NEITHER);

	// Enable all colors.
	gs_enable_color(true, true, true, true);

	// Depth- No depth test.
	gs_enable_depth_test(false);
	gs_depth_function(GS_ALWAYS);

	// Setup Stencil- No test, no write.
	gs_enable_stencil_test(false);
	gs_enable_stencil_write(false);
	gs_stencil_function(GS_STENCIL_BOTH, GS_ALWAYS);
	gs_stencil_op(GS_STENCIL_BOTH, GS_ZERO, GS_ZERO, GS_ZERO);
}

gs_effect_t* loadShaderEffect(std::string effectFilePath, gs_effect_t* effect)
{
	if (effect != NULL) {
		obs_enter_graphics();
		gs_effect_destroy(effect);
		effect = NULL;
		obs_leave_graphics();
	}
	
	std::string filename = obs_get_module_data_path(obs_current_module()) + effectFilePath;
	std::string shaderText = loadShaderFromFile(filename);
	char* errors = NULL;

	obs_enter_graphics();
	effect = gs_effect_create(shaderText.c_str(), NULL, &errors);
	obs_leave_graphics();

	if (effect == NULL) {
		obs_log(LOG_WARNING,
			"Unable to load .effect file.  Errors:\n%s",
			(errors == NULL || strlen(errors) == 0 ? "(None)"
				: errors));
		bfree(errors);
	}

	return effect;
}

bool startsWith(const std::string& str, const std::string& prefix) {
	return str.size() >= prefix.size() &&
		str.compare(0, prefix.size(), prefix) == 0;
}

// Performs loading of shader from file.  Properly includes #include directives.
std::string loadShaderFromFile(std::string fileName)
{
	char* file = os_quick_read_utf8_file(fileName.c_str());
	if (file == NULL)
		return NULL;
	std::string fileStr = file;
	bfree(file);
	std::vector<std::string> lines;
	std::istringstream stream(fileStr);
	std::string l;
	while (std::getline(stream, l)) {
		lines.push_back(l + '\n');
	}

	std::string shaderFile;

	for (auto const& line : lines) {
		if (startsWith(line, "#include")) {
			auto start = line.find('"');
			auto end = line.find('"', start + 1);
			std::string include;
			if (start != std::string::npos && end != std::string::npos && end > start) {
				include = line.substr(start + 1, end - start - 1);
			}

			std::string includePath = fileName + '/' + include;

			char* absIncludePath =
				os_get_abs_path_ptr(includePath.c_str());
			std::string fileContents =
				loadShaderFromFile(absIncludePath);
			shaderFile += fileContents + '\n';
			bfree(absIncludePath);
		} else {
			// else place current line here.
			shaderFile += line;
		}
	}
	return shaderFile;
}


void setting_visibility(const char *prop_name, bool visible,
			obs_properties_t *props)
{
	obs_property_t *p = obs_properties_get(props, prop_name);
	obs_property_set_enabled(p, visible);
	obs_property_set_visible(p, visible);
}