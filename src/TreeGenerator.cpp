#include "TreeGenerator.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <Images.h>
#include <Shapes.h>
#include <vector>
#include <Options.h>
#include <Random.h>
#include <Utilities.h>
#include <math.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tuple>

using namespace std;
using namespace glm;

//------------------------------------------------------------------------------------//
/*----------------------------------- FIELDS -----------------------------------------*/
//------------------------------------------------------------------------------------//

static vector<GLuint> treeTextures;
static vector<GLuint> leavesTextures;
static vector<GLuint> groundTextures;

//------------------------------------------------------------------------------------//
/*---------------------------------- METHODS -----------------------------------------*/
//------------------------------------------------------------------------------------//

static vector<int> GenerateLeafCount(int level, int segmentCount)
{
    if (level < Options::GetMinLeafLevel()) { return vector<int>(); }
    int leafCount = Options::GetLeafCount();

    vector<int> result;
    for (int i = 0; i < segmentCount; i++) { result.push_back(0); }
    float segmentSize = 1.0f / float(segmentCount);
    for (int i = 0; i < leafCount; i++)
    {
        float position = Random::NextRandom();
        int index = static_cast <int> (floor(position / segmentSize));
        result[index] = result[index]+1;
    }
    return result;
}

static void DrawSegment(int level, int segmentNumber, float segmentSize, float bottomRadius, float tipRadius, vector<int> leafCount)
{
    Random::SaveSeeds();
    // Segment Number 0 = Base, maxSegment-1 = Tip
    int totalSegments = Options::GetBranchSubdivisions(level);
    float minProgress = segmentNumber / float(totalSegments);
    float maxProgress = minProgress + (1.0f / float(totalSegments));

    float bottom = bottomRadius * (1.0f - minProgress) + tipRadius * minProgress;
    float top = bottomRadius * (1.0f - maxProgress) + tipRadius * maxProgress;

    //DrawRotatedTipCylinder
    if (Options::GetRenderWireframe())
    {
        Shapes::DrawLine({0,0,0}, {0, segmentSize, 0});
        Random::RestoreSeeds();
        return;
    }

    if (level >= Options::GetMinLeafLevel())
    {
        int segmentLeaves = leafCount[segmentNumber];

            Shapes::DrawCylinder({0,segmentNumber == 0 ? 0 : -segmentSize*0.065,0}, bottom, {0,segmentSize,0}, top, Shapes::Wrapped, Options::GetBranchQuality());


            Images::LoadTextureEnd(); // "Close" Drawing The Branches
            Shapes::SetRecordTarget(Shapes::Leaves);
            Images::LoadTextureStart(leavesTextures[Options::GetLeavesTexture()], true);

        // START DRAW LEAVES
            for (int i = 0; i < segmentLeaves; i++)
            {
                float h = Options::GetLeafSize();
                float w = h * Options::GetLeafSizeX();
                float bend = Options::GetLeafBend();

                float y = Random::NextRandom();
                float radiusAtHeight = bottom * (1.0f - y) + top * y;
                y *= segmentSize;
                float angle = 360.0f * Random::NextRandom() *(atan(1)*4/180); // Radians
                float tetha = i;
                float factor = cos(angle) > 0 ? 1 : -1;
                float z = (cos(angle) * radiusAtHeight) + factor * Options::GetLeafOffset();
                float x = sin(angle) * radiusAtHeight;

                switch(Options::GetLeafType())
                {
                    case 1: // Plane
                        Shapes::DrawLeafPlane({x,y,z}, w, h, bend);
                        break;
                    case 2: // Droplet
                        Shapes::DrawLeaf({x,y,z}, w, h, bend);
                        break;
                    case 3: // Crooked
                        Shapes::DrawFoldedLeaf({x,y,z}, w, h, bend);
                        break;
                    case 4: // Circle
                        Shapes::DrawCircleLeaf({x,y,z}, w, h, bend);
                        break;
                }

            }
        // END DRAW LEAVES
        Images::LoadTextureEnd();

        Shapes::SetRecordTarget(Shapes::Tree);
        Images::LoadTextureStart(treeTextures[Options::GetTreeTexture()], true);
        Random::RestoreSeeds();
        return;
    }

    Shapes::DrawCylinder({0,segmentNumber == 0 ? 0 : -segmentSize*0.065,0}, bottom, {0,segmentSize,0}, top, Shapes::Wrapped, Options::GetBranchQuality());
    Random::RestoreSeeds();
}

