// this file is part of homog2d
// used to build a figure that is included in manual
// See makefile target doc_fig
// Requires OpenCv

#include "fig_src.header"


void mult( std::vector<Point2d>& vec )
{
	std::for_each( vec.begin(), vec.end(), [](Point2d& pt){ pt=Point2d(pt.getX()*10,pt.getY()*10); } );
}

int main( int argc, const char** argv )
{
	return 0;

	std::cout << "START: " << argv[0] << '\n';
	Point2d pt0;

	char sep= ';';
	int c = 0;
	std::vector<Point2d> vpt{
		{+10,0}, {-10,0}, {0,+10}, {0,-10},
		{+5,+5}, {-5,+5}, {+5,-5}, {-5,-5}
	 };

	std::vector<Point2d> vpt_li{
		{+10,+5}, {+5,+10}, {+10,-5}, {+5,-10},
		{-10,+5}, {-5,+10}, {-10,-5}, {-5,-10}
	};

	mult(vpt);
	mult(vpt_li);
	std::string winName("cornerside");
	cv::namedWindow( winName );

	Homogr H( 150,150);
	uint8_t g = 200;
	auto colg = img::DrawParams().setColor(g,g,g);
	auto col1 = img::DrawParams().setColor(250,0,0);
	auto col2 = img::DrawParams().setColor(0,250,0);

	std::ofstream f( "cornerside2.csv" );

	for( size_t ipt1=0; ipt1<vpt_li.size(); ipt1++ )
	{
		auto pt1 = vpt_li[ipt1];
		for( size_t ipt2=ipt1+1; ipt2<vpt_li.size(); ipt2++ )
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
					for( size_t i=0; i<vpt.size(); i++ )
					{
						auto ptA = vpt[i];
						for( size_t j=i+1; j<vpt.size(); j++ )
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
								cv::putText( im.getReal(), id.str(), cv::Point2i( 20, 40), 0, 0.6, cv::Scalar( 150,0,0 ), 1 );
								cv::putText( im.getReal(), std::to_string(c), cv::Point2i( 20, 80), 0, 0.6, cv::Scalar( 150,0,0 ), 1 );

								std::ostringstream oss;
								oss << "pointcross_" << std::setfill('0') << std::setw(2) << id.str() << ".png";

								im.write( oss.str() );
								im.show( winName );
								auto key = cv::waitKey(0);

							f << c++ << sep << id.str() << sep
								<< s12 << s21
								<< (side( ptA, li1 )==-1?0:1)
								<< (side( ptB, li1 )==-1?0:1)
								<< (side( ptA, li2 )==-1?0:1)
								<< (side( ptB, li2 )==-1?0:1)
								<< sep << (char)key
								<< std::endl;
							}
						}
					}
			}
	}
}

