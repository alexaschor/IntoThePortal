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
to reproduce other well-known fractals such as the Koch snowflake. Ours is the
first algorithm to enable this level of general artistic control
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
 │   └── * triangle.cpp, .h (functions on triangles)
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
compiles and runs on my Linux machine with:

- `g++ (GCC) 14.1.1 20240522` (used for the results in the paper)
- `clang x86_64-pc-linux-gnu version 17.0.6` (seems to perform identically)

If you have compilation issues, feel free to reach out.

## Usage

### General usage

Running `make` should yield two executables in `bin`: `bin/run` and
`bin/sdfGen`. The pipeline for producing self-similar fractals with this
program is the following:

1. Start with your target shape mesh as `*.obj`.
2. Run `bin/sdfGen` on that mesh to produce an SDF in a format that this
   program understands. This will yield a `*.f3d` signed distance field.
3. Write a text file specifying the portal locations you want, if any. The
   format for this file is documented below.
4. Run `bin/run` or `bin/prun` on your signed distance field and your portal
   text file to produce a self-similar fractal. This will yield an `*.obj` mesh
   that you can do what you please with.

### Reproducing the paper examples

I've included the `*.f3d` SDFs and the `*.txt` portal files that we used to
produce the figures in the paper in the lzma archive `data.7z` - extracting it
will produce a structure as shown above in the **repo structure** diagram.

Specifically, to reproduce the bunny and hebe examples, first extract `data.7z`
and then run the following with either `./bin/prun` or `./bin/run`:

To run in up to 8x parallel with `prun` (requires trimesh2 `mesh_cat` and an `xargs` that supports the `-P` flag):
* **Bunny**: `./bin/prun data/fields/bunny100.f3d data/portals/bunny_ears.txt 1 9 300 10 0.1 0 0 0 bunny.obj`
* **Hebe**: `./bin/prun data/fields/hebe300.f3d data/portals/hebe.txt 1 9 300 0.29 8.2 0 0 0 hebe.obj`

To run single-threaded with `run` (no dependencies):
* **Bunny**: `./bin/prun data/fields/bunny100.f3d data/portals/bunny_ears.txt 1 9 300 10 0.1 0 0 0 bunny.obj`
* **Hebe**: `./bin/prun data/fields/hebe300.f3d data/portals/hebe.txt 1 9 300 0.29 8.2 0 0 0 hebe.obj`

These invocations can be understood by comparing with the usage documentation
below, but for convenience here's a table of the parameters used:

| Parameter              | Value (Bunny)                                    | Value (Hebe)                                     |
| ---------------------- | ------------------------------------------------ | ------------------------------------------------ |
| SDF file               | `data/fields/bunny100.f3d` (100<sup>3</sup> SDF) | `data/fields/hebe300.f3d` (300<sup>3</sup> SDF)  |
| Portal file            | `data/portals/bunny_ears.txt` (2 portals)        | `data/portals/hebe.txt` (1 portal)               |
| Versor noise octaves   | `1`                                              | `1`                                              |
| Versor noise scale     | `9`                                              | `9`                                              |
| Marching resolution    | `300` (Adjust to taste)                          | `300` (Adjust to taste)                          |
| Shape modulus alpha    | `10`                                             | `0.29`                                           |
| Shape modulus beta     | `0.1`                                            | `8.2`                                            |
| Origin offset          | `0 0 0` (No offset)                              | `0 0 0` (No offset)                              |
| Output filename        | `bunny.obj`                                      | `hebe.obj`                                       |

### Portal file syntax
Consider this example, the portal file for the bunny example:
```
Portals radius:      0.25
Portals scale:       4.50

Portal location:    -0.175255 0.441722 0.015167
Portal rotation:    0 0 1 0

Portal location:    -0.375654 0.433278 -0.309944
Portal rotation:    0 0 1 0
```
Some notes:
- The key names aren't case-sensitive, and the whitespace doesn't have to be
  aligned or even - it's all read in with a relatively crude `sscanf` loop.
  This makes it a little fragile, but it suffices for its simple purpose, which
  is allowing portal parameter changes without recompiling.
