#include "Shapes.h"
#include <Utilities.h>
#include <Options.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <tuple>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Images.h>
#include <Random.h>
#include <Options.h>
#include <CameraHandler.h>

using namespace glm;
using namespace std;

//------------------------------------------------------------------------------------//
/*----------------------------------- FIELDS -----------------------------------------*/
//------------------------------------------------------------------------------------//

const char* treeModelName = "Tree";
const char* leavesModelName = "Leaves";
const char* extrasModelName = "Extras";

static Shapes::RenderTarget renderTarget;
static enum EExtraAction{ None, AboutToRecord, Recording, SaveToFile } extraAction;

vector< vector<float> > treeVertices;
vector< vector<float> > treeUVs;
vector< vector<int> > treeFaces;

vector< vector<float> > leavesVertices;
vector< vector<float> > leavesUVs;
vector< vector<int> > leavesFaces;

vector< vector<float> > extrasVertices;
vector< vector<float> > extrasUVs;
vector< vector<int> > extrasFaces;

//------------------------------------------------------------------------------------//
/*---------------------------------- METHODS -----------------------------------------*/
//------------------------------------------------------------------------------------//

static void DrawVertex(vector<float> xyz)
{
    // Don't know if later something might want to be done with vertices, so they are created in a wrapper just in case.
    glVertex3f(xyz[0], xyz[1], xyz[2]);
}

static vector< vector<float> > GetTargetVertices()
{
    switch(renderTarget)
    {
        case Shapes::Tree:
            return treeVertices;
        case Shapes::Leaves:
            return leavesVertices;
        case Shapes::Extras:
            return extrasVertices;
        default:
            printf("An error has occurred attempting to retrieve vertices of target type: %d (0: Tree, 1: Leaves, 2: Extras)", renderTarget);
            return vector< vector<float> >();
    }
}

static vector< vector<int> > GetTargetFaces()
{
    switch(renderTarget)
    {
        case Shapes::Tree:
            return treeFaces;
        case Shapes::Leaves:
            return leavesFaces;
        case Shapes::Extras:
            return extrasFaces;
        default:
            printf("An error has occurred attempting to retrieve faces of target type: %d (0: Tree, 1: Leaves, 2: Extras)", renderTarget);
            return vector< vector<int> >();
    }
}

static vector< vector<float> > GetTargetUVs()
{
    switch(renderTarget)
    {
        case Shapes::Tree:
            return treeUVs;
        case Shapes::Leaves:
            return leavesUVs;
        case Shapes::Extras:
            return extrasUVs;
        default:
            printf("An error has occurred attempting to retrieve UVs of target type: %d (0: Tree, 1: Leaves, 2: Extras)", renderTarget);
            return vector< vector<float> >();
    }
}

static void PushVertexToTarget(vector<float> vertex, vector<float> uv)
{
    GLfloat matrix[16];
    glGetFloatv (GL_MODELVIEW_MATRIX, matrix);

    mat4 transformMatrix = make_mat4(matrix);
    vec4 temp = vec4(vertex[0],vertex[1],vertex[2],1);
    temp = transformMatrix * temp;

    vector<float> result = { temp.x, temp.y, temp.z };

    switch(renderTarget)
    {
        case Shapes::Tree:
            treeVertices.push_back(result);
            treeUVs.push_back(uv);
            return;
        case Shapes::Leaves:
            leavesVertices.push_back(result);
            leavesUVs.push_back(uv);
            return;
        case Shapes::Extras:
            extrasVertices.push_back(result);
            extrasUVs.push_back(uv);
            return;
        default:
            printf("An error has occurred attempting to record a vertex of target type: %d (0: Tree, 1: Leaves, 2: Extras)", renderTarget);
            return;
    }
}

static void PushFaceToTarget(vector<int> face)
{
    switch(renderTarget)
    {
        case Shapes::Tree:
            treeFaces.push_back(face);
            return;
        case Shapes::Leaves:
            leavesFaces.push_back(face);
            return;
        case Shapes::Extras:
            extrasFaces.push_back(face);
            return;
        default:
            printf("An error has occurred attempting to record a face of target type: %d (0: Tree, 1: Leaves, 2: Extras)", renderTarget);
            return;
    }
}

