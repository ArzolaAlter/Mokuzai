#ifndef SHAPES_H
#define SHAPES_H
#include <vector>
#include <tuple>

const float CIRCLE_SIDES = 32.0;

using namespace std;

class Shapes
{
    public:
        enum RenderTarget { Tree = 0, Leaves = 1, Extras = 2};
        enum CylinderSideUVs { Wrapped, Repeat };

        // Draw
        static tuple< vector< vector<float> >, vector< vector<float> > > DrawFilledCircle(vector<float>, float, float = CIRCLE_SIDES);
        static tuple< vector< vector<float> >, vector< vector<float> > > DrawAngledFilledCircle(vector<float>, float, float, float = CIRCLE_SIDES);
        static void DrawCylinder(vector<float>, float, vector<float>, float, CylinderSideUVs, float = CIRCLE_SIDES, float = 1.0);
        static void DrawWarpedCylinder(vector<float>, float, float, vector<float>, float, float, CylinderSideUVs, float = CIRCLE_SIDES, float = 1.0);
        static void DrawCone(vector<float>, float, vector<float>, float = CIRCLE_SIDES);
        static void DrawPolygon(vector< vector<float> >, vector< vector<float> >);
        static void DrawTriangleFan(vector< vector<float> >, vector< vector<float> >);
        static void DrawQuadStrip(vector< vector<float> >, vector< vector<float> >);
        static void DrawLine(vector<float>, vector<float>);
        static void DrawLeafPlane(vector<float>, float, float, float);
        static void DrawLeaf(vector<float>, float, float, float);
        static void DrawFoldedLeaf(vector<float>, float, float, float);
        static void DrawCircleLeaf(vector<float>, float, float, float);

        // Loop
        static void Start();
        static void EarlyUpdate();
        static void SetRecordTarget(RenderTarget);
        static void LateUpdate();
        static void SaveModel();
    protected:

    private:
};

#endif // SHAPES_H
