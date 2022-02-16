#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <GL/freeglut.h>

#include <vector>
#include <Random.h>
#include <CameraHandler.h>
#include <SceneRenderer.h>
#include <Options.h>
#include <Shapes.h>
#include <TreeGenerator.h>

//------------------------------------------------------------------------------------//
/*----------------------------------- FIELDS -----------------------------------------*/
//------------------------------------------------------------------------------------//

const int minH = 460;
const int minW = 640;

//------------------------------------------------------------------------------------//
/*---------------------------------- METHODS -----------------------------------------*/
//------------------------------------------------------------------------------------//

void ResizeWindow(int w, int h)
{
    bool change = false;
    if (h < minH) { change = true; }
    if (w < minW) { change = true; }
    if (change) { glutReshapeWindow(minW > w ? minW : w, minH > h ? minH : h); }

	float ratio =  w * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	CameraHandler::OnResize(w,h);
	Options::OnResize(w,h);
}

void RenderUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Early Update
    Shapes::EarlyUpdate();

    // Update
    Random::Update();
    CameraHandler::Update();
    SceneRenderer::Update();
    Options::Update();

    // Late Update
    Shapes::LateUpdate();

    glutSwapBuffers();
}

void PressNormalKey(unsigned char key, int x, int y)
{
   Options::PressNormalKey(key,x,y);
   CameraHandler::PressNormalKey(key,x,y);
   Random::PressNormalKey(key,x,y);
}

void ReleaseNormalKey(unsigned char key, int x, int y)
{
   Options::ReleaseNormalKey(key,x,y);
   CameraHandler::ReleaseNormalKey(key,x,y);
   Random::ReleaseNormalKey(key,x,y);
}

void PressSpecialKey(int key, int x, int y)
{
   Options::PressSpecialKey(key,x,y);
   CameraHandler::PressSpecialKey(key,x,y);
   Random::PressSpecialKey(key,x,y);
}

void ReleaseSpecialKey(int key, int x, int y)
{
   Options::ReleaseSpecialKey(key,x,y);
   CameraHandler::ReleaseSpecialKey(key,x,y);
   Random::ReleaseSpecialKey(key,x,y);
}

void MouseClick(int button, int state, int x, int y)
{
    Options::MouseClick(button, state, x, y);
    CameraHandler::MouseClick(button, state, x, y);
}

void MouseMove(int x, int y)
{
    CameraHandler::MouseMove(x, y);
}

void MouseWheel(int button, int dir, int x, int y)
{
    CameraHandler::MouseWheel(button, dir, x, y);
}

int main(int argc, char **argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glEnable(GL_DEPTH_TEST);

	// Initialize And Center The Window
    int screenH = glutGet(GLUT_SCREEN_HEIGHT);
    int screenW = glutGet(GLUT_SCREEN_WIDTH);
    int targetW = CameraHandler::TargetW();
    int targetH = CameraHandler::TargetH();
	int initialX = screenW < targetW ? 0 : (screenW - targetW)/2.0;
	int initialY = screenH < targetH ? 0 : (screenH - targetH)/2.0;
	glutInitWindowPosition(initialX,initialY);
	glutInitWindowSize(targetW,targetH);
	glutCreateWindow("Mokuzai - Luis Arzola");

	// Register Render Callbacks
	glutDisplayFunc(RenderUpdate);
	glutReshapeFunc(ResizeWindow);
	glutIdleFunc(RenderUpdate);

    // Register Input Callbacks
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(PressNormalKey);
	glutKeyboardUpFunc(ReleaseNormalKey);
	glutSpecialFunc(PressSpecialKey);
	glutSpecialUpFunc(ReleaseSpecialKey);
	glutMouseFunc(MouseClick);
	glutMotionFunc(MouseMove);
	glutMouseWheelFunc(MouseWheel);

	// Initialize Modules
	Options::Start();
    Random::Start();
    CameraHandler::Start();
    SceneRenderer::Start();
    Shapes::Start();
    TreeGenerator::Start();

	// Start Update Loop
	glutMainLoop();

	return 1;
}
