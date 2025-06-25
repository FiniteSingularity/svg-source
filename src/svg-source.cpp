#include "svg-source.hpp"
#include "svg-utils.hpp"
#include "obs-utils.hpp"

SVGSource::SVGSource(obs_data_t *settings, obs_source_t *obs_source)
	: source_(obs_source),
	  effect_(nullptr)
{
	LoadSvgEffect_();
	Update(this, settings);
}

SVGSource::~SVGSource()
{
	obs_enter_graphics();
	if (effect_) {
		gs_effect_destroy(effect_);
	}

	for (auto &texture : textures_) {
		gs_texture_destroy(texture);
	}
	obs_leave_graphics();
}

void *SVGSource::Create(obs_data_t *settings, obs_source_t *obs_source)
{
	auto source = new SVGSource(settings, obs_source);
	return source;
}

void SVGSource::Destroy(void *data)
{
	auto instance = static_cast<SVGSource *>(data);
	delete instance;
}

const char *SVGSource::GetName(void *)
{
	return "SVG";
}

void SVGSource::Update(void *data, obs_data_t *settings)
{
	auto &instance = *static_cast<SVGSource *>(data);

	uint32_t w = obs_source_get_width(instance.source_);
	uint32_t h = obs_source_get_height(instance.source_);

	if (w > 0) {
		obs_data_set_int(settings, "source_width", w);
	} else {
		w = (uint32_t)obs_data_get_int(settings, "source_width");
	}

	if (h > 0) {
		obs_data_set_int(settings, "source_height", h);
	} else {
		h = (uint32_t)obs_data_get_int(settings, "source_height");
	}

	SVGSource::GetDefaults(settings, w, h);

	const char *svgImagePath = obs_data_get_string(settings, "svg_image");
	const char *svgText = obs_data_get_string(settings, "svg_text");

	int inputType = (int)obs_data_get_int(settings, "svg_input_type");
	uint32_t width = (uint32_t)obs_data_get_int(settings, "svg_width");
	uint32_t height = (uint32_t)obs_data_get_int(settings, "svg_height");
	uint32_t scaleBy = (uint32_t)obs_data_get_int(settings, "svg_scale_by");
	uint32_t maxTextureSize =
		(uint32_t)obs_data_get_int(settings, "svg_max_texture_size");

	bool imagePathChanged = false;
	bool scaleByChanged = instance.scaleBy_ != scaleBy;
	bool maxTextureSizeChanged = instance.maxTextureSize_ != maxTextureSize;
	bool inputTypeChanged = instance.inputType_ != inputType;

	if (inputType == SVG_INPUT_TYPE_FILE) {
		imagePathChanged = instance.svgImagePath_ != svgImagePath;
	} else {
		imagePathChanged = instance.svgText_ != svgText;
	}

	bool regen_bitmap = imagePathChanged || inputTypeChanged;
	instance.svgImagePath_ = svgImagePath;
	instance.svgText_ = svgText;

	instance.inputType_ = inputType;

	instance.targetWidth_ = width;
	instance.targetHeight_ = height;
	instance.scaleBy_ = scaleBy;

	instance.maxTextureSize_ = maxTextureSize;

	bool empty = ((inputType == SVG_INPUT_TYPE_FILE &&
		       instance.svgImagePath_.empty()) ||
		      (inputType == SVG_INPUT_TYPE_TEXT &&
		       instance.svgText_.empty()));

	if ((regen_bitmap || scaleByChanged || maxTextureSizeChanged) &&
	    !empty) {
		instance.RenderSVGToTexture_();
	}

	uint32_t npt;
	switch (instance.scaleBy_) {
	case SVG_SCALE_WIDTH:
		instance.svgRenderWidth_ = instance.targetWidth_;
		instance.svgRenderHeight_ =
			(uint32_t)((double)instance.targetWidth_ *
				   (double)instance.textureHeight_ /
				   (double)instance.textureWidth_);
		npt = nextPowerOf2(instance.targetWidth_);
		instance.textureIndex_ =
			(std::min)((uint32_t)log2(npt) - 3,
				   (uint32_t)instance.textures_.size() - 1);
		break;
	case SVG_SCALE_HEIGHT:
		instance.svgRenderHeight_ = instance.targetHeight_;
		instance.svgRenderWidth_ =
			(uint32_t)((double)instance.targetHeight_ *
				   (double)instance.textureWidth_ /
				   (double)instance.textureHeight_);
		npt = nextPowerOf2(instance.targetHeight_);
		instance.textureIndex_ =
			(std::min)((uint32_t)log2(npt) - 3,
				   (uint32_t)instance.textures_.size() - 1);
		break;
	case SVG_SCALE_BOTH:
		instance.svgRenderWidth_ = instance.targetWidth_;
		instance.svgRenderHeight_ = instance.targetHeight_;
		npt = nextPowerOf2(instance.targetWidth_);
		instance.textureIndex_ =
			(std::min)((uint32_t)log2(npt) - 3,
				   (uint32_t)instance.textures_.size() - 1);
		break;
	}
}

