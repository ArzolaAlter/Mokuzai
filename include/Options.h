#ifndef OPTIONS_H
#define OPTIONS_H

#include <vector>

using namespace std;

class Options
{
    public:
        // Settings Getters Setters
        static int GetSession();
        static int GetSeed();
        static int GetBranchQuality();
        static bool GetRenderWireframe();
        static void SetSeed(int);
        static int GetTreeTexture();
        static int GetLeavesTexture();
        static int GetGroundTexture();

        // Leaf Getters
        static int GetLeafType();
        static int GetMinLeafLevel();
        static int GetLeafCount();
        static float GetLeafSize();
        static float GetLeafSizeX();
        static float GetLeafBend();
        static float GetLeafOffset();
        static int GetLeafZoom();

        // Tree Generation Getters
        // General
        static int GetMaxLevels();
        // Branches - Size
        static float GetBranchLength(int);
        static float GetBranchBaseRadius();
        static float GetBranchTipRadius(int);
        // Branches - Branching
        static float GetInitialUnusedSpace(int);
        static int GetBranchCount(int);
        static int GetBranchSubdivisions(int);
        // Branches - Curvature
        static float GetBranchSplitAngle(int);
        static float GetBranchCurvature(int);
        static float GetBranchSeparation(int);
        static float GetSubdivisionFirstHalfRotation(int);
        static float GetSubdivisionSecondHalfRotation(int);

        // Loop
        static void Start();
        static void Update();

        // Events
        static void PressNormalKey(unsigned char, int, int);
        static void ReleaseNormalKey(unsigned char, int, int);
        static void PressSpecialKey(int, int, int);
        static void ReleaseSpecialKey(int, int, int);
        static void OnResize(int,int);
        static void MouseClick(int, int, int, int);

    protected:

    private:
};

#endif // OPTIONS_H
