#ifndef OBJECTCENTERING_H
#define OBJECTCENTERING_H

//TODO takes the one tracking box on the swimmer we are analyzing and return the datatype
//that will define where the camera needs to move
//camera moves by the following switches: switch to go up, left, right, down, not go left or down, not go right or left
// we can only have one switch at a moment activated
//9 different options - think of as two servos: left, right, nothing; down, up, nothing
//sending those signals from this object, we can control this camera
//maybe every frame or every other frame we can update (will decide later)
//would be in the frame analysis main loop, where everytime the frame goes in, you find the swimmer tracking
// you put the box you finished tracking into this object, and then it goes left or right

struct tiltPanCommands
{
	bool moveRight; // if both are true or false, then don't move
	bool moveLeft;
	bool moveDown;
	bool moveUp;
};

class objectCentering
{
public:
	objectCentering();
	~objectCentering();

	void calculate() {};

private:

};

objectCentering::objectCentering()
{
}

objectCentering::~objectCentering()
{
}

#endif // !OBJECTCENTERING_H

