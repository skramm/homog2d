/**
\file
\brief Demo of point inside polygon algorithm
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "fig_src.header"

using namespace h2d;

template<typename P,typename BB, typename PT, typename IMG>
void drawBase( const P& poly, const BB& bb, const PT& pt, IMG& im )
{
	im.clear();
	poly.draw( im, DrawParams().setColor(250,0,20)  );
	bb.draw( im, DrawParams().setColor(0,0,250)  );
	pt.draw( im, DrawParams().setColor(0,250,0)  );
}

int main()
{
	auto nbim = 20; // nb images

	std::vector<Point2d> vpts1{
		{ 0, 0 },
		{ 5, 0 },
		{ 6, 1 },
		{ 7, 0 },
		{ 10, 0 },

		{ 10, 5 },
		{ 9, 6 },
		{ 10, 7 },
		{ 10, 10 },

		{ 0, 10 }
	};

	Point2d pt(5,5);

	CPolyline pol( vpts1 );

	auto H = Homogr().addTranslation(10,10).addScale(8);
	auto poly = H*pol;
	auto pt0 = H * pt;
	auto bb = getBB( poly );
	auto bbe = bb.getExtended();

	img::Image<img::SvgImage> im( 350,250 );

	drawBase( poly, bbe, pt0, im );

	im.write( "demo_pip_0.svg" );
	for( int i=0; i<4; i++ )
	{
		drawBase( poly, bbe, pt0, im );
		auto seg = Segment( bbe.get4Pts()[i], pt0 );
		seg.draw( im, DrawParams().setColor(0,0,250)  );
		std::ostringstream oss;
		oss << "demo_pip_" << i << ".svg";
		im.write( oss.str() );
	}

	auto segs = bbe.getSegs();
	for( int i=0; i<4; i++ )
	{
		drawBase( poly, bbe, pt0, im );
		auto ptmid = segs[i].getCenter();
		auto seg = Segment( ptmid,  pt0 );
		seg.draw( im, DrawParams().setColor(0,0,250)  );
		std::ostringstream oss;
		oss << "demo_pip_" << i+4 << ".svg";
		im.write( oss.str() );
	}

}

