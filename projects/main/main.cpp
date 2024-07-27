#include <iostream>
#include <cstdio>
#include <stdio.h>

#include <sys/stat.h>

#include "SETTINGS.h"

#include "MC.h"
#include "mesh.h"
#include "field.h"
#include "julia.h"


using namespace std;

int main(int argc, char *argv[]) {
    if(argc != 12 && argc != 13) {
        cout << "USAGE: " << endl;
        cout << "To create a shaped julia set from a distance field:" << endl;
        cout << " " << argv[0] << " <SDF *.f3d> <portals *.txt> <versor octaves> <versor scale> <output resolution> <alpha> <beta> <offset x> <offset y> <offset z> <output *.obj> <optional: octree specifier string>" << endl << endl;
        //                            argv[1]        argv[2]        argv[3]          argv[4]        argv[5]        argv[6] argv[7]  argv[8]    argv[9]   argv[10]      argv[11]               argv[12]

        cout << "    This will compute the 3D Julia quaternion Julia set of the function:" << endl;
        cout << "        f(q) = r(q) * d(q)" << endl;
        cout << "    Where:" << endl;
        cout << "        r(q) = e^( a * SDF(q) + b )" << endl;
        cout << "        d(q) = Normalized( P(q) )" << endl << endl;

        cout << "    P(q) is the quaternion polynomial function, which determines the character of the fractal detail." << endl;
        cout << "    a is a parameter which controls the thickness of the shell in which the chaotic effect has significant influence" << endl;
        cout << "    on set membership, and b is a parameter which controls the position along the surface of the SDF of that shell." << endl << endl;

        cout << "    The offset X, Y, and Z parameters move the origin of the dynamical system around in space, which causes" << endl;
        cout << "    the Julia set to dissolve in interesting ways." << endl;

        cout << "    The octree specifier string is an optional parameter useful for computing large Julia sets in parallel." << endl;
        cout << "    You can select a box in an evenly-subdivided octree of arbitrary depth specified by a string of digits 0-7," << endl;
        cout << "    laid out as follows:" << endl;

        cout << "                 +---+" << endl;
        cout << "               / |4|5|" << endl;
        cout << "              /  +-+-+" << endl;
        cout << "             /   |7|6|" << endl;
        cout << "            /    +---+" << endl;
        cout << "           +---+    / " << endl;
        cout << "           |0|1|   /  " << endl;
        cout << "           +-+-+  /   " << endl;
        cout << "           |3|2| /    " << endl;
        cout << "        ▲  +---+      " << endl;
        cout << "        |             " << endl;
        cout << "        Y X--▶        " << endl;
        cout << "        Z ●           " << endl;
        cout << "        (into page)   " << endl;

        cout << "    Each character of the string will go one level deeper, so the string '5555' specifies the 1/16-edge length box at the far back corner." << endl;

        exit(0);
    }

    // Read distfield
    ArrayGrid3D distFieldCoarse(argv[1]);
    PRINTF("Got distance field with res %dx%dx%d\n", distFieldCoarse.xRes, distFieldCoarse.yRes, distFieldCoarse.zRes);

    // Create interpolation grid (smooth it out)
    InterpolationGrid distField(&distFieldCoarse, InterpolationGrid::LINEAR);
    distField.mapBox.setCenter(VEC3F(0,0,0));

    PRINT("NOTE: Setting simulation bounds to hard-coded values (not from distance field)");
    distField.mapBox.min() = VEC3F(-0.5, -0.5, -0.5);
    distField.mapBox.max() = VEC3F(0.5, 0.5, 0.5);

    // Now we actually compute the Julia set
    Real alpha = atof(argv[6]);
    Real beta = atof(argv[7]);

    // Offset roots and distance field to reproduce QUIJIBO dissolution
    // effect - this is optional, and for all our results in the paper was zero.
    VEC3F offset3D(atof(argv[8]), atof(argv[9]), atof(argv[10]));

    distField.mapBox.setCenter(offset3D);

    int res = atoi(argv[5]);

    // Set up simulation bounds, taking octree zoom into account
    AABB boundsBox(distField.mapBox.min(), distField.mapBox.max() + VEC3F(0.25, 0.25, 0.25));
    if (argc == 13) { // If an octree specifier string was given, we zoom in on just one box
        char* octreeStr = argv[12];

        for (size_t i = 0; i < strlen(octreeStr); ++i) {
            int oIdx = octreeStr[i] - '0';
            if (oIdx < 0 || oIdx > 7) {
                PRINTF("Uh-oh: Found character '%c' in octree specifier string. Valid characters are numbers 0-7, inclusive.\n", octreeStr[i]);
                exit(1);
            }

            boundsBox = boundsBox.subdivideOctree()[oIdx];
        }

        // Pad by one grid cell to avoid gaps
        VEC3F delta = boundsBox.span() / res;
        boundsBox.max() += delta;
        boundsBox.min() -= delta;
    }

    int versor_octaves = atoi(argv[3]);
    Real versor_scale   = atof(argv[4]);

    PRINTF("vo=%s; vs=%s\n",argv[3], argv[4]);

    PRINTF("Computing Julia set with resolution %d, a=%f, b=%f, v. octaves=%d, v. scale=%f, offset=(%f, %f, %f)\n", res, alpha, beta, versor_octaves, versor_scale, offset3D.x(), offset3D.y(), offset3D.z());

    NoiseVersor  versor(versor_octaves, versor_scale);
    ShapeModulus modulus(&distField, alpha, beta);

    VersorModulusR3Map vm(&versor, &modulus);
    R3JuliaSet         mask_j(&vm, 4, 10);

    vector<VEC3F> portalCenters;
    vector<AngleAxis<Real>> portalRotations;

    // READ PORTAL FILE
    Real portalRadius;
    Real portalScale;
    VEC3F portalLocation;
    AngleAxis<Real> portalRotation;

    ifstream portalFile(argv[2]);
    if (portalFile.is_open()) {
        string line;
        while (getline(portalFile, line)) {
            if (line.length()) {
                string key = line.substr(0, line.find(":"));
                string value = line.substr(line.find(":")+1, line.length()-1);
                transform(key.begin(), key.end(), key.begin(), ::tolower);
                transform(value.begin(), value.end(), value.begin(), ::tolower);
                if (key == "portals radius") {
                    sscanf(value.c_str(), " %lf", &portalRadius);
                } else if (key == "portals scale") {
                    sscanf(value.c_str(), " %lf", &portalScale);
                } else if (key == "portal location") {
                    Real x,y,z;
                    sscanf(value.c_str(), " %lf %lf %lf", &x, &y, &z);
                    portalLocation = VEC3F(x,y,z);
                } else if (key == "portal rotation") {
                    Real t,x,y,z;
                    sscanf(value.c_str(), " %lf %lf %lf %lf", &t, &x, &y, &z);
                    portalRotation = AngleAxis<Real>(t, VEC3F(x,y,z));

                    portalCenters.push_back(portalLocation);
                    portalRotations.push_back(portalRotation);
                }
            }
        }
    }
    portalFile.close();


    // FOR BUNNY EARS:
    // portalCenters.push_back(VEC3F(-0.175255, 0.441722, 0.015167));
    // portalRotations.push_back(AngleAxis<Real>(0, VEC3F(0,1,0)));
    //
    // portalCenters.push_back(VEC3F(-0.375654, 0.433278, -0.309944));
    // portalRotations.push_back(AngleAxis<Real>(0, VEC3F(0,1,0)));

    PortalMap  pm(&vm, portalCenters, portalRotations, portalRadius, portalScale, &mask_j);

    // FOR HEBE:
    // portalCenters.push_back(VEC3F(0.140000, 0.350699, 0.126944));
    // portalRotations.push_back(AngleAxis<Real>(0, VEC3F(0,1,0)));
    //
    // PortalMap  pm(&vm, portalCenters, portalRotations, 0.25, 5, &mask_j);

    R3JuliaSet julia(&pm, 7, 10);

    VirtualGrid3DLimitedCache vg(res, res, res, boundsBox.min(), boundsBox.max(), &julia);

    Mesh m;
    MC::march_cubes(&vg, m, true);

    // Currently march_cubes doesn't take the grid's mapBox into account; all vertices are
    // placed in [ (0, xRes), (0, yRes), (0, zRes) ] space. TODO fix march_cubes to account for
    // the mapBox, but for now we'll just manually transform it. Normals should be okay as they are.
    for (uint i = 0; i < m.vertices.size(); ++i) {
        VEC3F v = m.vertices[i];
        m.vertices[i] = vg.gridToFieldCoords(v);
    }

    m.writeOBJ(argv[11]);

    return 0;
}

