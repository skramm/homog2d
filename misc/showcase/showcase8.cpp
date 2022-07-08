/**
\file
\brief Generates rotating point and draw the orthogonal segment to a line
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	auto nbim = 25; // nb images
	auto Hdraw = Homogr().addTranslation(3,2).addScale(40);

	auto k=1.5;
	for( int i=0; i<nbim; i++ )
	{
		auto angle = i*360./nbim;
		auto angle_r = angle * M_PI / 180.;
		auto x1 = std::cos(angle_r)*k;
		auto y1 = std::sin(angle_r)*k;
		Point2d pt( x1,y1);
		auto pt_d = Hdraw * pt;

		Segment seg( -.7, -.2, +.8, +.3 );
		auto li = seg.getLine();
		auto li_d = Hdraw * li;

		Segment seg2( -.9, +1.4, +.45, +1.0 );
		auto seg2_d = Hdraw * seg2;

		img::Image<cv::Mat> im( 250, 200 );
		pt_d.draw(   im, img::DrawParams().setColor(250,0,0) );
		li_d.draw(   im, img::DrawParams().setColor(0,250,0) );
		seg2_d.draw( im, img::DrawParams().setColor(0,250,0) );

		auto sego = li_d.getOrthogSegment( pt_d );
		sego.draw( im, img::DrawParams().setColor(20,0,250) );

		int segDistCase;
		auto dist = seg2_d.distTo( pt_d, &segDistCase );
		auto pts_seg2 = seg2_d.getPts();

		auto colA = img::DrawParams().setColor(200,200,0);
		auto colB = img::DrawParams().setColor(0,200,200);
		switch( segDistCase )
		{
			case -1:
				Segment( pt_d, pts_seg2.first ).draw( im, colA );
			break;
			case +1:
				Segment( pt_d, pts_seg2.second ).draw( im, colA );
			break;
			default:
				auto s = seg2_d.getLine().getOrthogSegment( pt_d );
				s.draw( im, colB );
		}

		std::ostringstream oss;
		oss << "showcase8_" << std::setfill('0') << std::setw(2) <<i << ".png";
		im.write( oss.str() );
	}
}

