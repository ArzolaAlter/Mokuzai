/*
    This entire module was created around the Wichmann-Hill pseudo number generator, a random generator that works by making use of seeds.
    It was selected to make use of the seed functionality and be able to "store" trees.
*/
#include "Random.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <Options.h>

//------------------------------------------------------------------------------------//
/*----------------------------------- FIELDS -----------------------------------------*/
//------------------------------------------------------------------------------------//

const int SEED_MIN = 100;

static int s1, s2, s3 = SEED_MIN;
static std::vector<double> randoms;
static int currentIndex = -1;
static const int MAX_RANDOMS = 8192;

static int lastSeed = -1;
static int currentIndexCopy;

//------------------------------------------------------------------------------------//
/*---------------------------------- METHODS -----------------------------------------*/
//------------------------------------------------------------------------------------//

static void RecalculateSeeds()
{
    randoms.clear();

    for (int i = 0; i < MAX_RANDOMS; i++)
    {
        // Wichmann-Hill Algorithm.
        s1 = (171 * s1) % 30269;
        s2 = (171 * s2) % 30307;
        s3 = (171 * s3) % 30323;
        double intPart;
        double next = modf(s1/30269.0 + s2/30307.0 + s3/30323.0, &intPart);
        randoms.push_back(next);
    }
}

static void ChangeSeed(int delta)
{
    int seed = Options::GetSeed();
    seed += delta;
    if (seed < 0) { seed = 30000; } // Seed needs to be kept on certain boundaries
    if (seed > 30000) { seed = 0; }
    Options::SetSeed(seed);

    s1 = SEED_MIN + seed;
    s2 = SEED_MIN + seed;
    s3 = SEED_MIN + seed;

    s1 = (171 * s1) % 30269;
    s2 = (171 * s2) % 30307;
    s3 = (171 * s3) % 30323;

    RecalculateSeeds();
}

void Random::Update()
{
    if (lastSeed != Options::GetSeed())
    {
        lastSeed = Options::GetSeed();
        ChangeSeed(0);
    }
    currentIndex = -1;
}

double Random::NextRandom()
{
    currentIndex++;
    if (currentIndex > MAX_RANDOMS -1) { currentIndex = 0; }
    return randoms[currentIndex];
}

double Random::NextRandomRanged(double minNumber, double maxNumber)
{
    double next = NextRandom();
    return minNumber + (next * (maxNumber-minNumber));
}

double Random::NextRandomDeltaClamped(double minNumber, double maxNumber, double clampMin, double clampMax)
{
    double res = Random::NextRandomDelta(minNumber, maxNumber);
    if (res < clampMin) { return clampMin; }
    if (res > clampMax) { return clampMax; }
    return res;
}

double Random::NextRandomDelta(double minNumber, double delta) { return Random::NextRandomRanged(minNumber - delta, minNumber + delta);  }

int Random::NextRandomDeltaIntClamped(int minClamp, int maxClamp, int  minNumber, int  delta)
{
    int result;
    if (Random::NextRandom() > 0.5)
    {
        result = minNumber - (int)((Random::NextRandom()-0.0001) / (1.0f / float(delta+1)));
    }
    else
    {
        result =  minNumber + (int)((Random::NextRandom()-0.0001) / (1.0f / float(delta+1)));
    }
    if (result < minClamp) { return minClamp; }
    if (result > maxClamp) { return maxClamp; }
    return result;
}

void Random::SaveSeeds()
{
    currentIndexCopy = currentIndex;
}

void Random::RestoreSeeds()
{
    currentIndex = currentIndexCopy;
}

void Random::PressNormalKey(unsigned char key, int x, int y)
{

}

void Random::ReleaseNormalKey(unsigned char key, int x, int y)
{

}

void Random::PressSpecialKey(int key, int x, int y)
{
    switch (key)
   {
     case GLUT_KEY_PAGE_UP:   ChangeSeed(1); break;
     case GLUT_KEY_PAGE_DOWN:    ChangeSeed(-1); break;
   }
}

void Random::ReleaseSpecialKey(int key, int x, int y)
{

}

void Random::Start()
{
    ChangeSeed(0);
}
