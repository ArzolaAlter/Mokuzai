#ifndef RANDOM_H
#define RANDOM_H

const int MIN_NRR = -1;
const int MAX_NRR = 1;

class Random
{
    public:
        static void Update();
        static void Start();
        static double NextRandom();
        static void SaveSeeds();
        static void RestoreSeeds();
        static int NextRandomDeltaIntClamped(int, int, int = MIN_NRR, int = MAX_NRR);
        static double NextRandomDelta(double = MIN_NRR, double = MAX_NRR);
        static double NextRandomRanged(double = MIN_NRR, double = MAX_NRR);
        static double NextRandomDeltaClamped(double = MIN_NRR, double = MAX_NRR, double = 0, double = 0);
        static void PressNormalKey(unsigned char, int, int);
        static void ReleaseNormalKey(unsigned char, int, int);
        static void PressSpecialKey(int, int, int);
        static void ReleaseSpecialKey(int, int, int);

    protected:

    private:
};

#endif // RANDOM_H
