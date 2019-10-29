#include "otr.h"

OTR::OTR()
{
}

void OTR::Init(vector<Point> input)
{
	points_input = input;
	delaunay_input.insert(input.begin(), input.end());
	
}
