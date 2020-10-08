# Welcome to Fractal Generator - A real-time fractal world explorer

The Mandelbrot fractal is a psichodelic mathematically-generated image that is self-like through different scales. The user will be able to zoom in and out through the fractal in order to explore it. In the upcoming future the application will integrate a GUI that will provide the user with the possibility of modifying the colouring of the fractal and applying different presets. These presets will allow the user to visualize and compare the Time Complexity of some of the most popular computer science algorithms (e.g. binary search, euclidean algorithm, Huffman algorithm, Djiistra...).

![Fractal](screenshots/10_03_Fractal_World_1.jpg)

## Getting Started

You can either clone the repository and generate the project with GenerateProject.bat or directly access the lastest binaries.<br/>

Use UP and DOWN to zoom in and out to the cursor location. Use LEFT and RIGHT to decrease or increase the fractal detail level (with a performance impact)<br/>

[Binaries](https://github.com/ManuCanedo/fractal-generator/tree/master/bin)  
[Source Code](https://github.com/ManuCanedo/fractal-generator/tree/master/src)   
[Screenshots](https://github.com/ManuCanedo/fractal-generator/tree/master/media/screenshots) 

## Development progress

Engine Progress checklist:
+ Application Engine set up
+ Logger implemented  
+ Function Timer implemented
+ Project generation through Premake
+ Pixel Engine Integration
+ Memory Allocations tracker implemented 
+ Using precompiled headers
+ Fractal algorithm multithreading
+ Started GLFW and OpenGL integration (replacing Pixel Engine)
+ Window Abstraction and platform specific implementation for Windows
+ Custom Event manager (moved away from Pixel Engine events)
+ Custom renderer using GLEW and OpenGL (fully removed support for Pixel Engine)

Fractal Application Progress checklist:
+ Bitmap Images Generation
+ Mandelbrot Fractal Generation
+ Histogram Colouring
+ Static zoom application
+ Real Time Fractal Generation
+ Real Time Sinusoidal Colouring
+ Screenshots

## Stay in touch

+ Linkedin - https://www.linkedin.com/in/manuel-tabares/
