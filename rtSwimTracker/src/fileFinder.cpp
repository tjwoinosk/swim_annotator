#include "fileFinder.h"

std::string fileFinder::retrunAbsolutePath(const std::string& root)
{
  bf::path testP(testDataLocation + root);
  bf::path speedP(speedTestDataLocation + root);
  bf::path result;

  if (bf::exists(testP))
  {
    if (bf::is_regular_file(testP))
    {
      result = bf::absolute(testP);
      return result.string();
    }
  }
  else if (bf::exists(speedP))
  {
    if (bf::is_regular_file(speedP))
    {
      result = bf::absolute(speedP);
      return result.string();
    }
  }

  return std::string();
}
