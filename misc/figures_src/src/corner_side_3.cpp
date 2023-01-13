// this file is part of homog2d
// used to build a figure that is included in manual
// See makefile target doc_fig
// Requires OpenCv

#include "fig_src.header"


void mult( std::vector<Point2d>& vec )
{
	std::for_each( vec.begin(), vec.end(), [](Point2d& pt){ pt=Point2d(pt.getX()*10,pt.getY()*10); } );
}

	Homogr H( 150,150);
	uint8_t g = 200;
	auto colg = img::DrawParams().setColor(g,g,g);
	auto col1 = img::DrawParams().setColor(250,0,0);
	auto col2 = img::DrawParams().setColor(0,250,0);

struct Data
{
	Line2d li1, li2;
	Point2d pt1, pt2;
	Point2d ptA, ptB;
	void swapAB()
	{
		std::swap( ptA, ptB );
	}
	void swap12()
	{
		std::swap( pt1, pt2 );
		buildLines();
	}
	void buildLines()
	{
		Segment seg1(Point2d(), pt1);
		Segment seg2(Point2d(), pt2);
		li1 = seg1.getLine();
		li2 = seg2.getLine();
	}
	bool linesEqual()
	{
		return li1 == li2;
	}
	void drawData( img::Image<cv::Mat>& im ) const
	{
		Segment seg1(Point2d(), pt1);
		Segment seg2(Point2d(), pt2);

		auto seg1d = H*Segment(Point2d(), pt1);
		auto seg2d = H*Segment(Point2d(), pt2);
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
	}
};

std::string BuildBinaryString( const Data& data )
{
	Point2d pt0;
	auto sA1 = side( data.ptA, data.li1 )==-1?0:1;
	auto sB1 = side( data.ptB, data.li1 )==-1?0:1;
	auto sA2 = side( data.ptA, data.li2 )==-1?0:1;
	auto sB2 = side( data.ptB, data.li2 )==-1?0:1;

// side correction
	auto side_corr_1 = ( data.pt1.getY() < pt0.getY() ? 1 : 0 );
	auto side_corr_2 = ( data.pt2.getY() < pt0.getY() ? 1 : 0 );
	auto sA1c = side_corr_1 ? sA1 : 1-sA1;
	auto sA2c = side_corr_2 ? sA2 : 1-sA2;
	auto sB1c = side_corr_1 ? sB1 : 1-sB1;
	auto sB2c = side_corr_2 ? sB2 : 1-sB2;

	auto s12 = (side( data.pt1, data.li2 )==-1 ? 0 : 1);
	auto s21 = (side( data.pt2, data.li1 )==-1 ? 0 : 1);

	char q = '"';
	std::ostringstream oss;
	oss << q << s12 << s21<< q << ';'
		<< q << sA1c << sA2c << sB1c << sB2c << q << ';'
		<< q << sA1 << sA2 << sB1 << sB2 << q;
	return oss.str();
}

/// Compute quadrant of the point [1-4] (trigonometric notation)
/**
\note In the real situation, this needs to be done relatively to reference point
*/
int getQuadrant( Point2d pt )
{
	if( pt.getX() >= 0 )
	{
		if( pt.getY() >= 0 )
			return 1;
		return 4;
	}
	if( pt.getY() >= 0 )
			return 2;
	return 3;
}

std::string getQuadrantStr(const Data& data )
{
	std::ostringstream oss;
	oss
		<< getQuadrant( data.pt1 )
		<< getQuadrant( data.pt2 )
		<< getQuadrant( data.ptA )
		<< getQuadrant( data.ptB );
	return oss.str();
}

void output( std::ofstream&f, int c1, int c2, const Data& data, char key )
{
	char sep = ';';
	f << c1 << sep << c2 << sep
//		<< data.pt1 << sep << data.pt2 << sep
//		<< data.ptA << sep << data.ptB << sep
		<< getQuadrantStr(data) << sep
		<< BuildBinaryString( data ) << sep << key
		<< std::endl;
}

void processSwap( std::ofstream& f, const Data& data, char key, int c1 )
{
	int c2 = 0;
	output( f, c1, c2++, data, key );

	auto data2 = data;
	data2.swapAB();
	output( f, c1, c2++, data2, key );

	auto data3 = data;
	data3.swap12();
	output( f, c1, c2++, data3, key );

	auto data4 = data;
	data4.swap12();
	data4.swapAB();
	output( f, c1, c2++, data4, key );
}

int main( int argc, const char** argv )
{
	std::cout << "START: " << argv[0] << '\n';
	Point2d pt0;

	char sep= ';';
	int c = 0;
#if 0
	std::vector<Point2d> vpt{
		{+10,0}, {-10,0}, {0,+10}, {0,-10},
		{+5,+5}, {-5,+5}, {+5,-5}, {-5,-5}
	 };

	std::vector<Point2d> vpt_li{
		{+10,+5}, {+5,+10}, {+10,-5}, {+5,-10},
		{-10,+5}, {-5,+10}, {-10,-5}, {-5,-10}
	};
#else
//                   quadrant     1         2
	std::vector<Point2d> vpt{ {+10,+1}, {-10,+1}, {-1,-10}, {-1,+10} };
	std::vector<Point2d> vpt_li{ {+10,+10}, {-10,+10}, {+10,-10}, {-10,-10} };
#endif

	for_each( vpt.begin(), vpt.end(), [](Point2d pt){ std::cout << "quadrant " << pt << "=" << getQuadrant(pt) << '\n';} );
	mult(vpt);
	mult(vpt_li);
	std::string winName("cornerside");
	cv::namedWindow( winName );

	std::ofstream f( "cornerside3.csv" );
	f << "# c1;c2;Q;s12-s21;idc;id;K;dec1;dec2\n";
	Data data;
	for( size_t ipt1=0; ipt1<vpt_li.size(); ipt1++ )
	{
		data.pt1 = vpt_li[ipt1];
		for( size_t ipt2=ipt1+1; ipt2<vpt_li.size(); ipt2++ )
			if( ipt1 != ipt2)
			{
				data.pt2 = vpt_li[ipt2];
				data.buildLines();
				if( !data.linesEqual() )
					for( size_t i=0; i<vpt.size(); i++ )
					{
						data.ptA = vpt[i];
						for( size_t j=i+1; j<vpt.size(); j++ )
						{
							if( i != j )
							{
								data.ptB = vpt[j];
								img::Image<cv::Mat> im( 300,300 );
								data.drawData( im );

								std::ostringstream id;
								id << ipt1 << ipt2 << i << j;
								cv::putText( im.getReal(), id.str(), cv::Point2i( 20, 40), 0, 0.6, cv::Scalar( 150,0,0 ), 1 );
								cv::putText( im.getReal(), std::to_string(c), cv::Point2i( 20, 80), 0, 0.6, cv::Scalar( 150,0,0 ), 1 );

								std::ostringstream oss;
								oss << "pointcross_" << std::setfill('0') << std::setw(2) << id.str() << ".png";

								im.write( oss.str() );
								im.show( winName );
								auto key = cv::waitKey(0);
								std::cout << "key=" << (char)key << '\n';
								processSwap( f, data, key, c++ );
							}
						}
					}
			}
	}
}