void Shapes::DrawPolygon(vector< vector<float> > vertices, vector< vector<float> > uv)
{
    int verticesCount = vertices.size();
    glBegin(GL_POLYGON);
        for (int i = 0; i < verticesCount; i++)
        {
            glNormal3f(0,1,0);
            glTexCoord2f(uv[i][0], uv[i][1]);
            DrawVertex(vertices[i]);
        }
    glEnd();

    // Record If Needed
    if (extraAction != Recording) { return; }

    int total = vertices.size();
    int currentVertexCount = GetTargetVertices().size();

    for (int i = total-1; i >= 0; i--)
    {
        PushVertexToTarget(vertices[i], uv[i]);
    }

    vector<int> currentVertices;
    for (int i = 0; i < total; i++)
    {
        currentVertices.push_back(currentVertexCount+1+i);
    }
    PushFaceToTarget(currentVertices);
}

void Shapes::DrawTriangleFan(vector< vector<float> > vertices, vector< vector<float> > uv)
{
    int verticesCount = vertices.size();
    glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i < verticesCount; i++)
        {
            glTexCoord2f(uv[i][0], uv[i][1]);
            DrawVertex(vertices[i]);
        }
    glEnd();

    // Record If Needed
    if (extraAction != Recording) { return; }

    vector<float> center = vertices[0];
    for (int i = 1; i < verticesCount-1; i++) // Skip the first and last one
    {
        int start = GetTargetVertices().size()+1;
        PushVertexToTarget(center, {0.5, 0.5});
        PushVertexToTarget(vertices[i+1], uv[i+1]);
        PushVertexToTarget(vertices[i], uv[i]);
        PushFaceToTarget({start, start+1, start+2});
    }
}

void Shapes::DrawQuadStrip(vector< vector<float> > vertices, vector< vector<float> > uv)
{
    int verticesCount = vertices.size();
    glBegin(GL_QUAD_STRIP);
        for (int i = 0; i < verticesCount; i++)
        {
            if (i % 2 == 0 && i+3 < verticesCount)
            {
                vec3 normal;
                vec3 C = vec3(vertices[i][0],vertices[i][1],vertices[i][2]);
                vec3 A = vec3(vertices[i+1][0],vertices[i+1][1],vertices[i+1][2]);
                vec3 D = vec3(vertices[i+2][0],vertices[i+2][1],vertices[i+2][2]);
                vec3 B = vec3(vertices[i+3][0],vertices[i+3][1],vertices[i+3][2]);
                vec3 V1 = C-A;
                vec3 V2 = D-B;
                normal.x = V1.y * V2.z - V1.z * V2.y;
                normal.y = V2.x * V1.z - V2.z * V1.x;
                normal.z = V1.x * V2.y - V1.y * V2.x;
                normal = normalize(normal);
                glNormal3f(normal.x, normal.y, normal.z);
            }
            glTexCoord2f(uv[i][0], uv[i][1]);
            DrawVertex(vertices[i]);
        }
    glEnd();

    // Record If Needed
    if (extraAction != Recording) { return; }

    int total = vertices.size();
    for (int i = 0; i < total-2; i+=2)
    {
        int start = GetTargetVertices().size()+1;
        PushVertexToTarget(vertices[i], uv[i]);
        PushVertexToTarget(vertices[i+2], uv[i+2]);
        PushVertexToTarget(vertices[i+3], uv[i+3]);
        PushVertexToTarget(vertices[i+1], uv[i+1]);
        PushFaceToTarget({start, start+1, start+2, start+3});
    }
}

tuple< vector< vector<float> >, vector< vector<float> > > Shapes::DrawFilledCircle(vector<float> center, float radius, float sidesInCircle )
{
    vector< vector<float> > vertices;
    vector< vector<float> > uv;
    float anglePerVert = 360.0 / sidesInCircle;
    for (float i = 0; i < 360.1; i+= anglePerVert)
    {
        float tetha = i*(atan(1)*4/180); // atan(1)*4 = PI
        float circleX = cos(tetha);
        float circleY = sin(tetha);
        float u = (circleX + 1.0) / 2.0;
        float v = (circleY + 1.0) / 2.0;
        float x = (radius*circleX)+center[0];
        float z = (radius*circleY)+center[2];
        uv.push_back({u, v});
        vertices.push_back({x, center[1], z});
    }
    DrawPolygon(vertices, uv);
    return make_tuple(vertices, uv);
}

