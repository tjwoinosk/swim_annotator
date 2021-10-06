#ifndef FILEFINDER_H
#define FILEFINDER_H

#include <boost/filesystem.hpp>
#include <string>


namespace bf = boost::filesystem;

class fileFinder
{
public:

  fileFinder() {};

  std::string retrunAbsolutePath(std::string root);

private:

  const std::string testDataLocation = "test\\testData\\";
  const std::string speedTestDataLocation = "test\\speedTestResults\\";

};


#endif // FILEFINDER_H
