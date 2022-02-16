#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <vector>

using namespace std;

class CameraHandler
{
    public:
        static int TargetW();
        static int TargetH();
        static int CurrentH();
        static void SetOrthographicProjection();
        static void RestorePerspectiveProjection();
        static void Update();
        static void Start();
        static bool IsMoving();
        static void PressNormalKey(unsigned char, int, int);
        static void ReleaseNormalKey(unsigned char, int, int);
        static void PressSpecialKey(int, int, int);
        static void ReleaseSpecialKey(int, int, int);
        static void MouseClick(int, int, int, int);
        static void MouseWheel(int, int, int, int);
        static void MouseMove(int, int);
        static void OnResize(int, int);
        static void ResetPosition();
        static void ResetRotation();
        static vector<float> GetObjectRotation();

    protected:

    private:
};

#endif // CAMERAHANDLER_H
