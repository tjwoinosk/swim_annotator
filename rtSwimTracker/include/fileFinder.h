#ifndef FILEFINDER_H
#define FILEFINDER_H

#include <boost/filesystem.hpp>
#include <string>


namespace bf = boost::filesystem;

class fileFinder
{
public:

	fileFinder() {};

	std::string absolutePath(const std::string& root);
	std::string returnDataLocation();
	std::string returnSpeedTestLocation();

private:

	std::string createNewDir(bf::path newDir);

	const std::string testDataLocation = "test\\testData\\";
	const std::string speedTestDataLocation = "test\\speedTestResults\\";

};


#endif // FILEFINDER_H