static void DrawCurvedBranch(int level, int segmentNumber, float bottomRadius, float tipRadius, float anglePerSegment, float branchSize, float branchSplitAngle,
                              vector<int> leafCount)
{
    int segmentCount = Options::GetBranchSubdivisions(level);
    int localSegment = segmentCount - segmentNumber;
    if (segmentNumber <= 0|| level > Options::GetMaxLevels()) { return; }

    float segmentSize = branchSize / float(segmentCount);

    float zAngle = anglePerSegment;
    glRotatef(zAngle, 0, 0, 1);
    glPushMatrix();
        glTranslatef(0,segmentSize, 0);
        DrawCurvedBranch(level, segmentNumber-1, bottomRadius, tipRadius, anglePerSegment, branchSize, branchSplitAngle, leafCount);
    glPopMatrix();

    // Draw Branch Segment
    DrawSegment(level, localSegment, segmentSize, bottomRadius, tipRadius, leafCount);
}

static void DrawCurvedBranch(int level, float bottomRadius)
{
    int segmentCount = Options::GetBranchSubdivisions(level);
    float tipRadiusRatio = Options::GetBranchTipRadius(level);
    float anglePerSegment = Options::GetBranchCurvature(level) / ((float)segmentCount);
    float branchSize = Options::GetBranchLength(level);
    float branchSplitAngle = Options::GetBranchSplitAngle(level);

    float segmentSize = branchSize / float(segmentCount);
    float zAngle = anglePerSegment + branchSplitAngle;

    glRotatef(zAngle, 0, 0, 1);
    glPushMatrix();
            DrawCurvedBranch(level, segmentCount, bottomRadius, bottomRadius*tipRadiusRatio, anglePerSegment, branchSize, branchSplitAngle, GenerateLeafCount(level, segmentCount));
    glPopMatrix();

}

static void DrawCurvedBranchWithSubBranches(int, float);

static void DrawSegmentWithPossibleBranch(int level, int segmentNumber, float segmentSize, vector< vector<float> > branchHeights, int subBranchCount,
                                          float bottomRadius, float tipRadius, float branchCurvature, float branchSplitAngle, float branchSeparation, vector<int> leafCount)
{
    int segmentCount = Options::GetBranchSubdivisions(level);
    int maxLevels = Options::GetMaxLevels();

    int localSegment = segmentCount - segmentNumber;
    if (segmentNumber <= 0 || level > maxLevels) { return; }

    float anglePerSegment = branchCurvature / ((float)segmentCount);

    float zAngle = anglePerSegment;

    glRotatef(zAngle, 0, 0, 1);
    glPushMatrix();
        glTranslatef(0,segmentSize, 0);
        DrawSegmentWithPossibleBranch(level, segmentNumber-1, segmentSize, branchHeights, subBranchCount + branchHeights[localSegment].size(),
                  bottomRadius, tipRadius, branchCurvature, branchSplitAngle, branchSeparation, leafCount);
    glPopMatrix();

    // Draw Segment
    DrawSegment(level, localSegment, segmentSize, bottomRadius, tipRadius, leafCount);

    // Draw Sub Branches
    int totalBranches = branchHeights[localSegment].size();

    for (int i = 0; i < totalBranches; i++)
    {
        float nextHeight = branchHeights[localSegment][i];
        float realHeight = ((float(localSegment) / float(segmentCount)) + (nextHeight / float(segmentCount) ));
        float baseSize = ((bottomRadius) * (1.0f - realHeight)) + tipRadius * realHeight;
        if (level == 0 && maxLevels == 0) { return; }
        glPushMatrix();
            glRotatef(branchSeparation*float(subBranchCount + i + 1), 0, 1, 0);
            glTranslatef(0, segmentSize * nextHeight,0);
            if (level +1 == maxLevels)
            {
                DrawCurvedBranch(level+1, baseSize);
            }else
            {
                DrawCurvedBranchWithSubBranches(level+1, baseSize);
            }
        glPopMatrix();
    }
}

