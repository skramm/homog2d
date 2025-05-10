// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc-fig


#include "fig_src.header"

void drawGrid( img::Image<img::SvgImage>& im, const Homogr& H )
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

struct SplitDemo
{
	Line2d li;
	Point2d p1;
	Point2d p2;
	Point2d p3;
	SplitDemo()
	{
		li=Line2d( 2, 4, 6, 2 );
	}
};

template<typename T>
void process( std::string fname, const T& vpts, const SplitDemo& sp )
{
	OPolyline op(vpts);
	img::Image<img::SvgImage> im( 300, 200 );
	auto H = Homogr().addScale(30).addTranslation(10,30);
//	drawGrid( im, H );

	auto dp = DrawParams().setThickness(2);
	(H*op).draw( im, dp.setColor(150,100,50) );
	(H*sp.li).draw( im, dp.setColor(50,100,150) );
	dp = dp.setColor(200,0,0).setPointStyle(PtStyle::Dot).setPointSize(5).setThickness(1);

	im.drawText( "p1", H*sp.p1, DrawParams().setColor(0,0,50) );
	im.drawText( "p2", H*sp.p2, DrawParams().setColor(0,0,50) );
	im.drawText( "p3", H*sp.p3, DrawParams().setColor(0,0,50) );

	(H*sp.p1).draw( im, dp );
//	(H*sp.p2).draw( im, dp );
	(H*sp.p3).draw( im, dp );

	im.write( fname );
}

int main()
{
	auto H = Homogr().addScale(30).addTranslation(10,30);

	SplitDemo sp;
	sp.p1 = Point2d(1,1);
	sp.p2 = Point2d(4,3);
	sp.p3 = Point2d(3,0);

	std::vector<Point2d> vpts1{ {0,2},sp.p1,sp.p2,sp.p3,{5,1} };
	process( "polyline_split_A.svg", vpts1, sp );

	sp.p3 = Point2d(6,4);
	std::vector<Point2d> vpts2{ {0,2},sp.p1,sp.p2,sp.p3,{8,3} };
	process( "polyline_split_B.svg", vpts2, sp );

	sp.p3 = Point2d(6,2);
	std::vector<Point2d> vpts3{ {0,2},sp.p1,sp.p2,sp.p3,{8,3} };
	process( "polyline_split_C.svg", vpts3, sp );
}
