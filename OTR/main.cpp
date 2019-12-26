#include "otr.h"


OTR a;
std::vector<Point> points_1;
std::vector<Point> points_re;
osg::ref_ptr<osg::Group> root = new osg::Group();
osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
osg::ref_ptr<osg::Geometry> geometry_points_1 = new osg::Geometry();
osg::ref_ptr<osg::Geometry> geometry_points_result = new osg::Geometry();
osg::ref_ptr<osg::Geometry> geometry_lines = new osg::Geometry();
osg::ref_ptr<osg::Geode> line_node = new osg::Geode();
osg::ref_ptr<osg::Geode> points_node = new osg::Geode();
void otr_extrat()
{

	ifstream infile;

	infile.open("points_on_edges.xyz", ios::in);
	while (!infile.eof())           
	{
		double x, y,z;
		infile >> x >>y >>z;
		Point p(x, y,z);
		points_1.push_back(p);
		
	}											//读入xy文件

	//CGAL::Random rng(5);
	//Point aa(-10, -10, -10);
	//Point bb(10, 10, 10);
	//Point cc(-10, 10, 10);
	//Point dd(-10, 10, -10);
	//CGAL::Random_points_on_segment_3<Point> point_generator1(aa,bb,rng);			//正方形上
	//CGAL::Random_points_on_segment_3<Point> point_generator2(bb, cc, rng);			//正方形上
	//CGAL::Random_points_on_segment_3<Point> point_generator3(cc, dd, rng);			//正方形上
	//CGAL::Random_points_on_segment_3<Point> point_generator4(dd, aa, rng);			//正方形上
	//CGAL::Random_points_on_segment_3<Point> point_generator5(bb, dd, rng);			//正方形上
	//CGAL::Random_points_on_segment_3<Point> point_generator6(aa, cc, rng);			//正方形上
	////CGAL::Random_points_on_circle_2<Point> point_generator(1., rng);		//圆上
	////CGAL::Random_points_on_square_2<Point> point_generator(1., rng);			//正方形上
	//////CGAL::Random_points_in_square_2<Point> point_generator(1., rng);		//正方形内
	//CGAL::cpp11::copy_n(point_generator1, 50, std::back_inserter(points_1));	//100为生成点的个数
	//CGAL::cpp11::copy_n(point_generator2, 50, std::back_inserter(points_1));	//100为生成点的个数
	//CGAL::cpp11::copy_n(point_generator3, 50, std::back_inserter(points_1));	//100为生成点的个数
	//CGAL::cpp11::copy_n(point_generator4, 50, std::back_inserter(points_1));	//100为生成点的个数
	//CGAL::cpp11::copy_n(point_generator5, 50, std::back_inserter(points_1));	//100为生成点的个数
	//CGAL::cpp11::copy_n(point_generator6, 50, std::back_inserter(points_1));	//100为生成点的个数
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array();
	for (auto iter = points_1.begin(); iter != points_1.end(); iter++)
	{
		double xx = iter->hx() * 8 - 40;
		double yy = iter->hy() * 8 - 40;
		double zz = iter->hz() * 8 - 40;
		Point p(xx, yy,zz);
		points_re.push_back(p);
		coords->push_back(osg::Vec3(p[0], p[1], p[2]));
		color->push_back(osg::Vec4(1, 1, 0, 1.0f));
	}
	geometry_points_1->setVertexArray(coords.get());
	geometry_points_1->setColorArray(color.get());
	geometry_points_1->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	geometry_points_1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, points_re.size()));
	//OTR a;



	a.Init(points_re);
	//a.CaculateAssinCost();
	//a.GetVaild();
	cout << "finish" << endl;
}





void refreashLines()
{
	geometry_lines = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
	int numlines = 0;
	for (auto eit = a.ms2.edges.begin(); eit != a.ms2.edges.end(); eit++)
	{

		osg::Vec3 a(eit->source().x(), eit->source().y(), eit->source().z());
		osg::Vec3 b(eit->target().x(), eit->target().y(), eit->target().z());
		vertexArray->push_back(a);
		vertexArray->push_back(b);
		numlines+=2;
	}
	geometry_lines->setVertexArray(vertexArray);

	// set the colors as before, plus using the above
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	geometry_lines->setColorArray(colors, osg::Array::BIND_OVERALL);
	geometry_lines->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, numlines));
	line_node = new osg::Geode();
	line_node->addDrawable(geometry_lines.get());
	root->addChild(line_node.get());
}

void refreashPointsre()
{
	geometry_points_result = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
	int numpoints = 0;
	for (auto iter = a.ms2.Vertexs.begin(); iter != a.ms2.Vertexs.end(); iter++)
	{
		osg::Vec3 a(iter->x(), iter->y(), iter->z());
		vertexArray->push_back(a);
		numpoints++;
	}

	geometry_points_result->setVertexArray(vertexArray);

	// set the colors as before, plus using the above
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	geometry_points_result->setColorArray(colors, osg::Array::BIND_OVERALL);
	geometry_points_result->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, numpoints));
	points_node = new osg::Geode();
	points_node->addDrawable(geometry_points_result.get());
	root->addChild(points_node.get());
}

class UseEventHandler :public osgGA::GUIEventHandler
{
public:
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
		if (!viewer)return false;

		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::KEYUP:
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Tab)
			{

				viewer->getSceneData()->asGroup()->removeChild(points_node);
				viewer->getSceneData()->asGroup()->removeChild(line_node);
				a.PickAndCollap();
				refreashLines();
				refreashPointsre();
			}
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_F1)
			{
				a.CaculateAssinCost();
				a.GetVaild1();

				a.vertex_points_map_temp.clear();
				a.edge_points_map_temp.clear();

				viewer->getSceneData()->asGroup()->removeChild(points_node);
				viewer->getSceneData()->asGroup()->removeChild(line_node);
				//a.PickAndCollap();
				refreashLines();
				refreashPointsre();
			}
			break;
		}
		default:
			break;
		}
		return false;
	}
};

int main(int argc, char** argv)
{
	otr_extrat();
	
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	
	geode->addDrawable(geometry_points_1.get());
	root->addChild(geode.get());
	refreashLines();
	refreashPointsre();
	
	osg::StateSet* stateSet = root->getOrCreateStateSet();

	osg::Point* point = new osg::Point;                  
	point->setSize(10);
	stateSet->setAttribute(point);

	osg::LineWidth* lineWidth = new osg::LineWidth;
	lineWidth->setWidth(5);
	stateSet->setAttribute(lineWidth);

	root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	viewer->setSceneData(root.get());
	viewer->addEventHandler(new UseEventHandler());
	viewer->run();
	return viewer->run();
}