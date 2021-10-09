#include "fileFinder.h"

std::string fileFinder::absolutePath(const std::string& root)
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

std::string fileFinder::returnDataLocation()
{
    bf::path testP(testDataLocation);
    bf::path result;

    if (bf::exists(testP))
    {
        result = bf::absolute(testP);
        return result.string();
    }
    return std::string();
}

std::string fileFinder::returnSpeedTestLocation()
{
    bf::path speedP(speedTestDataLocation);
    bf::path result;

    if (bf::exists(speedP))
    {
        result = bf::absolute(speedP);
        return result.string();
    }
    return std::string();
}