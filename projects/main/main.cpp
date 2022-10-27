#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdio>
#include <limits>
#include <stdio.h>
#include <cmath>
#include <random>

#include "SETTINGS.h"

#include "MC.h"
#include "mesh.h"
#include "field.h"
#include "Quaternion/QUATERNION.h"
#include "Quaternion/POLYNOMIAL_4D.h"

#include "julia.h"

#include <chrono>
#include <thread>

using namespace std;

POLYNOMIAL_4D randPolynomialInBox(AABB box, Real minPower, Real maxPower, uint nRoots, bool allowNonIntegerPowers = true) {
    random_device rd;
    default_random_engine eng(rd());

    uniform_real_distribution<> spaceXD(box.min()[0], box.max()[0]);
    uniform_real_distribution<> spaceYD(box.min()[1], box.max()[1]);
    uniform_real_distribution<> spaceZD(box.min()[2], box.max()[2]);

    uniform_real_distribution<> powerD(minPower, maxPower);

    vector<QUATERNION> roots{};
    vector<Real> powers{};
    for (uint i = 0; i < nRoots; ++i) {
        QUATERNION rootPos = QUATERNION(spaceXD(eng), spaceYD(eng), spaceZD(eng), 0);
        roots.push_back(rootPos);
        powers.push_back( allowNonIntegerPowers? powerD(eng) : int(powerD(eng)) );
    }

    POLYNOMIAL_4D poly(roots, powers);
    return poly;
}

POLYNOMIAL_4D randPolynomialInObject(Grid3D* sdf, Real minPower, Real maxPower, uint nRoots, bool allowNonIntegerPowers = true) {
    random_device rd;
    default_random_engine eng(rd());

    Real minX = 0, maxX = 1, minY = 0, maxY = 1, minZ = 0, maxZ = 1;
    if (sdf->hasMapBox) {
        minX = sdf->mapBox.min()[0];
        minY = sdf->mapBox.min()[1];
        minZ = sdf->mapBox.min()[2];

        maxX = sdf->mapBox.max()[0];
        maxY = sdf->mapBox.max()[1];
        maxZ = sdf->mapBox.max()[2];
    }

    uniform_real_distribution<> spaceXD(minX, maxX);
    uniform_real_distribution<> spaceYD(minY, maxY);
    uniform_real_distribution<> spaceZD(minZ, maxZ);

    uniform_real_distribution<> powerD(minPower, maxPower);

    vector<QUATERNION> roots{};
    vector<Real> powers{};
    for (uint i = 0; i < nRoots; ++i) {
        QUATERNION rootPos;
        bool insideTarget = false;
        while (not insideTarget) {
            rootPos = QUATERNION(spaceXD(eng), spaceYD(eng), spaceZD(eng), 0);
            insideTarget = (*sdf).getFieldValue(VEC3F(rootPos[0], rootPos[1], rootPos[2])) < 0;
        }
        roots.push_back(rootPos);
        powers.push_back( allowNonIntegerPowers? powerD(eng) : int(powerD(eng)) );
    }

    POLYNOMIAL_4D poly(roots, powers);
    return poly;
}