tuple< vector< vector<float> >, vector< vector<float> > > Shapes::DrawAngledFilledCircle(vector<float> center, float radius, float zAngle, float sidesInCircle )
{
    vector< vector<float> > vertices;
    vector< vector<float> > uv;
    float anglePerVert = 360.0 / sidesInCircle;
    for (float i = 0; i < 360.1; i+= anglePerVert)
    {
        float tetha = i*(atan(1)*4/180); // atan(1)*4 = PI
        float circleX = cos(tetha);
        float circleY = sin(tetha);
        float u = (circleX + 1.0) / 2.0;
        float v = (circleY + 1.0) / 2.0;
        float x = (radius*circleX)+center[0];
        float y = center[1];
        float z = (radius*circleY)+center[2];

        vec4 res = vec4(x, y, z, 1);
        float magAngle = (((circleY +1.0) / 2.0)) * zAngle;
        res = rotate(mat4(1),zAngle, vec3(0,0,1)) * res;

        x = res.x;
        y = res.y;
        z = res.z;

        uv.push_back({u, v});
        vertices.push_back({x, y, z});
    }
    DrawPolygon(vertices, uv);
    return make_tuple(vertices, uv);
}


void Shapes::DrawLine(vector<float> a, vector<float> b)
{
    glBegin(GL_LINES);
        glVertex3f(a[0], a[1], a[2]);
        glVertex3f(b[0], b[1], b[2]);
    glEnd();

    // Record If Needed
    if (extraAction != Recording) { return; }

    int start = GetTargetVertices().size()+1;
    PushVertexToTarget(a, {0, 0});
    PushVertexToTarget(b, {0, 0});
    PushFaceToTarget({start, start+1});
}

void Shapes::DrawCylinder(vector<float> bottomCenter, float bottomRadius, vector<float> topCenter, float topRadius, Shapes::CylinderSideUVs uvType, float sidesInCircle, float verticalUV)
{
    tuple< vector< vector<float> >, vector< vector<float> > > circleInfo = DrawFilledCircle(bottomCenter, bottomRadius, sidesInCircle);
    vector< vector<float> > bottomCircleVertices = get<0>(circleInfo);
    vector< vector<float> > bottomCircleUV = get<1>(circleInfo);

    circleInfo = DrawFilledCircle(topCenter, topRadius, sidesInCircle);
    vector< vector<float> > topCircleVertices = get<0>(circleInfo);
    vector< vector<float> > topCircleUV = get<1>(circleInfo);

    vector< vector<float> > quadStripVertices;
    vector< vector<float> > quadStripVerticesUVs;

    int circleCount = bottomCircleVertices.size();
    float circleDelta = 1.0 / (circleCount * 1.0);
    for (int i = 0; i < circleCount; i++)
    {
        quadStripVertices.push_back(bottomCircleVertices[i]);
        quadStripVertices.push_back(topCircleVertices[i]);
        switch(uvType)
        {
            case Shapes::Wrapped:
                quadStripVerticesUVs.push_back({i*circleDelta, verticalUV});
                quadStripVerticesUVs.push_back({i*circleDelta, 0.0});
                break;
            case Shapes::Repeat:
                if ((int)sidesInCircle % 3 == 0)
                {
                    quadStripVerticesUVs.push_back({(i % 3 == 0 ? 0.0 : (i % 3 == 1 ? 0.5 : 1.0)), verticalUV});
                    quadStripVerticesUVs.push_back({(i % 3 == 0 ? 0.0 : (i % 3 == 1 ? 0.5 : 1.0)), 0.0});
                } else
                {
                    quadStripVerticesUVs.push_back({(i % 2 == 0 ? 0.0 : 0.5), verticalUV});
                    quadStripVerticesUVs.push_back({(i % 2 == 0 ? 0.0 : 0.5), 0.0});
                }
                break;
        }
    }

    DrawQuadStrip(quadStripVertices, quadStripVerticesUVs);
}