void SVGSource::VideoRender(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);
	auto &instance = *static_cast<SVGSource *>(data);

	if (instance.textures_.size() == 0) {
		return;
	}

	gs_texture_t *svgTexture = instance.textures_[instance.textureIndex_];

	const bool previous = gs_framebuffer_srgb_enabled();
	gs_enable_framebuffer_srgb(true);

	gs_blend_state_push();
	gs_blend_function(GS_BLEND_ONE, GS_BLEND_INVSRCALPHA);

	gs_effect_set_texture_srgb(instance.paramSvgImage_, svgTexture);
	//gs_eparam_t* const param = gs_effect_get_param_by_name(effect, "image");
	//gs_effect_set_texture_srgb(param, svgTexture);

	gs_technique_t *tech =
		gs_effect_get_technique(instance.effect_, "Draw");
	gs_technique_begin(tech);
	gs_technique_begin_pass(tech, 0);
	gs_draw_sprite(svgTexture, 0, instance.svgRenderWidth_,
		       instance.svgRenderHeight_);
	gs_technique_end_pass(tech);
	gs_technique_end(tech);

	gs_blend_state_pop();

	gs_enable_framebuffer_srgb(previous);
}

void SVGSource::VideoTick(void *data, float seconds)
{
	UNUSED_PARAMETER(data);
	UNUSED_PARAMETER(seconds);
	//auto& instance = *static_cast<SVGSource*>(data);
}

uint32_t SVGSource::GetWidth(void *data)
{
	auto &instance = *static_cast<SVGSource *>(data);
	return instance.svgRenderWidth_;
}

uint32_t SVGSource::GetHeight(void *data)
{
	auto &instance = *static_cast<SVGSource *>(data);
	return instance.svgRenderHeight_;
}

obs_properties_t *SVGSource::GetProperties(void *data)
{
	UNUSED_PARAMETER(data);
	//auto &instance = *static_cast<SVGSource *>(data);
	obs_properties_t *props = obs_properties_create();

	obs_properties_t *svg_group = obs_properties_create();
	obs_properties_t *svg_advanced_group = obs_properties_create();
	obs_property_t *p;

	obs_property_t *input_type = obs_properties_add_list(
		svg_group, "svg_input_type",
		obs_module_text("SvgSource.InputType"), OBS_COMBO_TYPE_LIST,
		OBS_COMBO_FORMAT_INT);

	obs_property_list_add_int(input_type,
				  obs_module_text(SVG_INPUT_TYPE_FILE_LABEL),
				  SVG_INPUT_TYPE_FILE);

	obs_property_list_add_int(input_type,
				  obs_module_text(SVG_INPUT_TYPE_TEXT_LABEL),
				  SVG_INPUT_TYPE_TEXT);

	obs_property_set_modified_callback(input_type,
					   SVGSource::InputTypeChanged);

	obs_properties_add_path(svg_group, "svg_image",
				obs_module_text("SvgSource.File"),
				OBS_PATH_FILE, "Textures (*.svg);;", NULL);

	obs_properties_add_text(svg_group, "svg_text",
				obs_module_text("SvgSource.Text"),
				OBS_TEXT_MULTILINE);

	obs_property_t *scale_by = obs_properties_add_list(
		svg_group, "svg_scale_by", obs_module_text("SvgSource.ScaleBy"),
		OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);

	obs_property_list_add_int(scale_by,
				  obs_module_text(SVG_SCALE_WIDTH_LABEL),
				  SVG_SCALE_WIDTH);

	obs_property_list_add_int(scale_by,
				  obs_module_text(SVG_SCALE_HEIGHT_LABEL),
				  SVG_SCALE_HEIGHT);

	obs_property_list_add_int(scale_by,
				  obs_module_text(SVG_SCALE_BOTH_LABEL),
				  SVG_SCALE_BOTH);

	obs_property_set_modified_callback(scale_by, SVGSource::ScaleByChanged);

	p = obs_properties_add_int(svg_group, "svg_width",
				   obs_module_text("SvgSource.Width"), 0, 20000,
				   1);
	obs_property_float_set_suffix(p, "px");

	p = obs_properties_add_int(svg_group, "svg_height",
				   obs_module_text("SvgSource.Height"), 0,
				   20000, 1);
	obs_property_float_set_suffix(p, "px");

	obs_properties_add_group(props, "svg_group",
				 obs_module_text("SvgSource.Label"),
				 OBS_GROUP_NORMAL, svg_group);

	p = obs_properties_add_int(svg_advanced_group, "svg_max_texture_size",
				   obs_module_text("SvgSource.MaxTextureSize"),
				   8, 8196, 1);

	obs_properties_add_group(props, "svg_advanced_group",
				 obs_module_text("SvgSource.Advanced"),
				 OBS_GROUP_NORMAL, svg_advanced_group);

	std::string info = "<a href=\"https://github.com/finitesingularity/svg-source/\">SVG Source</a> (";
	info += PLUGIN_VERSION;
	info += ") by <a href=\"https://twitch.tv/finitesingularity\">FiniteSingularity</a>";

	obs_properties_add_text(props, "plugin_info", info.c_str(),
				OBS_TEXT_INFO);

	return props;
}

