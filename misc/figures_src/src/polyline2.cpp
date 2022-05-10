// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig


#include "fig_src.header"

void drawGrid( img::Image<cv::Mat>& im, const Homogr& H )
{
	int g=220, g0=g/2;
	auto colgrid  = img::DrawParams().setColor(g,g,g);
	auto colgrid0 = img::DrawParams().setColor(g0,g0,g0).setThickness(2);
	for (int i=0; i<5; i++ )
	{
		Line2d li( LineDir::H, i );
		auto li2 = H*li;
		if( i==0 )
			li2.draw( im, colgrid0 );
		else
			li2.draw( im, colgrid );
	}
	for (int i=0; i<4; i++ )
	{
		Line2d li( LineDir::V, i );
		auto li2 = H*li;
		if( i==0 )
			li2.draw( im, colgrid0 );
		else
			li2.draw( im, colgrid );
	}
}

template<typename T,typename U>
void generate( std::string fname, const base::PolylineBase<T,U>& pl )
{
	img::Image<cv::Mat> im( 200, 200 );
	auto H = Homogr().setScale(30).addTranslation(10,30);
	drawGrid( im, H );
	auto pl2 = H * pl;
	pl2.draw( im, DrawParams().setColor(250,0,20).showPoints(true) );
	im.write( fname );
}

int main()
{
	std::vector<Point2d> vpts1{ {1,1},{3,4},{1,3} };
	std::vector<Point2d> vpts2{ {3,4},{1,3},{1,1} };

	OPolyline op1(vpts1);
	OPolyline op2(vpts2);

	CPolyline cp1(vpts1);
	auto H = Homogr().setScale(30).addTranslation(10,30);
	generate( "polyline2_o1.png", op1 );
	generate( "polyline2_o2.png", op2 );
	generate( "polyline2_c.png", cp1 );
}