void Shapes::DrawWarpedCylinder(vector<float> bottomCenter, float bottomRadius, float bottomWarpAngle, vector<float> topCenter, float topRadius, float topWarpAngle,
                                Shapes::CylinderSideUVs uvType, float sidesInCircle, float verticalUV)
{
    tuple< vector< vector<float> >, vector< vector<float> > > circleInfo = DrawAngledFilledCircle(bottomCenter, bottomRadius, bottomWarpAngle ,sidesInCircle);
    vector< vector<float> > bottomCircleVertices = get<0>(circleInfo);
    vector< vector<float> > bottomCircleUV = get<1>(circleInfo);

    glRotatef(topWarpAngle, 0, 0, 1);
    circleInfo = DrawAngledFilledCircle(topCenter, topRadius, topWarpAngle, sidesInCircle);
    vector< vector<float> > topCircleVertices = get<0>(circleInfo);
    vector< vector<float> > topCircleUV = get<1>(circleInfo);

    vector< vector<float> > quadStripVertices;
    vector< vector<float> > quadStripVerticesUVs;

    int circleCount = bottomCircleVertices.size();
    float circleDelta = 1.0 / (circleCount * 1.0);
    for (int i = 0; i < circleCount; i++)
    {
        quadStripVertices.push_back(bottomCircleVertices[i]);
        quadStripVertices.push_back(topCircleVertices[i]);
        switch(uvType)
        {
            case Shapes::Wrapped:
                quadStripVerticesUVs.push_back({i*circleDelta, verticalUV});
                quadStripVerticesUVs.push_back({i*circleDelta, 0.0});
                break;
            case Shapes::Repeat:
                if ((int)sidesInCircle % 3 == 0)
                {
                    quadStripVerticesUVs.push_back({(i % 3 == 0 ? 0.0 : (i % 3 == 1 ? 0.5 : 1.0)), verticalUV});
                    quadStripVerticesUVs.push_back({(i % 3 == 0 ? 0.0 : (i % 3 == 1 ? 0.5 : 1.0)), 0.0});
                } else
                {
                    quadStripVerticesUVs.push_back({(i % 2 == 0 ? 0.0 : 0.5), verticalUV});
                    quadStripVerticesUVs.push_back({(i % 2 == 0 ? 0.0 : 0.5), 0.0});
                }
                break;
        }
    }

    DrawQuadStrip(quadStripVertices, quadStripVerticesUVs);
}

void Shapes::DrawCone(vector<float> bottomCenter, float bottomRadius, vector<float> topCenter, float sidesInCircle)
{
    tuple< vector< vector<float> >, vector< vector<float> > > circleInfo = DrawFilledCircle(bottomCenter, bottomRadius, sidesInCircle);
    vector< vector<float> > circleVertices = get<0>(circleInfo);
    vector< vector<float> > circleUV = get<1>(circleInfo);

    vector< vector<float> > coneFanVertices;
    vector< vector<float> > coneFanUV;

    coneFanVertices.push_back(topCenter);
    coneFanUV.push_back({0.5, 0.5});

    int circleCount = circleVertices.size();
    for (int i = 0; i < circleCount; i++)
    {
        coneFanVertices.push_back(circleVertices[i]);
        coneFanUV.push_back(circleUV[i]);
    }
    DrawTriangleFan(coneFanVertices, coneFanUV);
}

static int totalDrawnLeaves = 0;
static vector< vector<float> > GetNextLeafTextureQuadrant()
{
    vector< vector<float> > result;
    int zoom = Options::GetLeafZoom();

    float zoomDelta = 1.0f / float(zoom);
    int clampedItem = totalDrawnLeaves % (zoom * zoom);
    int col = clampedItem % zoom;
    int row = static_cast <int> (floor( float(clampedItem) / float(zoom) ));

    float xStart = float(col) * zoomDelta;
    float xEnd = xStart + zoomDelta;
    float yStart = float(row) * zoomDelta;
    float yEnd = yStart + zoomDelta;
    result.push_back({xStart, yStart});
    result.push_back({xStart, yEnd});
    result.push_back({xEnd, yStart});
    result.push_back({xEnd, yEnd});
    totalDrawnLeaves++;
    return result;
}

