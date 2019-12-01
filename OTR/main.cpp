#include "otr.h"


OTR a;
std::vector<Point> points_1;
std::vector<Point> points_re;

void otr_extrat()
{

	ifstream infile;

	infile.open("noise2.xy", ios::in);
	while (!infile.eof())           
	{
		double x, y;
		infile >> x >>y;
		Point p(x, y);
		points_1.push_back(p);
		
	}											//读入xy文件

	//CGAL::Random rng(3);
	////CGAL::Random_points_on_circle_2<Point> point_generator(1., rng);		//圆上
	//CGAL::Random_points_on_square_2<Point> point_generator(1., rng);			//正方形上
	//////CGAL::Random_points_in_square_2<Point> point_generator(1., rng);		//正方形内
	//CGAL::cpp11::copy_n(point_generator, 100, std::back_inserter(points_1));	//100为生成点的个数

	for (auto iter = points_1.begin(); iter != points_1.end(); iter++)
	{
		double xx = iter->hx() *30+200;
		double yy = iter->hy() * 30 + 200;
		Point p(xx, yy);
		points_re.push_back(p);
	}
	
	//OTR a;
	a.Init(points_re);
	
}

void lines_draw(double t)	//画tgl2中的线
{

	//glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glColor3f(0.0, 1.0, t);
	glLineWidth(1);
	
	int kk = 0;

	glBegin(GL_LINES);
	//Delaunay dt = a.delaunay_temp;


	for (auto eit = a.tgl1.edges_begin(); eit != a.tgl1.edges_end(); eit++)
	{
		vertex_handle start_p = eit->first->vertex(a.tgl1.cw(eit->second));
		vertex_handle end_p = eit->first->vertex(a.tgl1.ccw(eit->second));

		glVertex2f(start_p->point().x(), start_p->point().y());
		glVertex2f(end_p->point().x(), end_p->point().y());


	}
	glEnd();


	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	//Delaunay dt = a.delaunay_temp;

	for (auto eit = a.tgl2.edges_begin(); eit != a.tgl2.edges_end(); eit++)
	{
		vertex_handle start_p = eit->first->vertex(a.tgl2.ccw(eit->second));
		vertex_handle end_p = eit->first->vertex(a.tgl2.cw(eit->second));

		glVertex2f(start_p->point().x(), start_p->point().y());
		glVertex2f(end_p->point().x(), end_p->point().y());


	}
	glEnd();




	glColor3f(0.5, 0, 0.5);
	glBegin(GL_LINES);

	for (auto eit = a.block_edge.begin(); eit != a.block_edge.end(); eit++)
	{


		glVertex2f(eit->first->point().x(), eit->first->point().y());
		glVertex2f(eit->second->point().x(), eit->second->point().y());
		//glEnd();

	}


	glEnd();
	glPopMatrix();
}

void points_draw_dt(double t)	//绘制三角网格上的点(tgl2)
{
	//glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	//std::vector <Point>::iterator iter;
	glColor3f(0.0, 1.0, t);
	glPointSize(5);

	//Delaunay dt = a.delaunay_temp;

	glBegin(GL_POINTS);
	for (auto iter = a.tgl2.vertices_begin(); iter != a.tgl2.vertices_end(); iter++)
	{
		glVertex2f(iter->point().hx(), iter->point().hy());
	}
		
	glEnd();
	glPopMatrix();
	//glutSwapBuffers();
}

void points_draw(double t)	//绘制生成的原始点
{
	//glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	std::vector <Point>::iterator iter;
	glColor3f(0.0, 1.0, t);
	glPointSize(3);
	glBegin(GL_POINTS);
	for (iter = points_re.begin(); iter != points_re.end(); iter++)
		glVertex2f(iter->hx(), iter->hy());
	glEnd();
	glPopMatrix();
	//glutSwapBuffers();
}

void assin_draw()		//绘制所有被分配到顶点的点
{
	glPushMatrix();
	//std::vector <Point>::iterator iter;
	glColor3f(0.2, 0.6, 0.1);
	glPointSize(5);
	glBegin(GL_POINTS);

	for (auto iter=a.vertex_points_map.begin();iter!=a.vertex_points_map.end();iter++)
	{
		_Cost cost = iter->second;
		for (auto pit=cost.assined_points.begin();pit!=cost.assined_points.end();pit++)
		{
			glVertex2f(pit->x(),pit->y());
		}
		
	}
	glEnd();
	glPopMatrix();
}

void display(void)
{
	//glClear(GL_COLOR_BUFFER_BIT);
	//RandomInit(NumSeed);
	glClear(GL_COLOR_BUFFER_BIT);
	
	//points_draw_dt(0.5);
	points_draw(0.5);
	

	a.InitPriQueue();
	a.PickAndCollap();
	lines_draw(0.5);
	points_draw_dt(1);
	//lines_draw(1);

	glutSwapBuffers();
	//points_triangulation();
}

void keybordClick(unsigned char key, int x, int y)
{
	if (key == 's')
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		a.PickAndCollap();
		points_draw(0.5);
		lines_draw(0.5);
		points_draw_dt(1);
		assin_draw();
		glPopMatrix();
		glutSwapBuffers();
	}
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
	glutKeyboardFunc(keybordClick);
	glutMainLoop();
	return 0;
}