static void DrawCurvedBranchWithSubBranches(int level, float bottomRadius)
{
    int segmentCount = Options::GetBranchSubdivisions(level);
    float segmentSize = Options::GetBranchLength(level) / float(segmentCount);
    vector< vector<float> > branchHeights;
    for (int i = 0; i < segmentCount; i++)
    {
        branchHeights.push_back(vector<float>());
    }

    int subBranchCount = Options::GetBranchCount(level);
    float initialUnusedSpace = Options::GetInitialUnusedSpace(level);
    float progressPerSegment = 1.0f / float(segmentCount);
    float progressPerPick = (1.0f - initialUnusedSpace) / float(subBranchCount+1);
    float bottomThreshold = initialUnusedSpace;
    float nextBranchPosition = bottomThreshold + Random::NextRandom() * progressPerPick;

    int currentSection = 0;
    for (int i = 0; i < subBranchCount; i++)
    {
        while (float(currentSection+1) * progressPerSegment < nextBranchPosition) { currentSection++; }
        float minSectionProgress = float(currentSection) * progressPerSegment;
        branchHeights[currentSection].push_back( (nextBranchPosition - minSectionProgress) / progressPerSegment );
        bottomThreshold += progressPerPick;
        nextBranchPosition = bottomThreshold + Random::NextRandom() * progressPerPick;
    }

    float tipRadiusRatio = Options::GetBranchTipRadius(level);
    float branchCurvature = Options::GetBranchCurvature(level);
    float branchSplitAngle = Options::GetBranchSplitAngle(level);
    float branchSeparation = Options::GetBranchSeparation(level);

    float anglePerSegment = branchCurvature / ((float)segmentCount);
    float zAngle = anglePerSegment + branchSplitAngle;

    glRotatef(zAngle, 0, 0, 1);
    glPushMatrix();
        DrawSegmentWithPossibleBranch(level, segmentCount, segmentSize, branchHeights, subBranchCount, bottomRadius, bottomRadius*tipRadiusRatio, branchCurvature,
                                      branchSplitAngle, branchSeparation, GenerateLeafCount(level, segmentCount));
    glPopMatrix();
}

static void GenerateTree()
{
    if (Options::GetMaxLevels() == 0)
    {
        DrawCurvedBranch(0, Options::GetBranchBaseRadius());
        return;
    }
    DrawCurvedBranchWithSubBranches(0, Options::GetBranchBaseRadius());
}

void TreeGenerator::DrawTree()
{
    float yOffset = -1.0;
    Shapes::SetRecordTarget(Shapes::Extras);
    Images::LoadTextureStart(groundTextures[Options::GetGroundTexture()], true);
    // START DRAW GROUND
    glPushMatrix();
        glTranslatef(0.0,yOffset,0.0);
        Shapes::DrawCylinder({0.0,-0.1,0.0}, 1.55f, {0.0,0.1,0.0}, 1.3f, Shapes::Repeat, 12);
        Shapes::DrawCylinder({0.0,-0.5,0.0}, 0.45f, {0.0,-0.1,0.0}, 1.55f, Shapes::Repeat, 12);
    glPopMatrix();
    // END DRAW GROUND
    Images::LoadTextureEnd();

    Shapes::SetRecordTarget(Shapes::Tree);
    Images::LoadTextureStart(treeTextures[Options::GetTreeTexture()], true);
    // START DRAW TREE STUMP AND BRANCHES
    glPushMatrix();
        glTranslatef(0.0,yOffset,0.0);
        glPushMatrix();
            GenerateTree();
        glPopMatrix();
    glPopMatrix();
    // END DRAW TREE STUMP AND BRANCHES
    Images::LoadTextureEnd();


}

void TreeGenerator::Start()
{
    treeTextures =
    {
        Images::GetImageTexture(Utilities::FilePath("textures\\tree1.jpg")),
        Images::GetImageTexture(Utilities::FilePath("textures\\tree2.jpg")),
        Images::GetImageTexture(Utilities::FilePath("textures\\tree3.jpg"))
    };

    leavesTextures =
    {
        Images::GetImageTexture(Utilities::FilePath("textures\\leaves1.jpg")),
        Images::GetImageTexture(Utilities::FilePath("textures\\leaves2.jpg")),
        Images::GetImageTexture(Utilities::FilePath("textures\\leaves3.jpg"))
    };

    groundTextures =
    {
        Images::GetImageTexture(Utilities::FilePath("textures\\ground1.jpg")),
        Images::GetImageTexture(Utilities::FilePath("textures\\ground2.jpg")),
        Images::GetImageTexture(Utilities::FilePath("textures\\ground3.jpg"))
    };
}