void Shapes::DrawLeafPlane(vector<float> origin, float width, float height, float bend)
{
    vector< vector<float> > texture = GetNextLeafTextureQuadrant();

    vector<vector<float>> points;
    points.push_back({origin[0] - width / 2.0f, 0, origin[2]});
    points.push_back({origin[0] + width / 2.0f, 0, origin[2]});
    points.push_back({origin[0] - width / 2.0f, height, origin[2]});
    points.push_back({origin[0] + width / 2.0f, height, origin[2]});
    vector<vector<float>> uvs;
    uvs.push_back({texture[0][0],texture[0][1]});
    uvs.push_back({texture[1][0],texture[1][1]});
    uvs.push_back({texture[2][0],texture[2][1]});
    uvs.push_back({texture[3][0],texture[3][1]});

    glPushMatrix();
        glTranslatef(0.0,origin[1],0.0);
        glRotatef(360*Random::NextRandom(), 0, 1, 0);
        glRotatef(bend, 0, 0, 1);
        DrawQuadStrip(points, uvs);
    glPopMatrix();
}

void Shapes::DrawLeaf(vector<float> origin, float width, float height, float bend)
{
    vector< vector<float> > texture = GetNextLeafTextureQuadrant();

    vector<vector<float>> points;
    float minX = origin[0] - width;
    float maxX = origin[0] + width;
    float deltaX = maxX - minX;
    float z = origin[2];
    // Top
    points.push_back({minX + deltaX*0.37,height*0.8,z});
    points.push_back({origin[0], height, z});
    points.push_back({minX + deltaX*0.63,height*0.8,z});
    // Right Half
    points.push_back({minX + deltaX*0.7,height*0.6,z});
    points.push_back({minX + deltaX*0.8,height*0.4,z});
    points.push_back({minX + deltaX*0.7,height*0.2,z});
    points.push_back({minX + deltaX*0.6,height*0.15,z});
    // Left Half
    points.push_back({minX + deltaX*0.4,height*0.15,z});
    points.push_back({minX + deltaX*0.3,height*0.2,z});
    points.push_back({minX + deltaX*0.2,height*0.4,z});
    points.push_back({minX + deltaX*0.3,height*0.6,z});

    vector<vector<float>> uvs;

    float minXT = texture[0][0];
    float maxXT = texture[2][0];
    float deltaXT = maxXT - minXT;
    float minYT = texture[0][1];
    float maxYT = texture[1][1];
    float deltaYT = maxYT - minYT;

    // Top
    uvs.push_back({minXT + deltaXT*0.37,minYT + deltaYT*0.8});
    uvs.push_back({minXT + deltaXT*0.5, maxYT});
    uvs.push_back({minXT + deltaXT*0.63,minYT + deltaYT*0.8});
    // Right Half
    uvs.push_back({minXT + deltaXT*0.7,minYT + deltaYT*0.6});
    uvs.push_back({minXT + deltaXT*0.8,minYT + deltaYT*0.4});
    uvs.push_back({minXT + deltaXT*0.7,minYT + deltaYT*0.2});
    uvs.push_back({minXT + deltaXT*0.6,minYT + deltaYT*0.15});
    // Left Half
    uvs.push_back({minXT + deltaXT*0.4,minYT + deltaYT*0.15});
    uvs.push_back({minXT + deltaXT*0.3,minYT + deltaYT*0.2});
    uvs.push_back({minXT + deltaXT*0.2,minYT + deltaYT*0.4});
    uvs.push_back({minXT + deltaXT*0.3,minYT + deltaYT*0.6});


    glPushMatrix();
        glTranslatef(0.0,origin[1],0.0);
        glRotatef(360*Random::NextRandom(), 0, 1, 0);
        glRotatef(bend, 0, 0, 1);
        DrawPolygon(points, uvs);
    glPopMatrix();
}