- `Portals radius` is the radius of the spherical input portals, and `Portals
  scale` is the ratio between the output portal size and the input portal size.
- The `Portals radius` and `Portals scale` parameters apply to all portals - so
  this code supports portals only all of the same size and scale.  The code
  also hard-codes the "output" portal (shown with blue outline in paper) to be
  centered at the origin. Neither of these are theoretical limits, and it
  should be pretty straightforward to change the code to support changing these
  if you so desire.
- Individual portals are specified with a location and rotation, with location
  always coming first. The location is `X Y Z`, and the rotation is an
  angle-axis `theta X Y Z`.

### Invocations
The following documentation is also produced when running the executables in `./bin/` with no arguments after compilation, but they're reproduced here for convenience:
```
> ./bin/run
USAGE:
To create a self-similar Julia set from a distance field and portal description
file:
    ./bin/run <SDF *.f3d> <portals *.txt> <versor octaves> <versor scale> <output resolution> <alpha> <beta> <offset x> <offset y> <offset z> <output *.obj> <optional: octree specifier string>

This will generate a shape modulus Julia set using the SDF that you provide and
Perlin noise for the versor field. Alpha is a parameter which controls the
thickness of the shell in which the chaotic effect has significant influence on
set membership, and beta is a parameter which controls the position along the
SDF where the shell appears.

The offset X, Y, and Z parameters move the origin of the dynamical system
around in space, which causes the Julia set to dissolve in interesting ways.
The octree specifier string is an optional parameter useful for computing large
Julia sets in parallel. You can select a box in an evenly-subdivided octree of
arbitrary depth specified by a string of digits 0-7, laid out as follows:
                 +---+
               / |4|5|
              /  +-+-+
             /   |7|6|
            /    +---+
           +---+    /
           |0|1|   /
           +-+-+  /
           |3|2| /
        ▲  +---+
        |
        Y X--▶
        Z ●
        (into page)
Each character of the string will go one level deeper, so the string '5555'
specifies the 1/16-edge length box at the far back corner.
```

```
> ./bin/prun
USAGE:
To create a self-similar Julia set from a distance field:
 ./bin/prun <SDF *.f3d> <portals *.txt> <versor octaves> <versor scale> <output resolution> <a> <b> <offset x> <offset y> <offset z> <output *.obj>

This will pass all the parameters along to ./bin/run and run it in 8X parallel.

NOTE: This script depends on two programs being available in your $PATH:
    - mesh_cat, from trimesh2 (https://gfx.cs.princeton.edu/proj/trimesh2/) to
      stitch the meshes together
    - an xargs implementation that supports the -P flag (e.g. from findutils,
      most implementations support this)

Optionally, you can insert two directives before the rest of the parameters:
    - 'KEEP' with no arguments, e.g. './bin/prun KEEP <sdf.f3d> ...'
    - 'SUB' with one argument, e.g. './bin/prun SUB 001 <sdf.f3d> ...'

KEEP will still stitch the meshes together that each job produced, but it will
not clean them up afterwards, so you'll end up with your output.obj as well as
e.g. output.1.obj, output.2.obj, etc.

SUB will run as normal, but it will confine the whole process to an octree
sub-section. So normally, it's computing the entire mesh in 8x parallel, but if
you run it with SUB 1 it'll compute just octree node #1 in 8x parallel. These
nest as with ./bin/run, see the usage notes for that executable for a
description of the octree layout and labeling. This option does support
multiple nesting, so SUB 123 will compute a 1/128th size region.

An important note is that if you want to use both of these directives, they
must be used in the order KEEP SUB, e.g. './bin/run KEEP SUB 123 <sdf.f3d> ...'
```

```
> ./bin/sdfGen
USAGE:
To generate an SDF from a mesh with automatically generated bounds:
 ./bin/sdfGen <*.obj input> <resolution> <*.f3d output> <padding cells>
To get the bounds for a mesh sequence:
 ./bin/sdfGen BOUNDS <obj 1> <obj 2> ... <obj N>
To generate an SDF from a mesh with specified bounds:
 ./bin/sdfGen <*.obj input> <resolution> <*.f3d output> <min X> <min Y> <min Z> <max X> <max Y> <max Z>
```

