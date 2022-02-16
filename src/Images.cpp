#include "Images.h"
#include <SOIL/SOIL.h>
#include <GL/gl.h>
#include <stdio.h>
#include <Utilities.h>

//------------------------------------------------------------------------------------//
/*---------------------------------- METHODS -----------------------------------------*/
//------------------------------------------------------------------------------------//

GLuint Images::GetImageTextureFromFilePath(char* path)
{
    char* fileName = Utilities::FilePath(path);
    GLuint tex = SOIL_load_OGL_texture(fileName, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT);

    if (tex == 0)
    {
        printf("Error loading file: %s (%s)\n", fileName, SOIL_last_result());
    }

    return tex;
}

GLuint Images::GetImageTexture(char* fileName)
{
    GLuint tex = SOIL_load_OGL_texture(fileName, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT);

    if (tex == 0)
    {
        printf("Error loading file: %s (%s)\n", fileName, SOIL_last_result());
    }

    return tex;
}

void Images::LoadTextureStart(GLuint texture, bool loadFor3DSpace)
{
	glPushMatrix();
        if (!loadFor3DSpace) { glLoadIdentity(); }
        glColor4f(1.0,1.0,1.0,1.0);

        glEnable(GL_TEXTURE_2D);

        if (!loadFor3DSpace) { glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE ); } // Allows original png colors to be respected
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Images::LoadTextureEnd()
{
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
