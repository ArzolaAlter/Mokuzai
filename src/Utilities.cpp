#include "Utilities.h"

#include <string>
#include <sstream>

using namespace std;

#if DEBUG
const string currentDir =
#if ARZOLA
"C:\\Mokuzai Test\\" // You can set a custom folder for the contents of "Mokuzai Assets.zip" here.
#endif
;
#endif // DEBUG

char* Utilities::FilePath(const string& fileName)
{
  ostringstream oss;
  #if DEBUG
  oss << currentDir;
  #endif // DEBUG
  oss << fileName;
  return const_cast<char*>(oss.str().c_str());
}

char* Utilities::Concatenate(const string& str, int num)
{
  ostringstream oss;
  oss << str << num;
  return const_cast<char*>(oss.str().c_str());
}

char* Utilities::Concatenate(const string& str, const string& str2)
{
  ostringstream oss;
  oss << str << str2;
  return const_cast<char*>(oss.str().c_str());
}

char* Utilities::Concatenate(const string& str, float num)
{
  ostringstream oss;
  oss << str << num;
  return const_cast<char*>(oss.str().c_str());
}

char* Utilities::Concatenate(const string& str1, int num, const string& str2)
{
  ostringstream oss;
  oss << str1 << num << str2;
  return const_cast<char*>(oss.str().c_str());
}

char* Utilities::Concatenate(const string& str1, float num, const string& str2)
{
  ostringstream oss;
  oss << str1 << num << str2;
  return const_cast<char*>(oss.str().c_str());
}
