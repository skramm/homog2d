/**
\file
\brief Demo of point inside polygon (PIP) algorithm
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "fig_src.header"

using namespace h2d;

template<typename P,typename BB, typename PT, typename IMG>
void drawBase( const P& poly, const BB& bb, const PT& pt, const Segment* seg, IMG& im )
{
	im.clear();
	poly.draw( im, DrawParams().setColor(250,0,20)  );
	bb.draw( im, DrawParams().setColor(0,0,250)  );
	pt.draw( im, DrawParams().setColor(0,250,0)  );
	if( seg )
	{
		seg->draw( im, DrawParams().setColor(0,0,250)  );
		auto inter = seg->intersects( poly );
		inter.get()[0].draw( im, DrawParams().setColor(0,250,0).setPointStyle(PtStyle::Dot).setWidth(3) );
	}
}

int main()
{
//	auto nbim = 20; // nb images

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
	CPolyline pol( vpts1 );


// considered point is exactly in the center of the polygon
	Point2d pt(5,5);

	auto H = Homogr().addTranslation(20,20).addScale(8);
	auto poly = H*pol;
	auto pt0 = H * pt;
	auto bbe = getBB( poly ).getExtended();

	img::Image<img::SvgImage> im( 350,350 );

	drawBase( poly, bbe, pt0, nullptr, im );

	im.write( "demo_pip_0.svg" );
	auto pts = bbe.get4Pts();

// step 1: show that taking segments based on the 4 points
// of the extended BB does not meet the requirements
// (the segments will intersect the polygon on one of its points)
	for( int i=0; i<4; i++ )
	{
		auto seg = Segment( pts[i], pt0 );
		drawBase( poly, bbe, pt0, &seg, im );

		std::ostringstream oss;
		oss << "demo_pip_" << i+1 << ".svg";
		im.write( oss.str() );
	}

// step 2: show that taking segments based on the middle point of the
// 4 segments of the extended BB does not work either
	auto segs = bbe.getSegs();
	for( int i=0; i<4; i++ )
	{
		auto ptmid = segs[i].getCenter();
		auto seg = Segment( ptmid,  pt0 );
		drawBase( poly, bbe, pt0, &seg, im );

		std::ostringstream oss;
		oss << "demo_pip_" << i+5 << ".svg";
		im.write( oss.str() );
	}

// final step: taking the middle point of half the above segments does
// generate a segment that does not intersect the polygon on one of its points

}