POLYNOMIAL_4D randPolynomialNearSurface(Grid3D* sdf, Real minPower, Real maxPower, uint nRoots, Real maxDist, bool allowNonIntegerPowers = true) {
    random_device rd;
    default_random_engine eng(rd());

    Real minX = 0, maxX = 1, minY = 0, maxY = 1, minZ = 0, maxZ = 1;
    if (sdf->hasMapBox) {
        minX = sdf->mapBox.min()[0];
        minY = sdf->mapBox.min()[1];
        minZ = sdf->mapBox.min()[2];

        maxX = sdf->mapBox.max()[0];
        maxY = sdf->mapBox.max()[1];
        maxZ = sdf->mapBox.max()[2];
    }

    uniform_real_distribution<> spaceXD(minX, maxX);
    uniform_real_distribution<> spaceYD(minY, maxY);
    uniform_real_distribution<> spaceZD(minZ, maxZ);

    uniform_real_distribution<> powerD(minPower, maxPower);

    vector<QUATERNION> roots{};
    vector<Real> powers{};
    for (uint i = 0; i < nRoots; ++i) {
        QUATERNION rootPos;
        bool insideTarget = false;
        while (not insideTarget) {
            rootPos = QUATERNION(spaceXD(eng), spaceYD(eng), spaceZD(eng), 0);
            insideTarget = abs((*sdf).getFieldValue(VEC3F(rootPos[0], rootPos[1], rootPos[2]))) < maxDist;
        }
        roots.push_back(rootPos);
        powers.push_back( allowNonIntegerPowers? powerD(eng) : int(powerD(eng)) );
    }

    POLYNOMIAL_4D poly(roots, powers);
    return poly;
}

POLYNOMIAL_4D randPolynomialNearSurfaceBlueNoise(Grid3D* sdf, Real minPower, Real maxPower, uint nRoots, Real maxDist, bool allowNonIntegerPowers = true) {
    random_device rd;
    default_random_engine eng(rd());

    Real minX = 0, maxX = 1, minY = 0, maxY = 1, minZ = 0, maxZ = 1;
    if (sdf->hasMapBox) {
        minX = sdf->mapBox.min()[0];
        minY = sdf->mapBox.min()[1];
        minZ = sdf->mapBox.min()[2];

        maxX = sdf->mapBox.max()[0];
        maxY = sdf->mapBox.max()[1];
        maxZ = sdf->mapBox.max()[2];
    }

    uniform_real_distribution<> spaceXD(minX, maxX);
    uniform_real_distribution<> spaceYD(minY, maxY);
    uniform_real_distribution<> spaceZD(minZ, maxZ);

    uniform_real_distribution<> powerD(minPower, maxPower);

    vector<QUATERNION> roots{};
    vector<Real> powers{};
    for (uint i = 0; i < nRoots; ++i) {
        QUATERNION rootPos;
        bool insideTarget = false;
        while (not insideTarget) {
            rootPos = QUATERNION(spaceXD(eng), spaceYD(eng), spaceZD(eng), 0);
            insideTarget = abs((*sdf).getFieldValue(VEC3F(rootPos[0], rootPos[1], rootPos[2]))) < maxDist;
        }
        roots.push_back(rootPos);
        powers.push_back( allowNonIntegerPowers? powerD(eng) : int(powerD(eng)) );
    }

    POLYNOMIAL_4D poly(roots, powers);
    return poly;
}

void dumpRotInfo(QuatToQuatFn* p, AABB range, int res) {
    // This is hacky, I'm using three scalar fields where
    // I really should just make a vector field class.
    QuatQuatRotField r(p, 0);
    QuatQuatRotField g(p, 1);
    QuatQuatRotField b(p, 2);
    QuatQuatMagField mag(p);

    VirtualGrid3D gr(res, res, res, range.min(), range.max(), &r);
    gr.writeF3D("temp/r.f3d", true);

    VirtualGrid3D gg(res, res, res, range.min(), range.max(), &g);
    gg.writeF3D("temp/g.f3d", true);

    VirtualGrid3D gb(res, res, res, range.min(), range.max(), &b);
    gb.writeF3D("temp/b.f3d", true);

    VirtualGrid3D gm(res, res, res, range.min(), range.max(), &mag);
    gm.writeF3D("temp/mag.f3d", true);
}

