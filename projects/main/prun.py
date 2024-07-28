#!/usr/bin/env python3
import sys
import os

# This is a quick and dirty script that will compute the geometry of a DQM Julia set
# in 8x parallel with seq and xargs -P. It relies on trimesh2's mesh_cat (TODO get the
# source for that building in this project). You should adjust the parallel factor
# from 8 to however many cores you have on your machine.

NUM_PARALLEL_JOBS = 8
keep = False
subsection = ""

if sys.argv[1] == "KEEP":
    keep = True
    sys.argv = [sys.argv[0]] + sys.argv[2:]

if sys.argv[1] == "SUB":
    subsection = sys.argv[2]
    sys.argv = [sys.argv[0]] + sys.argv[3:]

if len(sys.argv) != 11:
    print("USAGE:")
    print("To create a shaped julia set from a distance field:")
    print(f" {sys.argv[0]} <SDF *.f3d> <versor octaves> <versor scale> <output resolution> <a> <b> <offset x> <offset y> <offset z> <output *.obj>")
    print("This will pass all the parameters along to ./bin/run and run it in "+ str(NUM_PARALLEL_JOBS)+"X parallel")
    exit(1)

sdf = sys.argv[1]
vo = sys.argv[2]
vs = sys.argv[3]
out_res = int(sys.argv[4])
a = sys.argv[5]
b = sys.argv[6]
ox = sys.argv[7]
oy = sys.argv[8]
oz = sys.argv[9]
out_obj = sys.argv[10]

params = [sdf, vo, vs, str(int(out_res / 2)), a, b, ox, oy, oz, out_obj]

generate_command = "seq 0 7 | xargs -P" + str(NUM_PARALLEL_JOBS) + " -I{} ./bin/run " + " ".join(params) +  ".{}.obj " + subsection + "{}"
cat_command      = f"mesh_cat {out_obj}.*.obj -o {out_obj}"
rm_command       = f"rm {out_obj}.*.obj" if not keep else "echo 'Keeping objs...'"

command = "time (" +generate_command + " && " + cat_command + " && " + rm_command + ")"
print(command)

os.system(command)
