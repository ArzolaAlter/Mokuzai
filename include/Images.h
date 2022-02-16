#ifndef IMAGES_H
#define IMAGES_H

#include <GL/gl.h>

class Images
{
    public:
        static GLuint GetImageTextureFromFilePath(char*);
        static GLuint GetImageTexture(char*);
        static void LoadTextureStart(GLuint, bool = false);
        static void LoadTextureEnd();

    protected:

    private:
};

#endif // IMAGES_H
