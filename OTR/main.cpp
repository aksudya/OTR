#include "otr.h"


OTR a;
std::vector<Point> points_1;
std::vector<Point> points_re;

void otr_extrat()
{

	ifstream infile;

/*	infile.open("stair-noise00.txt", ios::in);
	while (!infile.eof())           
	{
		double x, y,z;
		infile >> x >>y >>z;
		Point p(x, y,z);
		points_1.push_back(p);
		
	}	*/										//读入xy文件

	CGAL::Random rng(3);
	Point aa(-10, -10, 10);
	Point bb(10, 10, 5);
	Point cc(-10, 7, 7);
	Point dd(10, 6, -10);
	CGAL::Random_points_on_segment_3<Point> point_generator1(aa,bb,rng);			//正方形上
	CGAL::Random_points_on_segment_3<Point> point_generator2(bb, cc, rng);			//正方形上
	CGAL::Random_points_on_segment_3<Point> point_generator3(cc, dd, rng);			//正方形上
	CGAL::Random_points_on_segment_3<Point> point_generator4(dd, aa, rng);			//正方形上
	CGAL::Random_points_on_segment_3<Point> point_generator5(bb, dd, rng);			//正方形上
	//CGAL::Random_points_on_circle_2<Point> point_generator(1., rng);		//圆上
	//CGAL::Random_points_on_square_2<Point> point_generator(1., rng);			//正方形上
	////CGAL::Random_points_in_square_2<Point> point_generator(1., rng);		//正方形内
	CGAL::cpp11::copy_n(point_generator1, 100, std::back_inserter(points_1));	//100为生成点的个数
	CGAL::cpp11::copy_n(point_generator2, 100, std::back_inserter(points_1));	//100为生成点的个数
	CGAL::cpp11::copy_n(point_generator3, 100, std::back_inserter(points_1));	//100为生成点的个数
	CGAL::cpp11::copy_n(point_generator4, 100, std::back_inserter(points_1));	//100为生成点的个数
	CGAL::cpp11::copy_n(point_generator5, 100, std::back_inserter(points_1));	//100为生成点的个数

	for (auto iter = points_1.begin(); iter != points_1.end(); iter++)
	{
		double xx = iter->hx() *3+0;
		double yy = iter->hy() * 3 +0;
		double zz = iter->hy() * 3 + 0;
		Point p(xx, yy,zz);
		points_re.push_back(p);
	}
	
	//OTR a;
	a.Init(points_re);
	//a.CaculateAssinCost();
	//a.GetVaild();
	cout << "finish" << endl;
}

void lines_draw(double t)	//画tgl2中的线
{

	//glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glColor3f(0.0, 1.0, t);
	glLineWidth(2);
	
	int kk = 0;


	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	//Delaunay dt = a.delaunay_temp;

	for (auto eit = a.ms2.edges.begin(); eit != a.ms2.edges.end(); eit++)
	{


		glVertex3f(eit->source().x(),eit->source().y(), eit->source().z());
		glVertex3f(eit->target().x(), eit->target().y(), eit->target().z());


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
	for (auto iter = a.ms2.Vertexs.begin(); iter != a.ms2.Vertexs.end(); iter++)
	{
		glVertex3f(iter->x(), iter->y(),iter->z());
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
		glVertex3f(iter->hx(), iter->hy(),iter->hz());
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
			glVertex3f(pit->x(),pit->y(), pit->z());
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
	
	points_draw_dt(0.5);
	points_draw(0.5);
	//assin_draw();

	//a.InitPriQueue();
	//a.PickAndCollap();
	lines_draw(0.5);
	//points_draw_dt(1);
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
		assin_draw();
		lines_draw(0.5);
		points_draw_dt(1);
		
		glPopMatrix();
		glutSwapBuffers();
	}
	else if(key=='t')
	{
		a.CaculateAssinCost();
		a.GetVaild1();

		a.vertex_points_map_temp.clear();
		a.edge_points_map_temp.clear();
		glClear(GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		//a.PickAndCollap();
		points_draw(0.5);
		assin_draw();
		lines_draw(0.5);
		points_draw_dt(1);

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
	glOrtho(-100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f);
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
	glOrtho(-100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keybordClick);
	glutMainLoop();
	return 0;
}