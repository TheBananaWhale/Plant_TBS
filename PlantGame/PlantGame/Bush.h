#pragma once

#include "Unit.h"

class Bush : public Unit
{
public:
	Bush(int w, int h);	//Sets classID to BUSH plus whatever else we come up with
	~Bush();	//Garbage collection
};