void Shapes::DrawFoldedLeaf(vector<float> origin, float width, float height, float bend)
{
    vector< vector<float> > texture = GetNextLeafTextureQuadrant();

    vector<vector<float>> points;
    float minX = origin[0] - width;
    float maxX = origin[0] + width;
    float deltaX = maxX - minX;
    float zMax = deltaX * Random::NextRandom();
    float z = origin[2];
    // Top
    points.push_back({minX + deltaX*0.37,height*0.8,z});
    points.push_back({origin[0], height, z+zMax * 0.75});
    points.push_back({minX + deltaX*0.63,height*0.8,z});
    // Right Half
    points.push_back({minX + deltaX*0.7,height*0.6,z+zMax*0.25});
    points.push_back({minX + deltaX*0.8,height*0.4,z});
    points.push_back({minX + deltaX*0.7,height*0.2,z+zMax*0.25});
    points.push_back({minX + deltaX*0.6,height*0.15,z+zMax*0.5});
    // Left Half
    points.push_back({minX + deltaX*0.4,height*0.15,z+zMax*0.5});
    points.push_back({minX + deltaX*0.3,height*0.2,z+zMax*0.25});
    points.push_back({minX + deltaX*0.2,height*0.4,z});
    points.push_back({minX + deltaX*0.3,height*0.6,z+zMax*0.25});

    vector<vector<float>> uvs;

    float minXT = texture[0][0];
    float maxXT = texture[2][0];
    float deltaXT = maxXT - minXT;
    float minYT = texture[0][1];
    float maxYT = texture[1][1];
    float deltaYT = maxYT - minYT;

    // Top
    uvs.push_back({minXT + deltaXT*0.37,minYT + deltaYT*0.8});
    uvs.push_back({minXT + deltaXT*0.5, maxYT});
    uvs.push_back({minXT + deltaXT*0.63,minYT + deltaYT*0.8});
    // Right Half
    uvs.push_back({minXT + deltaXT*0.7,minYT + deltaYT*0.6});
    uvs.push_back({minXT + deltaXT*0.8,minYT + deltaYT*0.4});
    uvs.push_back({minXT + deltaXT*0.7,minYT + deltaYT*0.2});
    uvs.push_back({minXT + deltaXT*0.6,minYT + deltaYT*0.15});
    // Left Half
    uvs.push_back({minXT + deltaXT*0.4,minYT + deltaYT*0.15});
    uvs.push_back({minXT + deltaXT*0.3,minYT + deltaYT*0.2});
    uvs.push_back({minXT + deltaXT*0.2,minYT + deltaYT*0.4});
    uvs.push_back({minXT + deltaXT*0.3,minYT + deltaYT*0.6});


    glPushMatrix();
        glTranslatef(0.0,origin[1],0.0);
        glRotatef(360*Random::NextRandom(), 0, 1, 0);
        glRotatef(bend, 0, 0, 1);
        DrawPolygon(points, uvs);
    glPopMatrix();
}


void Shapes::DrawCircleLeaf(vector<float> origin, float width, float height, float bend)
{
    glPushMatrix();
        glTranslatef(0.0,+0.5f*height,0.0);
        glRotatef(360*Random::NextRandom(), 0, 1, 0);
        glRotatef(bend, 0, 0, 1);
        //glScalef(width/height, 0, 0);
        DrawFilledCircle(origin, height /2.0f, 8);
    glPopMatrix();
}


