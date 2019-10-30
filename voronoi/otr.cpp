#include "otr.h"

OTR::OTR()
{
}

void OTR::Init(vector<Point> input)
{
	points_input = input;
	delaunay_input.insert(input.begin(), input.end());
	
}

void OTR::InitPriQueue()
{
	for (auto eiter=delaunay_input.finite_edges_begin();eiter!=delaunay_input.finite_edges_begin();eiter++)
	{
		Segment s = delaunay_input.segment(eiter);
		Point a = s.point(0);
		Point b = s.point(1);
	}
}
