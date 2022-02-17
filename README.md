# Mokuzai

![GIF](https://user-images.githubusercontent.com/48701454/154351831-b9755412-5c7d-4d0f-86f2-957ccb12104c.gif)

This was a project made in 3 ~(gruesome)~ days with C++, FreeGLUT and OpenGL for a school assignment.

It's a tree generation application that features:
* The ability to fully customize many aspects of the tree, from the allowed angles to spawn branches, to establishing a minimum height needed to spawn them.
* Fully exportable models as .obj files.
* Seeded generation.
* Wireframe mode.
* Multiple textures to visualize with.
* Full 3D mouse controls.
* Session management and saving.
* Application of some basic UV mapping to the generated models.
* Fully exposed images that allow you to add your own preview textures.

It was mainly made following [Charlie Hewitt's "Procedural Generation of Tree Models
For Use In Computer Graphics"](https://chewitt.me/Papers/CTH-Dissertation-2017.pdf) paper.

The models are somewhat rough, a little disconnected on some angles, and not fully efficient on the polycount (mainly due to the implementation of cylinders without the top and bottom being hollowed out). But none of it is really that noticeable unless you deliberately look for it.

# Environment Setup

For legacy purposes, the following section will contain instructions to setup an enviroment identical to the one used while developing.

The bulk of the installation is just following [this (reuploaded as legacy) video](https://youtu.be/ugmAdJs2_R4) that downloads the following:

* [CodeBlocks 13.12 (MinGW Version)](https://sourceforge.net/projects/codeblocks/files/Binaries/13.12/Windows/codeblocks-13.12mingw-setup.exe/download)
* [Freeglut](https://www.transmissionzero.co.uk/software/freeglut-devel/)

Once you have this CodeBlocks version installed, you can simply open `Tree Generation.cbp` file at the root of the project from within the IDE.

If you try to run it as is, you'll notice that many textures are going to be missing, this is because the program will expect for these files to be either in a specified directory (while on DEBUG) or in the same folder as the exe (on builds). To edit the DEBUG path, simply go to `src\Utilities.cpp` and modify these lines to whatever you want:

![image](https://user-images.githubusercontent.com/48701454/154349798-258bb53b-131e-425f-b658-f5b207311cd7.png)

Don't forget to also unzip the contents of `Mokuzai Assets.zip` on the folder you pick for this, or to leave these assets next to the .exe on builds.
