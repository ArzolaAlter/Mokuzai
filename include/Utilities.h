#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>

using namespace std;

class Utilities
{
    public:
        static char* FilePath(const string&);
        static char* Concatenate(const string&, int);
        static char* Concatenate(const string&, float);
        static char* Concatenate(const string&, const string&);
        static char* Concatenate(const string&, int, const string&);
        static char* Concatenate(const string&, float, const string&);

    protected:

    private:
};

#endif // UTILITIES_H