void loadPointWeightsFromCSVAndLiftInto3D(ArrayGrid3D &grid, Grid3D* distField, string filename) {
    ifstream in;
    string line;
    in.open(filename);

    for (uint i = 0; i < grid.totalCells(); ++i) {
        grid[i] = -1;
    }

    vector<VEC3F> points;
    vector<Real>  weights;
    AABB meshBox{};// = AABB::insideOut();

    while( !in.eof() ) {
        in >> line;
        Real x, y, z, weight;
        sscanf(line.c_str(), "%lf,%lf,%lf,%lf", &x, &y, &z, &weight);

        // XXX For now we get the data as 1 = normal, 0 = fractal
        // XXX But we want 0 = normal, 1 = fractal
        weight = 1 - weight;

        VEC3F v(x,y,z);
        meshBox.include(v);

        if (weight != 0) {
            points.push_back(v);
            weights.push_back(weight);
        }

    }

    grid.setMapBox(meshBox);
    PRINTF("Got %lu point weights\n", weights.size());
    PRINTV3(meshBox.min());
    PRINTV3(meshBox.max());
    PRINTV3(meshBox.span());

    AABB meshTargetBox(VEC3F(-0.165,-0.165,-0.165), VEC3F(0.165,0.165,0.165));

    grid.setMapBox(distField->mapBox);
    for (uint i=0; i < points.size(); ++i) {
        points[i] = AABB::transferPoint(points[i], meshBox, meshTargetBox);
    }

    PRINTV3(grid.mapBox.min());
    PRINTV3(grid.mapBox.max());
    PRINTV3(meshTargetBox.min());
    PRINTV3(meshTargetBox.max());

    PB_START("Propogate surface weights to 3D...");

    Real propogation_distance = 0.01;
    Real pd_squared = propogation_distance * propogation_distance;

    for (uint i = 0; i < grid.xRes; ++i) {
#pragma omp parallel for
        for (uint j = 0; j < grid.yRes; ++j) {
            for (uint k = 0; k < grid.yRes; ++k) {
                VEC3F cellCenter = grid.getCellCenter(VEC3I(i, j, k));

                bool inStroke = false;
                for (VEC3F pt : points) {
                    if ((pt - cellCenter).squaredNorm() < pd_squared) {
                        inStroke = true;
                        break;
                    }
                }

                grid(cellCenter) = inStroke ? 7.00 : 500.00;

            }
        }
        PB_PROGRESS((Real) i / grid.xRes);
    }

    PB_END();

}

void loadPointWeightsFromCSVAndLiftInto3DSmooth(ArrayGrid3D &grid, Grid3D* distField, string filename) {
    ifstream in;
    string line;
    in.open(filename);

    for (uint i = 0; i < grid.totalCells(); ++i) {
        grid[i] = -1;
    }

    vector<VEC3F> points;
    vector<Real>  weights;
    AABB meshBox{};// = AABB::insideOut();

    while( !in.eof() ) {
        in >> line;
        Real x, y, z, weight;
        sscanf(line.c_str(), "%lf,%lf,%lf,%lf", &x, &y, &z, &weight);

        // XXX For now we get the data as 1 = normal, 0 = fractal
        // XXX But we want 0 = normal, 1 = fractal
        weight = 1 - weight;

        VEC3F v(x,y,z);
        meshBox.include(v);

        if (weight != 0) {
            points.push_back(v);
            weights.push_back(weight);
        }

    }


    grid.setMapBox(meshBox);
    PRINTF("Got %lu point weights\n", weights.size());
    PRINTV3(meshBox.min());
    PRINTV3(meshBox.max());
    PRINTV3(meshBox.span());

    for (uint i=0; i < points.size(); ++i) {
        grid(points[i]) = weights[i];
    }

    grid.setMapBox(distField->mapBox);
    for (uint i=0; i < points.size(); ++i) {
        points[i] = AABB::transferPoint(points[i], meshBox, distField->mapBox);
    }

    grid.writeF3D("populated.f3d", true);

    // For now we'll assume that a field value of 1 means
    // a high A, zero means a lower A, and all -1s must be
    // eliminated.


    PB_START("Propogate surface weights to 3D...");

    Real propogation_distance = 0.005;

    for (uint i = 0; i < grid.xRes; ++i) {
#pragma omp parallel for
        for (uint j = 0; j < grid.yRes; ++j) {
            for (uint k = 0; k < grid.yRes; ++k) {
                VEC3F cellCenter = grid.getCellCenter(VEC3I(i, j, k));

                if (grid.at(i,j,k) == -1) {

                    Real minDistVal = 0;
                    Real minDist = numeric_limits<Real>::max();

                    for (uint i=0; i < points.size(); ++i) {
                        Real dist = (points[i] - cellCenter).squaredNorm();
                        if (dist < minDist) {
                            minDist = dist;
                            minDistVal = weights[i] * max(0.0, 1 - (dist / propogation_distance));
                        }
                    }

                    grid.at(i,j,k) = minDistVal;
                }

            }
        }
        PB_PROGRESS((Real) i / grid.xRes);
    }

    PB_END();

    grid.writeF3D("propogated.f3d", true);


}


