# Welcome to Fractal Generator - A real-time fractal world explorer

The Mandelbrot fractal is a psichodelic mathematically-generated image that is self-like through different scales. Zoom in and out on the fractal to explore it! You can gradually apply a maximum zoom of around 1: 10^300 discovering countless fractal underworlds in the process. 

An UI offers you the possibility to modify the fractal colours and apply other performance settings. In addition, a fixed preset allows you to visuallise the Time Complexity of the Binary Search algorithm and compare it with the corresponding complexity of the fractal.

![Fractal](screenshots/Fractal_Screenshot_User2.jpg)

## Getting Started

You can either clone the repository and generate the project with GenerateProject.bat or directly access the lastest binaries.<br/>

*MOUSE WHEEL BUTTON* (click and drag) ----- move around the Fractal.  
*UP* and *DOWN* keys ----- zoom in and out.  
*LEFT* and *RIGHT* keys ----- decrease or increase the fractal detail level.  
*SPACEBAR* key ----- take a screenshot.  
*ESC* key ----- quit the application.<br/>

Expand the *Settings* interface to play around with the colouring and other configurations.<br/>

[Executable](https://github.com/ManuCanedo/fractal-generator/tree/master/bin)  
[Source Code](https://github.com/ManuCanedo/fractal-generator/tree/master/src)   
[Screenshots](https://github.com/ManuCanedo/fractal-generator/tree/master/screenshots) 

## Development Checklist

Engine progress:
+ **Application Engine** set up
+ (Dev Tool) Logger implemented  
+ (Dev Tool) Function Timer implemented
+ (Dev Tool) Project generation generation using Premake
+ (Dev Tool) Pixel Engine Integration
+ (Dev Tool) Memory Allocation tracker 
+ (Dev Tool) Using precompiled headers
+ Fractal algorithm **multithreading**
+ GLFW and OpenGL (GLEW) integration (Pixel Engine replaced)
+ Window Abstraction and **platform specific Window implementation** for Windows
+ Custom **Event Manager**
+ **ImGui** Layer Abstraction and platform specific implementation
+ Fractal Generation using **AVX2** CPU vector extensions (Compiler Intrinsics)
+ (Dev Tool) CPU Features analizer tool implemented
+ Checking CPU support for AVX2 instructions at launch


Fractal application progress:
+ Bitmap Images Generation
+ **Mandelbrot Fractal Generation**
+ Histogram Colouring
+ Static zoom application
+ **Real Time Fractal Generation**
+ Real Time Sinusoidal Colouring
+ **Screenshots**
+ **UI** and fractal configuration in real time


## Stay in touch

+ Linkedin - https://www.linkedin.com/in/manuel-tabares/
