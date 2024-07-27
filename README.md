# *Into the Portal: Directable Fractal Self-Similarity*

[![DOI:10.1111/cgf.14905](https://zenodo.org/badge/DOI/10.1145/3641519.3657466.svg)](https://doi.org/10.1145/3641519.3657466)

## Intro
![Four self-similar fractals: a bunny, a statue of Hebe, a fox's tail, a cat](https://github.com/user-attachments/assets/b1f31c93-b81d-4c64-b0b7-183385c6e37c)

This is the code for the paper titled _Into the Portal: Directable Fractal
Self-Similarity_, published at SIGGRAPH 2024 by Alexa Schor (me) and [Theodore
Kim](https://tkim.graphics).

Download the paper:
[PDF (42MiB)](https://alexaschor.com/into-the-portal/files/into_the_portal.pdf),
[PDF low-res (5MiB)](https://alexaschor.com/into-the-portal/files/into_the_portal_smaller.pdf)

See turntable and zoom videos of our results here:
[YouTube link](https://youtu.be/8X9RlcaklHU)


_Abstract:_ We present a novel, directable method for introducing fractal
self-similarity into arbitrary shapes. Our method allows a user to directly
specify the locations of self-similarities in a Julia set and is general enough
to reproduce other well-known fractals such as the Koch snowflake.

Ours is the first algorithm to enable this level of general artistic control
while also maintaining the character of the original fractal shape. We
introduce the notion of placing “portals” into the iteration space of a
dynamical system, bridging the aesthetics of iterated maps with the
fine-grained control of iterated function systems (IFS). Our method is
effective in both 2D and 3D.

## This Repo

First of all, thanks for coming to this page! If you have any questions or
ideas about the paper or code or any problems with building, running or
modifying the code, please open an issue here or email me at
`alexa.schor@yale.edu` and I'll do my best to help.

This is the code used to generate the meshes used in all the 3D paper figures.

### Repo Structure
```
[ ] (root)
 ├── * README.md (this file)
 ├── * Makefile (main Makefile: calls into the two project Makefiles)
 ├──[ ] projects
 │   ├── * include.mk (included in the two project Makefiles; defines compiler and flags)
 │   ├──[ ] main
 │   │   ├── * Makefile
 │   │   ├── * main.cpp (compiles into bin/run; builds Julia set, adds portals, marches)
 │   │   └── * prun.py (calls into bin/run to compute mesh in parallel, then stitches it back together)
 │   └──[ ] sdfGen (lightly modified version of github: christopherbatty/SDFGen)
 ├──[ ] src (common code that I share among different projects)
 │   ├── * field.h (provides 3D grid/field representations: caching, interpolation, gradients, etc.)
 │   ├── * julia.h (provides Julia set implementation: shape modulus, portals, etc.)
 │   ├── * MC.h (modified version of github: aparis69/MarchingCubeCpp)
 │   ├── * mesh.h (triangle mesh)
 │   ├── * SETTINGS.h (poorly named: contains debugging/timing/typedef macros)
 │   ├── * triangle.cpp, .h (functions on triangles)
 ├──[X] data.7z (lzma archive)
 │   └──[ ] fields (SDFs for example shapes)
 │       ├── * benchy100.f3d (benchy: 100^3)
 │       ├── * bunny100.f3d  (bunny:  100^3)
 │       ├── * chair100.f3d  (office chair: 100^3)
 │       ├── * hebe100.f3d   (hebe statue: 100^3)
 │       ├── * hebe300.f3d   (hebe statue: 300^3)
 │       └── * tooth100.f3d  (tooth: 100^3)
 ├──[ ] bin (compiled executables will end up here)
 │   └── prun (symlink to ../projects/main/prun.py)
 └──[ ] lib (external libraries)
     ├──[ ] Eigen (Eigen library version 3.3.9)
     ├──[ ] PerlinNoise (Perlin Noise implementation from github: reputeless/PerlinNoise)
     └──[ ] Quaternion (quaternion math implementations from github: theodorekim/QUIJIBO)
```


## Compiling

The program should compile with `make`. I've built it with `GNU Make
x86_64-pc-linux-gnu 4.4.1`, but I don't think my Makefiles use any GNU-specific
syntax. Before compiling, you'll want to specify your preferred compiler and
any flags you might want to enable in `projects/include.mk`.

All code in this repo is written to the C++17 standard. I've verified that it
compiles and runs with:
    - `g++ (GCC) 14.1.1 20240522` (used for the results in the paper)
    - `clang x86_64-pc-linux-gnu version 17.0.6` (seems to perform identically)

I've tested it on a Dell Latitude 7490 running Arch Linux, but that shouldn't
really matter. If you have compilation issues, feel free to reach out.

## Usage

Running `make` should yield two executables in `bin`: `bin/run` and
`bin/sdfGen`. The pipeline for producing self-similar fractals with this
program is the following:

1. Start with your target shape mesh as `*.obj`.
2. Run `bin/sdfGen` on that mesh to produce an SDF in a format that this
   program understands. This will yield a `*.f3d` signed distance field.
3. Run `bin/run` or `bin/prun` on that signed distance field to produce a
   self-similar fractal. This will yield an `*.obj` mesh that you can do what
   you please with.

### Reproducing the paper examples
