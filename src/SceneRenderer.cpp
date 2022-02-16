#include "SceneRenderer.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>
#include <Random.h>
#include <TreeGenerator.h>
#include <CameraHandler.h>

using namespace std;

const GLfloat light_ambient[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat light_diffuse[]  = { 0.5f, 0.5f, 0.5f, 0.5f };
const GLfloat light_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat light_position[] = { -50.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 300.0f };

//------------------------------------------------------------------------------------//
/*---------------------------------- METHODS -----------------------------------------*/
//------------------------------------------------------------------------------------//

const int GRID = 20;
const float GRID_COLOR_R = 200.0/255.0;
const float GRID_COLOR_G = 200.0/255.0;
const float GRID_COLOR_B = 200.0/255.0;

void SceneRenderer::Update()
{
        // Rotate Object
        glTranslatef(0 , 0 , -5 );

        vector<float> objectAngle = CameraHandler::GetObjectRotation();
        float rx = objectAngle[0], ry = objectAngle[1], rz = objectAngle[2];

        glRotatef(rx,1,0,0);
        glRotatef(ry,0,1,0);
        glRotatef(rz,0,0,1);
    glPushMatrix();

        glEnable( GL_BLEND );
        glBegin(GL_LINES);
            for(int i=-GRID;i<=GRID;i++)
            {
                if (i==0) { glColor3f(0,0,1); } else { glColor4f(GRID_COLOR_R, GRID_COLOR_G, GRID_COLOR_B,0.3); };
                    glVertex3f(i,-1,-GRID);
                    glVertex3f(i,-1,GRID);
                if (i==0) { glColor3f(1,0,0); } else { glColor4f(GRID_COLOR_R, GRID_COLOR_G, GRID_COLOR_B,0.3); };
                    glVertex3f(-GRID,-1,i);
                    glVertex3f(GRID,-1,i);
            };
        glEnd();
        glDisable( GL_BLEND );

        TreeGenerator::DrawTree();
    glPopMatrix();

}

void SceneRenderer::Start()
{
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	glClearColor(148.0/255.0,212.0/255.0,237.0/255.0,1.0); // Sky Blue

    //Setup:
    glShadeModel(GL_SMOOTH);
    glDepthFunc(GL_LEQUAL);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
