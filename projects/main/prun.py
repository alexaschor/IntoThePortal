#!/usr/bin/env python3
import sys
import os

# This is a quick and dirty script that will compute the geometry of a DQM Julia set
# in 8x parallel with seq and xargs -P. It relies on trimesh2's mesh_cat (TODO get the
# source for that building in this project). You should adjust the parallel factor
# from 8 to however many cores you have on your machine.


def die_usage():
    print("USAGE:")
    print("To create a shaped julia set from a distance field:")
    print(f" {sys.argv[0]} <SDF *.f3d> <portals *.txt> <versor octaves> <versor scale> <output resolution> <a> <b> <offset x> <offset y> <offset z> <output *.obj>")
    print("")
    print("This will pass all the parameters along to ./bin/run and run it in "+ str(NUM_PARALLEL_JOBS)+"X parallel.")
    print("")
    print("NOTE: This script depends on two programs being available in your $PATH:")
    print("    - mesh_cat, from trimesh2 (https://gfx.cs.princeton.edu/proj/trimesh2/) to stitch the meshes together")
    print("    - an xargs implementation that supports the -P flag (e.g. from findutils, most implementations support this)")
    print("")
    print("Optionally, you can insert two directives before the rest of the parameters:")
    print("    - 'KEEP' with no arguments, e.g. './bin/prun KEEP <sdf.f3d> ...'")
    print("    - 'SUB' with one argument, e.g. './bin/prun SUB 001 <sdf.f3d> ...'")
    print("")
    print("KEEP will still stitch the meshes together that each job produced, but it will not clean them up afterwards, so you'll end up with your output.obj as well as e.g. output.1.obj, output.2.obj, etc.")
    print("")
    print("SUB will run as normal, but it will confine the whole process to an octree sub-section. So normally, it's computing the entire mesh in 8x parallel, but if you run it with SUB 1 it'll compute just octree node #1 in 8x parallel. These nest as with ./bin/run, see the usage notes for that executable for a description of the octree layout and labeling. This option does support multiple nesting, so SUB 123 will compute a 1/128th size region.")
    print("")
    print("An important note is that if you want to use both of these directives, they must be used in the order KEEP SUB, e.g. './bin/run KEEP SUB 123 <sdf.f3d> ...'")
    exit(1)


NUM_PARALLEL_JOBS = 8
keep = False
subsection = ""

if len(sys.argv) < 2:
    die_usage()

if sys.argv[1] == "KEEP":
    keep = True
    sys.argv = [sys.argv[0]] + sys.argv[2:]

if sys.argv[1] == "SUB":
    subsection = sys.argv[2]
    sys.argv = [sys.argv[0]] + sys.argv[3:]

if len(sys.argv) != 12:
    die_usage()

sdf = sys.argv[1]
portals = sys.argv[2]
vo = sys.argv[3]
vs = sys.argv[4]
out_res = int(sys.argv[5])
a = sys.argv[6]
b = sys.argv[7]
ox = sys.argv[8]
oy = sys.argv[9]
oz = sys.argv[10]
out_obj = sys.argv[11]

params = [sdf, portals, vo, vs, str(int(out_res / 2)), a, b, ox, oy, oz, out_obj]

generate_command = "seq 0 7 | xargs -P" + str(NUM_PARALLEL_JOBS) + " -I{} ./bin/run " + " ".join(params) +  ".{}.obj " + subsection + "{}"
cat_command      = f"mesh_cat {out_obj}.*.obj -o {out_obj}"
rm_command       = f"rm {out_obj}.*.obj" if not keep else "echo 'Keeping objs...'"

command = "time (" +generate_command + " && " + cat_command + " && " + rm_command + ")"
print(command)

os.system(command)
