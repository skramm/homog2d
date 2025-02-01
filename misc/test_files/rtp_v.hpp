/**
Variant-based polymorphism

ONLY TO BE USED WITH THE ASSOCIATED PROGRAMS
\c homog2d_test_rtp_*.cpp
*/

template<typename IM>
void
do_vrtp( const std::vector<h2d::Point2d>& vecpts, IM& im )
{
	CPolyline cpol( vecpts );
	OPolyline opol( vecpts );

	std::vector<CommonType> vec;

	vec.push_back( Circle()  );
	vec.push_back( Segment() );
	vec.push_back( FRect()   );
	vec.push_back( Line2d()  );
	vec.push_back( Point2d() );
	vec.push_back( Ellipse() );
	vec.push_back( cpol      );
	vec.push_back( opol      );

	fct::DrawFunct vde( im );
	auto h = Homogr().addTranslation(3,3).addScale(15);
	for( auto& e: vec )
	{
		std::cout << getString(type(e))
			<< "\n -area=" << area(e)
			<< "\n -size=" << size(e);
		if( type(e) != Type::Line2d )
			std::cout <<  "\n -length=" << length(e);
		else
		{
			Line2d li = fct::VariantUnwrapper{e};
			std::cout << "\n -length=N/A (li=" << li << ')';
		}

		std::cout << "\n- data type=" << getString(dtype(e)) << '\n';

		e = transform( h, e );
		std::visit( vde, e );              // then draw
	}
}