int main(int argc, char *argv[]) {

    if(argc != 10 && argc != 11) {
        cout << "USAGE: " << endl;
        cout << "To create a shaped julia set from a distance field:" << endl;
        cout << " " << argv[0] << " <SDF *.f3d> <P(q) *.poly4d, or the word RANDOM> <output resolution> <a> <b> <offset x> <offset y> <offset z> <output *.obj> <optional: octree specifier string>" << endl << endl;

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

    // Save radius-membership relationship
    // sampleSphereIntersection(&distField, 0, 0.6, 2500, 100000, "inside.csv");
    // exit(0);

    PRINT("NOTE: Setting simulation bounds to hard-coded values (not from distance field)");
    distField.mapBox.min() = VEC3F(-0.5, -0.5, -0.5);
    distField.mapBox.max() = VEC3F(0.5, 0.5, 0.5);

    RationalQuatPoly *p;

    if (string(argv[2]) == "RANDOM") {
        // Create random polynomial

        // POLYNOMIAL_4D polyTop = randPolynomialInBox(0.7, 8, 13, 25, false);
        // POLYNOMIAL_4D polyTop = randPolynomialInObject(&distField, 2, 13, 10, false);
        PRINT("Generating random polynomial...");
        POLYNOMIAL_4D polyTop = randPolynomialNearSurface(&distField, 2, 13, 10, 0.001, false);

        p = new RationalQuatPoly(polyTop);
    } else {
        PRINT("Loading polynomial from file...");
        POLYNOMIAL_4D polyTop(argv[2]);
        p = new RationalQuatPoly(polyTop);
    }

    // Save polynomial info for inspection later
    FILE* out = fopen("temp/p.poly4d", "w");
    p->topPolynomial.write(out);
    PRINT("Wrote polynomial to temp/p.poly4d");
    fclose(out);

    // dumpRotInfo(&p, distField.mapBox, 100);

    // Finally we actually compute the Julia set
    Real fillLevel = atof(argv[4]); // This is param a in the equation
    Real fillOffset = atof(argv[5]); // This is param b in the equation

    // Offset roots and distance field to reproduce QUIJIBO dissolution
    // effect
    VEC3F offset3D(atof(argv[6]), atof(argv[7]), atof(argv[8]));

    distField.mapBox.setCenter(offset3D);
    for (unsigned int i = 0; i < p->topPolynomial.roots().size(); i++)
        p->topPolynomial.rootsMutable()[i] = p->topPolynomial.roots()[i] + offset3D;

    int res = atoi(argv[3]);

    // Set up simulation bounds, taking octree zoom into account
    AABB boundsBox(distField.mapBox.min(), distField.mapBox.max());
    if (argc == 11) { // If an octree specifier string was given, we zoom in on just one box
        char* octreeStr = argv[10];

        for (size_t i = 0; i < strlen(octreeStr); ++i) {
            int oIdx = octreeStr[i] - '0';
            if (oIdx < 0 || oIdx > 7) {
                PRINTF("Found character '%c' in octree specifier string. Valid characters are numbers 0-7, inclusive.\n", octreeStr[i]);
                exit(1);
            }

            boundsBox = boundsBox.subdivideOctree()[oIdx];
        }

        // Pad by one grid cell to avoid gaps
        VEC3F delta = boundsBox.span() / res;
        boundsBox.max() += delta;
        boundsBox.min() -= delta;
    }

    PRINTF("Computing Julia set with resolution %d, a=%f, b=%f, offset=(%f, %f, %f)\n", res, fillLevel, fillOffset, offset3D.x(), offset3D.y(), offset3D.z());

    // Make fields for a and b
    ConstantFunction3D b(fillOffset);

    // ConstantFunction3D a(fillLevel);

    // ArrayGrid3D a_fromVerts_coarse(100, 100, 100);
    // loadPointWeightsFromCSVAndLiftInto3D(a_fromVerts_coarse, &distField, "/Users/als/Projects/cpp/DistFieldMap/DistFieldMap/vertices.csv");

    ArrayGrid3D a_fromVerts_coarse("./propogated.f3d");
    InterpolationGrid a_fromVerts_smooth(&a_fromVerts_coarse, InterpolationGrid::LINEAR);

    // VirtualGrid3D(300, 300, 300, &a_fromVerts_smooth).writeF3D("propogated.f3d", true);

    // a_fromVerts_coarse.writeF3D("propogated.f3d", true);
    // exit(1);

    // ArrayGrid3D aCoarse("./propogated.f3d"); // Load in, on scale 0 = no fractal to 1 = lots fractal
    // for (uint i=0; i < aCoarse.totalCells(); ++i) {
    //     aCoarse[i] = -20 + ((1 - aCoarse[i]) * 30);
    // }
    // InterpolationGrid a(&aCoarse, InterpolationGrid::LINEAR);
    // a.setMapBox(distField.mapBox);

    // a.writeF3D("temp/a.f3d");
    // exit(1);


    // Smooth transition across X axis
    FieldFunction3D a_xSmooth
        (
            [](VEC3F pos) {
                Real transitionStart = -0.3;
                Real transitionEnd = 0.3;
                Real levelStart = 10;
                Real levelEnd = 500;

                Real xp = (pos.z() - transitionStart) / (transitionEnd - transitionStart);
                xp = max(0.0, min(1.0, xp));

                // Make cubic
                xp = pow(xp, 3);

                // Real d = (3 * xp * xp) - (2 * xp * xp * xp);
                // return (1 - d)*levelStart + (d * levelEnd);

                // return (pos.z() < 0 ? 30.0: 500.0);

                return (1 - xp)*levelStart + (xp * levelEnd);
            }
        );

    FieldFunction3D a_hardSphere
        (
            [](VEC3F pos) {

                VEC3F center(0,0,0.5);

                if ((pos - center).squaredNorm() < 0.04) {
                    return 7.0;
                }

                return 500.0;

            }
        );

    VirtualGrid3D(100, 100, 100, distField.mapBox.min(), distField.mapBox.max(), &a_xSmooth).writeF3D("temp/a.f3d", true);
    // exit(1);

    DistanceGuidedQuatFn map(&distField, p, &a_xSmooth, &b);
    // DistanceGuidedQuatFn map(&distField, p, &a_hardSphere, &b);
    // DistanceGuidedQuatFn map(&distField, p, &a_fromVerts_smooth, &b);

    // Simple gradient

    // ArrayGrid3D a_grid(100, 100, 100);
    // loadPointWeightsFromCSVAndLiftInto3D(a_grid, &distField, "/Users/als/Projects/cpp/DistFieldMap/DistFieldMap/vertices.csv");
    // exit(1);

    // DistanceGuidedQuatFn map(&distField, p, &a, &b);

    QuaternionJuliaSet julia(&map, 3, 20);

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

    m.writeOBJ(argv[9]);

    PRINTD((Real) vg.numHits / vg.numQueries);

    return 0;
}

