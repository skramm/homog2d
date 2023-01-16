// this file is part of homog2d
// used to build a figure that is included in manual
// See makefile target doc_fig
// Requires OpenCv

#include "fig_src.header"


void mult( std::vector<Point2d>& vec )
{
	std::for_each( vec.begin(), vec.end(), [](Point2d& pt){ pt=Point2d(pt.getX()*10,pt.getY()*10); } );
}

int main()
{
	return 0;
	Point2d pt0;

	char sep= ';';
	int c = 0;
	std::vector<Point2d> vpt{ {+10,0}, {-10,0}, {0,+10}, {0,-10} };

	std::vector<Point2d> vpt_li{
		{+10,+5}, {+5,+10}, {+10,-5}, {+5,-10},
		{-10,+5}, {-5,+10}, {-10,-5}, {-5,-10}
	};

	mult(vpt);
	mult(vpt_li);

	Homogr H( 150,150);
	uint8_t g = 200;
	auto colg = img::DrawParams().setColor(g,g,g);
	auto col1 = img::DrawParams().setColor(250,0,0);
	auto col2 = img::DrawParams().setColor(0,250,0);

	for( auto ipt1=0; ipt1<vpt_li.size(); ipt1++ )
	{
		auto pt1 = vpt_li[ipt1];
		for( auto ipt2=0; ipt2<vpt_li.size(); ipt2++ )
			if( ipt1 != ipt2)
			{
				auto pt2 = vpt_li[ipt2];
//				std::cout << pt0 << "-" << pt1 << "\n";
//				std::cout << pt0 << "-" << pt2 << "\n";
				Segment seg1(pt0, pt1);
				Segment seg2(pt0, pt2);
				auto li1 = seg1.getLine();
				auto li2 = seg2.getLine();
				auto s12 = (side( pt1, li2 )==-1 ? 0 : 1);
				auto s21 = (side( pt2, li1 )==-1 ? 0 : 1);

				if( li1 != li2 )
					for( auto i=0; i<4; i++ )
					{
						auto ptA = vpt[i];
						for( auto j=0; j<4; j++ )
						{
							if( i != j )
							{
								auto ptB = vpt[j];
								img::Image<cv::Mat> im( 300,300 );
								auto seg1d = H*seg1;
								auto seg2d = H*seg2;
								auto li1d = H*li1;
								auto li2d = H*li2;
								auto ptAd = H*ptA;
								auto ptBd = H*ptB;

								draw( im, li1d, colg );
								draw( im, li2d, colg );
								draw( im, seg1d, col1 );
								draw( im, seg2d, col2 );
								draw( im, ptAd );
								draw( im, ptBd );
								std::ostringstream id;
								id << ipt1 << ipt2 << i << j;

							std::cout << c++ << sep << id.str() << sep
								<< s12 << s21
								<< (side( ptA, li1 )==-1?0:1)
								<< (side( ptB, li1 )==-1?0:1)
								<< (side( ptA, li2 )==-1?0:1)
								<< (side( ptB, li2 )==-1?0:1)
	//							<< area( ptA, ptB, pt0 )
								<< '\n';

								std::ostringstream oss;
								oss << "pointcross_" << std::setfill('0') << std::setw(2) << id.str() << ".png";

								im.write( oss.str() );
							}
						}
					}

//				for( auto k=0; k<2; k++ )
			}
	}


/*	Segment s1( p0, Point2d(x12[i],y12[j] );
	Segment s2( pb1, pb2 );

}
	img::Image<cv::Mat> img( 350,250 );
	s1.draw( img, DrawParams().setColor(250,50,80).setThickness(2) );
//	s2.draw( img, DrawParams().setColor(50,250,80).setThickness(2) );

	auto l1 = s1.getLine();
	auto l2 = s2.getLine();

	l1.draw( img );
	l2.draw( img );

	pa1.draw( img, DrawParams().setPointStyle(img::PtStyle::Dot).setColor(250,50,80).setThickness(4) );
	pa2.draw( img, DrawParams().setPointStyle(img::PtStyle::Dot).setColor(250,50,80).setThickness(4) );
	auto inter = l1 * l2;
	inter.draw( img, DrawParams().setPointStyle(img::PtStyle::Dot).setColor(50,50,250) );

	cv::putText( img.getReal(), "a", pa1.getCvPti(),   0, 1.0, cv::Scalar( 20,20,20 ), 2 );
	cv::putText( img.getReal(), "b", pa2.getCvPti(),   0, 1.0, cv::Scalar( 20,20,20 ), 2 );
	cv::putText( img.getReal(), "c", inter.getCvPti(), 0, 1.0, cv::Scalar( 20,20,20 ), 2 );

	img.write( "segment_intersect_1.png" );
	*/
}
