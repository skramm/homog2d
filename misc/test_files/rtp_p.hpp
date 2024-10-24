/**
Pointer-based polymorphism

ONLY TO BE USED WITH THE ASSOCIATED PROGRAMS
\c homog2d_test_rtp_*.cpp
*/

template<typename IM>
void
do_prtp( const std::vector<h2d::Point2d>& vecpts, IM& im )
{
	CPolyline cpol( vecpts );
	OPolyline opol( vecpts );

	std::vector<std::shared_ptr<rtp::Root>> vec;

	vec.push_back( std::make_shared<Circle>(   Circle()  ) );
	vec.push_back( std::make_shared<Segment>(  Segment() ) );
	vec.push_back( std::make_shared<FRect>(    FRect()   ) );
	vec.push_back( std::make_shared<Line2d>(   Line2d()  ) );
	vec.push_back( std::make_shared<Point2d>(  Point2d() ) );
	vec.push_back( std::make_shared<Ellipse>(  Ellipse() ) );
	vec.push_back( std::make_shared<CPolyline>( cpol     ) );
	vec.push_back( std::make_shared<OPolyline>( opol     ) );

	for( auto& e: vec )  // demonstration of polymorphic member functions
	{
		std::cout << getString(e->type()) << ": " << *e
			<< "\n  -area = " << e->area();
		if( e->type() != Type::Line2d )
			std::cout << "\n -length = " << e->length();
		else
			std::cout << "\n -length = infinite";
		std::cout << '\n';
		e->draw( im );

		if( e->type() == Type::CPolyline )
		{
			auto pl1 = std::dynamic_pointer_cast<CPolyline>( e );
			std::cout << "pl1 is closed=" << pl1->isClosed() << '\n';

// demonstration of bad casting, this will segfault
/*
			auto pl2 = std::dynamic_pointer_cast<OPolyline>( e );
			std::cout << "pl2 is closed=" << pl2->isClosed() << '\n';
			std::cout << "pl2 area=" << pl2->area() << '\n';
*/
		}
	}
}
