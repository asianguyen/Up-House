# CS1230 Final Project
Team members:
- Sophia Lim (szlim)
- Asia Nguyen (atnguyen)
- Natalie King (nking12)
- Jinho Lee (jlee812)

## Overview

New Technical Features:
- Bump mapping: add realistic details to the house (szlim and atnguyen)
- Bezier curve camera motion: house "floating away" (jlee812)
- Skybox: large skybox showing clear sky textures  to set the high-altitude scene (szlim)
- Physics: house swaying to mock wind (atnguyen)
- Balloon movement: (atnguyen and nking12)

Additional
- Blender model rendering for a more realistic house and balloon objects (atnguyen and nking12)
- .obj parser for extracting house and balloon data (atnguyen)
- Final video (jlee812)

## How to use
1. Change the filepaths of the images and .obj/.mtl files to reflect your own directory
2. Run the program
3. Select "Upload Scene File"
4. Use "mesh.json" located in the house/ folder
5. Once loaded, set the far plane to 100 using the slider
6. Choose your bezier curve option:
None- no motion but can use WASD, space, and command keys to move and rotate around the scene with mouse
Circle - Circles around the house. Can use sliders to change camera distance and y position
Curve - House moves in a curve
7. Choose bezier movement speed for Circle and Curve options
8. Toggle on/off the bump mapping textures

## External Libraries
- stb library for loading images for the skybox: https://github.com/nothings/stb/blob/master/stb_image.h (sophia)
