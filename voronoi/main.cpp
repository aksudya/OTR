#include "otr.h"

int main()
{
	std::vector<Point> points_1;
	std::vector<Point> points_re;
	CGAL::Random_points_on_circle_2<Point> point_generator(1.);
	CGAL::cpp11::copy_n(point_generator, 200, std::back_inserter(points_1));

	for (auto iter = points_1.begin(); iter != points_1.end(); iter++)
	{
		double xx = iter->hx() * 300 + 400;
		double yy = iter->hy() * 300 + 400;
		Point p(xx, yy);
		points_re.push_back(p);
	}

	OTR a;
	a.Init(points_re);
}