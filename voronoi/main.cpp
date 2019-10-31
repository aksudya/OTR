#include "otr.h"


OTR a;
std::vector<Point> points_1;
std::vector<Point> points_re;

void otr_extrat()
{
	//std::vector<Point> points_1;
	//std::vector<Point> points_re;

	//CGAL::Random_points_on_circle_2<Point> point_generator(1.);
	CGAL::Random rng(1);
	CGAL::Random_points_in_square_2<Point> point_generator(1., rng);
	CGAL::cpp11::copy_n(point_generator, 10, std::back_inserter(points_1));

	for (auto iter = points_1.begin(); iter != points_1.end(); iter++)
	{
		double xx = iter->hx() * 300 + 400;
		double yy = iter->hy() * 300 + 400;
		Point p(xx, yy);
		points_re.push_back(p);
	}
	
	//OTR a;
	a.Init(points_re);
	
	//a.InitPriQueue();
}

void lines_draw(double t)
{

	//glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glColor3f(0.0, 1.0, t);
	glLineWidth(1);
	glBegin(GL_LINES);
	//Delaunay dt = a.delaunay_temp;

	for (auto eit = a.delaunay_temp.edges_begin(); eit != a.delaunay_temp.edges_end(); eit++)
	{
		//std::cout << "2 " << eit->first << " " << eit->second << std::endl;

		//std::cout << "first " << points[eit->first].hx() << " " << points[eit->first].hy() << std::endl;
		//std::cout << "second " << points[eit->second].hx() << " " << points[eit->second].hy() << std::endl;

		//glBegin(GL_LINE);
		Segment s = a.delaunay_temp.segment(eit);

		Point a = s.point(0);
		Point b = s.point(1);

		glVertex2f(a.x(), a.y());
		glVertex2f(b.x(), b.y());
		//glEnd();

	}
	glEnd();
	glPopMatrix();
}

void points_draw_dt(double t)
{
	//glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	//std::vector <Point>::iterator iter;
	glColor3f(0.0, 1.0, t);
	glPointSize(5);

	//Delaunay dt = a.delaunay_temp;

	glBegin(GL_POINTS);
	for (auto iter = a.delaunay_input.vertices_begin(); iter != a.delaunay_input.vertices_end(); iter++)
	{
		glVertex2f(iter->point().hx(), iter->point().hy());
	}
		
	glEnd();
	glPopMatrix();
	//glutSwapBuffers();
}

void points_draw(double t)
{
	//glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	std::vector <Point>::iterator iter;
	glColor3f(0.0, 1.0, t);
	glPointSize(5);
	glBegin(GL_POINTS);
	for (iter = points_re.begin(); iter != points_re.end(); iter++)
		glVertex2f(iter->hx(), iter->hy());
	glEnd();
	glPopMatrix();
	//glutSwapBuffers();
}

void display(void)
{
	//glClear(GL_COLOR_BUFFER_BIT);
	//RandomInit(NumSeed);
	glClear(GL_COLOR_BUFFER_BIT);
	
	points_draw_dt(0.5);
	//points_draw(0.5);
	lines_draw(0.5);

	a.InitPriQueue();

	//points_draw_dt(1);
	//lines_draw(1);

	glutSwapBuffers();
	//points_triangulation();
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0);
	glShadeModel(GL_FLAT);
}

void reshape(int w, int h)
{
	//global_w = w;
	//global_h = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv)
{
	//border = Polygon_exact(Border_p.begin(), Border_p.end());
	//PrintHelp();
	otr_extrat();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Lloyd");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	//glutKeyboardFunc(keybordClick);
	glutMainLoop();
	return 0;
}