void SVGSource::GetDefaults(obs_data_t *settings, uint32_t w, uint32_t h)
{

	struct obs_video_info info;
	obs_get_video_info(&info);
	uint32_t size = (std::min)(info.base_width, info.base_height);
	obs_data_set_default_int(settings, "svg_width", size);
	obs_data_set_default_int(settings, "svg_height", size);
	obs_data_set_default_int(settings, "svg_max_texture_size",
				 2 * size);
	obs_data_set_default_int(settings, "svg_input_type",
				 SVG_INPUT_TYPE_FILE);
}

void SVGSource::RenderSVGToTexture_()
{
	gs_texture_t *(*genTexture)(const char *, int, int, int);

	std::string svgData;

	if (inputType_ == SVG_INPUT_TYPE_FILE) {
		genTexture = &gsTextureFromSvgPath;
		svgData = svgImagePath_;
	} else {
		genTexture = &gsTextureFromSvg;
		svgData = svgText_;
	}

	for (size_t i = 0; i < textures_.size(); i++) {
		gs_texture_destroy(textures_[i]);
	}
	textures_.clear();

	const int maxSize = (int)maxTextureSize_;
	int lastTexture = 1000000;

	for (int i = 8; i <= maxSize; i *= 2) {
		gs_texture_t *tex = genTexture(svgData.c_str(), i, i, scaleBy_);
		textures_.push_back(tex);
		obs_enter_graphics();
		textureWidth_ = gs_texture_get_width(tex);
		textureHeight_ = gs_texture_get_height(tex);
		obs_leave_graphics();
		lastTexture = i;
	}
	if (lastTexture < maxSize) {
		gs_texture_t *tex =
			genTexture(svgData.c_str(), maxSize, maxSize, scaleBy_);
		textures_.push_back(tex);
		obs_enter_graphics();
		textureWidth_ = gs_texture_get_width(tex);
		textureHeight_ = gs_texture_get_height(tex);
		obs_leave_graphics();
	}
}

void SVGSource::LoadSvgEffect_()
{
	const char *effectFilePath = "/shaders/svg.effect";

	effect_ = loadShaderEffect(effectFilePath, effect_);
	if (effect_) {
		size_t effect_count = gs_effect_get_num_params(effect_);
		for (size_t effect_index = 0; effect_index < effect_count;
		     effect_index++) {
			gs_eparam_t *param = gs_effect_get_param_by_idx(
				effect_, effect_index);
			struct gs_effect_param_info info;
			gs_effect_get_param_info(param, &info);
			if (strcmp(info.name, "image") == 0) {
				paramSvgImage_ = param;
			}
		}
	}
}

bool SVGSource::ScaleByChanged(obs_properties_t *props,
			       obs_property_t *property, obs_data_t *settings)
{
	UNUSED_PARAMETER(property);
	int scale_by = (int)obs_data_get_int(settings, "svg_scale_by");
	switch (scale_by) {
	case SVG_SCALE_WIDTH:
		setting_visibility("svg_width", true, props);
		setting_visibility("svg_height", false, props);
		break;
	case SVG_SCALE_HEIGHT:
		setting_visibility("svg_width", false, props);
		setting_visibility("svg_height", true, props);
		break;
	case SVG_SCALE_BOTH:
		setting_visibility("svg_width", true, props);
		setting_visibility("svg_height", true, props);
		break;
	}
	return true;
}

bool SVGSource::InputTypeChanged(obs_properties_t *props,
				 obs_property_t *property, obs_data_t *settings)
{
	UNUSED_PARAMETER(property);
	int input_type = (int)obs_data_get_int(settings, "svg_input_type");
	switch (input_type) {
	case SVG_INPUT_TYPE_FILE:
		setting_visibility("svg_image", true, props);
		setting_visibility("svg_text", false, props);
		break;
	case SVG_INPUT_TYPE_TEXT:
		setting_visibility("svg_image", false, props);
		setting_visibility("svg_text", true, props);
		break;
	}
	return true;
}

obs_source_info svg_source_info()
{
	obs_source_info source_info = {};
	source_info.id = "svg_source";
	source_info.type = OBS_SOURCE_TYPE_INPUT;
	source_info.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW |
				   OBS_SOURCE_SRGB;
	source_info.get_name = SVGSource::GetName;
	source_info.create = SVGSource::Create;
	source_info.destroy = SVGSource::Destroy;
	source_info.update = SVGSource::Update;
	source_info.video_render = SVGSource::VideoRender;
	source_info.video_tick = SVGSource::VideoTick;
	source_info.get_width = SVGSource::GetWidth;
	source_info.get_height = SVGSource::GetHeight;
	source_info.get_properties = SVGSource::GetProperties;
	source_info.icon_type = OBS_ICON_TYPE_IMAGE;
	//source_info.get_defaults = SVGSource::GetDefaults;
	return source_info;
}