static void SaveModelAsFile()
{
    const char* modelName;
    switch(renderTarget)
    {
        case Shapes::Tree:
            modelName = treeModelName;
            break;
        case Shapes::Leaves:
            modelName = leavesModelName;
            break;
        case Shapes::Extras:
            modelName = extrasModelName;
            break;
        default:
            printf("Unable to save model, unknown model name type.");
            return;
    }
    char* fileName = Utilities::FilePath(Utilities::Concatenate(modelName, Options::GetSession(), ".obj"));

    printf("Saving Model Into File: \"%s\"\n", fileName);

    ofstream model;
    model.open(fileName);

    if (!model.is_open()) { printf("An Error Has Occurred Attempting To Create File\n"); return; }

    vector< vector<float> > vertices = GetTargetVertices();
    int total = vertices.size();
    for(int i = 0; i < total; i++)
    {
        model << "v "<< vertices[i][0] << " " << vertices[i][1] << " "  << vertices[i][2] << endl;
    }

    vector< vector<float> > uvs = GetTargetUVs();
    total = uvs.size();
    for(int i = 0; i < total; i++)
    {
        model << "vt "<< uvs[i][0] << " " << uvs[i][1] << endl;
    }

    vector< vector<int> > faces = GetTargetFaces();
    total = faces.size();
    int elapsed = 1;
    for(int i = 0; i < total; i++)
    {
        model << "f ";
        int vCount = faces[i].size();
        for (int j = 0; j < vCount; j++)
        {
            model << faces[i][j] << "/" << elapsed << " ";
            elapsed++;
        }
        model << endl;
    }

    model.close();
    printf("Model Saved Successfully.\n\n");
}

void Shapes::SaveModel() { extraAction = AboutToRecord; }

static void* helv12 = (void*)GLUT_BITMAP_HELVETICA_12;
static void* helv18 = (void*)GLUT_BITMAP_HELVETICA_18;
static void DrawText(float x,float y,void *font,char *string)
{
    char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) { glutBitmapCharacter(font, *c); }
}

void Shapes::EarlyUpdate()
{
    totalDrawnLeaves = 0;

    switch(extraAction)
    {
        case Recording:
            treeVertices.clear();
            treeUVs.clear();
            treeFaces.clear();
            leavesVertices.clear();
            leavesUVs.clear();
            leavesFaces.clear();
            extrasVertices.clear();
            extrasUVs.clear();
            extrasFaces.clear();
            printf("Recording the model vertices, uvs and faces to export.\nPlease wait, this can take a while...\n\n");
            break;
    }
}

void Shapes::SetRecordTarget(Shapes::RenderTarget target) { renderTarget = target; }

const int POPUP_WIDTH = 250;
const int POPUP_HEIGHT = 80;
static GLint infoBackgroundTex;
void Shapes::LateUpdate()
{
    float startX = float(glutGet(GLUT_WINDOW_WIDTH)/2.0) - (float(POPUP_WIDTH) / 2.0);
    float startY = float(glutGet(GLUT_WINDOW_HEIGHT)/2.0) - (float(POPUP_HEIGHT) / 2.0);
    float topY = startY+POPUP_HEIGHT;

    switch(extraAction)
    {
        case AboutToRecord:
            extraAction = Recording;

            CameraHandler::SetOrthographicProjection();
            Images::LoadTextureStart(infoBackgroundTex);
            glBegin (GL_QUADS);
                glTexCoord2f(0.02,0.02);
                glVertex2d(startX,startY);
                glTexCoord2f(0.02,0.98);
                glVertex2d(startX,topY);
                glTexCoord2f(0.98,0.98);
                glVertex2d(startX+POPUP_WIDTH,topY);
                glTexCoord2f(0.98,0.02);
                glVertex2d(startX+POPUP_WIDTH,startY);
            glEnd();
            Images::LoadTextureEnd();

            startX += 3;
            startY += 3;
            glPushMatrix();
                glLoadIdentity();
                glColor3f(1,1,1);
                DrawText(startX, startY + 20, helv18, "Saving Models");
                DrawText(startX, startY + 38, helv12, "Currently recording and saving models.");
                DrawText(startX, startY + 56, helv12, "Please wait, this can take a while...");
            glPopMatrix();
            CameraHandler::RestorePerspectiveProjection();

            break;
        case Recording:
            extraAction = SaveToFile;
            printf("Export model info successfully recorded.\n");
            break;
        case SaveToFile:
            extraAction = None;

            printf("Creating models files...\n");

            // Save All Models
            renderTarget = Shapes::Tree;
            SaveModelAsFile();

            renderTarget = Shapes::Leaves;
            SaveModelAsFile();

            renderTarget = Shapes::Extras;
            SaveModelAsFile();
            break;
    }
}

void Shapes::Start()
{
    extraAction = None;
    infoBackgroundTex = Images::GetImageTextureFromFilePath("UI\\Darker.png");
}
