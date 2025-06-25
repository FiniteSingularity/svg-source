#pragma once

#include <string>
#include <vector>

#include <obs-module.h>
#include <svg-source-support.h>

#define SVG_SCALE_WIDTH 1
#define SVG_SCALE_WIDTH_LABEL "SvgSource.ScaleBy.Width"
#define SVG_SCALE_HEIGHT 2
#define SVG_SCALE_HEIGHT_LABEL "SvgSource.ScaleBy.Height"
#define SVG_SCALE_BOTH 3
#define SVG_SCALE_BOTH_LABEL "SvgSource.ScaleBy.Both"

#define SVG_INPUT_TYPE_FILE 1
#define SVG_INPUT_TYPE_FILE_LABEL "SvgSource.InputType.File"
#define SVG_INPUT_TYPE_TEXT 2
#define SVG_INPUT_TYPE_TEXT_LABEL "SvgSource.InputType.Text"

#define SVG_GENERATE_NEW_SMALL 1
#define SVG_GENERATE_NEW_LARGE 2
#define SVG_GENERATE_ALL 3

class SVGSource {
public:
	SVGSource(obs_data_t *settings, obs_source_t *obs_source);
	~SVGSource();

	static void *Create(obs_data_t *settings, obs_source_t *obs_source);
	static void Destroy(void *data);
	static void Update(void *data, obs_data_t *settings);
	static const char *GetName(void *);
	static void VideoRender(void *data, gs_effect_t *effect);
	static void VideoTick(void *data, float seconds);
	static uint32_t GetWidth(void *data);
	static uint32_t GetHeight(void *data);
	static obs_properties_t *GetProperties(void *data);
	static void GetDefaults(obs_data_t *settings);

	static bool ScaleByChanged(obs_properties_t *props,
				   obs_property_t *property,
				   obs_data_t *settings);
	static bool InputTypeChanged(obs_properties_t *props,
				     obs_property_t *property,
				     obs_data_t *settings);

private:
	void RenderSVGToTexture_();
	void LoadSvgEffect_();

	gs_effect_t *effect_ = nullptr;

	gs_eparam_t *paramSvgImage_ = nullptr;
	gs_eparam_t *paramUvSize_ = nullptr;
	gs_eparam_t *paramSvgUvSize_ = nullptr;

	obs_source_t *source_;
	uint32_t scaleBy_;
	uint32_t maxTextureSize_;
	int inputType_;
	std::string svgImagePath_;
	std::string svgText_;
	uint32_t targetWidth_;
	uint32_t targetHeight_;
	uint32_t svgRenderWidth_;
	uint32_t svgRenderHeight_;
	uint32_t textureHeight_;
	uint32_t textureWidth_;
	uint32_t textureIndex_;

	std::vector<gs_texture_t *> textures_;
};

obs_source_info svg_source_info();
