# *Into the Portal: Directable Fractal Self-Similarity *

[![DOI:10.1111/cgf.14905](https://zenodo.org/badge/DOI/10.1145/3641519.3657466.svg)](https://doi.org/10.1145/3641519.3657466)

## Intro
![Four self-similar fractals: a bunny, statue of Hebe, fox's tail, cat](https://github.com/user-attachments/assets/b1f31c93-b81d-4c64-b0b7-183385c6e37c)

This is the code for the paper titled _Into the Portal: Directable Fractal Self-Similarity_, published at SIGGRAPH 2024 by Alexa Schor (me) and [Theodore Kim](https://tkim.graphics).

Download the paper: [PDF (42MiB)](https://alexaschor.com/into-the-portal/files/into_the_portal.pdf), [PDF low-res (5MiB)](https://alexaschor.com/into-the-portal/files/into_the_portal_smaller.pdf)

See the paper's accompanying video here: [YouTube link](https://youtu.be/8X9RlcaklHU)


_Abstract:_ We present a novel, directable method for introducing fractal self-similarity into arbitrary shapes. Our method allows a user to directly specify the locations of self-similarities in a Julia set and is general enough to reproduce other well-known fractals such as the Koch snowflake.

Ours is the first algorithm to enable this level of general artistic control while also maintaining the character of the original fractal shape. We introduce the notion of placing “portals” into the iteration space of a dynamical system, bridging the aesthetics of iterated maps with the fine-grained control of iterated function systems (IFS). Our method is effective in both 2D and 3D.

## This Repo

First of all, thanks for coming to this page! If you have any questions or ideas about the paper or code or any problems with building, running or modifying the code, please open an issue here or email me at `alexaschor@gmail.com` and I'll do my best to help. 

This is the code used to generate the meshes used in most of the paper figures. For an interactive, (mostly) real time 2D version that allows you to play with the parameters, check out https://github.com/alexaschor/JSM2D
