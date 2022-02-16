#include "CameraHandler.h"

#include <vector>
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/freeglut.h>
#endif

using namespace std;

//------------------------------------------------------------------------------------//
/*----------------------------------- FIELDS -----------------------------------------*/
//------------------------------------------------------------------------------------//

const int targetW = 1280;
const int targetH = 720;

const float originX = -0.6;
const float originY = 0.35;
const float originZ = 1;

static int screenW = 0;
static int screenH = 0;

const float DELTA_MOVE = 0.1f;
const float DELTA_ROTATE = 0.2f;

static vector<float> cameraPosition;
static vector<float> objectAngle;

static vector<float> deltaMove;
static vector<float> deltaAngle;

static bool wasMouseWheelEvent;

//------------------------------------------------------------------------------------//
/*---------------------------------- METHODS -----------------------------------------*/
//------------------------------------------------------------------------------------//

int CameraHandler::TargetH() { return targetH; }
int CameraHandler::CurrentH() { return screenH; }
int CameraHandler::TargetW() {return targetW;}

void CameraHandler::SetOrthographicProjection() {

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);

	// save previous matrix which contains the
	//settings for the perspective projection
	glPushMatrix();

	// reset matrix
	glLoadIdentity();

	// set a 2D orthographic projection
	gluOrtho2D(0, screenW,screenH, 0);

	// switch back to modelview mode
	glMatrixMode(GL_MODELVIEW);
}

void CameraHandler::RestorePerspectiveProjection()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void CameraHandler::Update()
{
	cameraPosition[0] += deltaMove[0];
	cameraPosition[1] += deltaMove[1];
	cameraPosition[2] += -deltaMove[2];

	objectAngle[0] += deltaAngle[0];
	objectAngle[1] += deltaAngle[1];
	objectAngle[2] += deltaAngle[2];

	if (wasMouseWheelEvent)
    {
        wasMouseWheelEvent = false;
        deltaMove[2] = 0;
    }

    float x = cameraPosition[0], y = cameraPosition[1], z = cameraPosition[2];
	gluLookAt(x, y, z, x, y, z-1, 0.0f, 1.0f,  0.0f);
}

void CameraHandler::ResetPosition()
{
    cameraPosition[0] = originX;
    cameraPosition[1] = originY;
    cameraPosition[2] = originZ;
}
void CameraHandler::ResetRotation()
{
    objectAngle[0] = objectAngle[1] = objectAngle[2] = 0;
}

void CameraHandler::PressNormalKey(unsigned char key, int x, int y)
{

}

void CameraHandler::ReleaseNormalKey(unsigned char key, int x, int y)
{
    switch (key)
   {
     case 127: ResetPosition(); break; // DELETE
   }
}

bool moving = false;
bool rotating = false;
bool CameraHandler::IsMoving()
{
    return moving || rotating;
}

void CameraHandler::PressSpecialKey(int key, int xx, int yy)
{
   switch (key)
   {
     case GLUT_KEY_RIGHT:   deltaMove[0] = DELTA_MOVE; break;
     case GLUT_KEY_LEFT:    deltaMove[0] = -DELTA_MOVE; break;
     case GLUT_KEY_UP:
         if(glutGetModifiers() == GLUT_ACTIVE_CTRL)
         {
            deltaMove[1] = DELTA_MOVE; break;
            break;
         }
         deltaMove[2] = DELTA_MOVE;
         break;
     case GLUT_KEY_DOWN:
         if(glutGetModifiers() == GLUT_ACTIVE_CTRL)
         {
            deltaMove[1] = -DELTA_MOVE; break;
            break;
         }
         deltaMove[2] = -DELTA_MOVE;
         break;

     case GLUT_KEY_INSERT: ResetRotation(); break;
   }
}

void CameraHandler::ReleaseSpecialKey(int key, int x, int y)
{
    switch (key)
    {
     case GLUT_KEY_RIGHT:
     case GLUT_KEY_LEFT:    deltaMove[0] = 0; break;
     case GLUT_KEY_UP:
     case GLUT_KEY_DOWN:    deltaMove[1] = deltaMove[2] = 0; break;
    }
}

int lastX = 0, lastY = 0;
static void HandleObjectRotation(int x, int y)
{
    int deltaX = x - lastX;
    int deltaY = y - lastY;

    lastY = y;
    lastX = x;

    if (moving)
    {
        deltaMove[1] = DELTA_MOVE * deltaY * 0.025;
        deltaMove[0] = DELTA_MOVE * deltaX * -0.025;
        return;
    }

    if (!rotating) { return; }

    deltaAngle[0] = DELTA_ROTATE * deltaY;
    deltaAngle[1] = DELTA_ROTATE * deltaX;
}

void CameraHandler::MouseMove(int x, int y)
{
    HandleObjectRotation(x, y);
}

void CameraHandler::MouseClick(int button, int state, int x, int y)
{
    if (button == GLUT_MIDDLE_BUTTON)
    {
        switch (state)
        {
            case GLUT_UP:
                deltaMove[0] = deltaMove[1] = 0;
                moving = false;
                break;
            case GLUT_DOWN:
                lastY = y;
                lastX = x;
                moving  = true;
                break;
        }
        return;
    }

    if (button != GLUT_RIGHT_BUTTON) { return; }

    switch (state)
    {
        case GLUT_UP:
            deltaAngle[0] = deltaAngle[1] = deltaAngle[2] = 0;
            rotating = false;
            break;
        case GLUT_DOWN:
            lastY = y;
            lastX = x;
            rotating = true;
            break;
    }
}

void CameraHandler::MouseWheel(int button, int dir, int x, int y)
{
    wasMouseWheelEvent = true;

    if (dir > 0)
    {
        // Zoom in
        deltaMove[2] = DELTA_MOVE*10;
        return;
    }

    // Zoom out
    deltaMove[2] = -DELTA_MOVE*10;
}

void CameraHandler::OnResize(int w, int h) { screenH = h; screenW = w; }

vector<float> CameraHandler::GetObjectRotation() { return objectAngle; }

void CameraHandler::Start()
{
    for (int i = 0; i < 3; i++)
    {
        cameraPosition.push_back(0);
        objectAngle.push_back(0);

        deltaMove.push_back(0);
        deltaAngle.push_back(0);
    }

    CameraHandler::ResetPosition();
}
