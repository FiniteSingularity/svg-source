<!-- Title -->
# SVG Source Plugin for OBS

<p align="center">
    <i align="center">Add scalable images to your OBS setup with SVG Source.</i>
</p>

<h4 align="center">
    <a href="https://github.com/FiniteSingularity/svg-source/releases">
        <img src="https://img.shields.io/github/v/release/finitesingularity/svg-source?filter=*&style=flat-square&label=Latest" alt="Latest release">
    </a>
    <img src="https://img.shields.io/badge/OBS-30+-blue.svg?style=flat-square" alt="supports obs versions 30+">
    <img src="https://img.shields.io/badge/Windows-0078D6?style=flat-square&logo=windows&logoColor=white">
    <img src="https://img.shields.io/badge/mac%20os-000000?style=flat-square&logo=apple&logoColor=white">
    <img src="https://img.shields.io/badge/Linux-FCC624?style=flat-square&logo=linux&logoColor=black"><br>
    <a href="https://twitter.com/FiniteSingulrty">
        <img src="https://img.shields.io/badge/Twitter-1DA1F2?style=flat-square&logo=twitter&logoColor=white">
    </a>
    <a href="https://twitch.tv/finitesingularity">
        <img src="https://img.shields.io/badge/Twitch-9146FF?style=flat-square&logo=twitch&logoColor=white">
    </a>
</h4>

> **Note**
> While we only release new versions of SVG Source after testing it on local hardware, there are bugs and issues that will slip through. If you happen to run into any issues, please [open an issue](https://github.com/finitesingularity/svg-source/issues) and we will work to resolve it.

## Introduction
An SVG (Scalable Vector Graphics) file is an XML-based image format that uses vector data to display crisp, resolution-independent graphics at any size. The SVG Source plugin adds native support for SVG files and text in OBS, letting you easily integrate scalable, dynamic graphics into your scenes.

- 📄 Allows for use of SVG files, and a text-based input where you can paste raw SVG data.
- 📏 Scale SVGs by width or height (maintaining aspect ratio) or scale by both.
- 🖥️ Specify a maximum generated texture size to either save memory, or create highly scalable images.

### Usage
After installing the SVG Source plugin, you will see a new source called "SVG" that can be added to your scenes. You will then see the following dialog:

![SVG Source Interface](.github/assets/images/properties.png)

- *Input Type:* Select if you want to provide an SVG file, or paste SVG XML text.
- *File:* File selector if you selected `File` above.
- *Text:* Text box for SVG XML text if you selected `Text` above.
- *Scale By:* Scale the source by width or height (preserving aspect ratio) or by both.
- *Width:* Width of the source in pixels (Shown if `Width` or `Both` selected above).
- *Height:* Height of the source in pixels (Shown if `Height` or `Both` selected above).
- *Max Texture Size:* The maximum dimension of the texture generated from the SVG. If you are not rescaling the SVG you can set this to the `Width` or `Height` value specified. If you are scaling the SVG (e.g.- using the Move plugin to resize it) set this to the maximum size you want to scale to (up to 10000 pixels).

Thank you for exploring the SVG Source plugin! Your feedback and contributions are highly appreciated as we continue to improve and expand this project.