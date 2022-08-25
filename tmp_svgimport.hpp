// tmp_svgimport.hpp

// https://www.w3.org/TR/SVG/paths.html
#include "tinyxml2.h"


namespace h2d {
namespace svg {

void printFileAttrib( const tinyxml2::XMLDocument& doc )
{
	const tinyxml2::XMLElement* root = doc.RootElement();

	const tinyxml2::XMLAttribute* pAttrib = root->FirstAttribute();
	while(pAttrib)
	{
		std::cout << "Value: " << pAttrib->Name() << ":" << pAttrib->Value() << '\n';
		pAttrib=pAttrib->Next();
	}
}


namespace priv {
//-------------------------------------------------------------------
/// General string tokenizer, taken from http://stackoverflow.com/a/236803/193789
/**
- see also this one: http://stackoverflow.com/a/53878/193789
*/
inline
std::vector<std::string>
tokenize( const std::string &s, char delim )
{
	std::vector<std::string> velems;
//    std::stringstream ss( TrimSpaces(s) );
    std::stringstream ss( s );
    std::string item;
    while( std::getline( ss, item, delim ) )
        velems.push_back(item);

    return velems;
}

} // namespace priv


/// Basic parsing of points that are in the format "10,20 30,40 50,60"
std::vector<Point2d>
parsePoints( const char* pts )
{
	std::vector<Point2d> out;
	std::string s(pts);
	std::cout << "processing " << s << '\n';
//	trimString( s );
	auto v1 = priv::tokenize( s, ' ' );
	for( const auto& pt: v1 )
	{
		auto v2 = priv::tokenize( pt, ',' );
		if( v2.size() != 2 )
			throw "h2d:img::svg: invalid point format in importing svg element: " + s;
		auto x = std::stod( v2[0] );
		auto y = std::stod( v2[1] );
		out.emplace_back( Point2d(x,y) );
	}
	return out;
}

//------------------------------------------------------------------
/// Visitor class, derived from the tinyxml2 visitor class
class Visitor: public tinyxml2::XMLVisitor
{
private:
	std::vector<detail::Root*> vec;
public:
	std::vector<detail::Root*> get() const
	{
		return vec;
	}

	bool VisitExit( const tinyxml2::XMLElement& elem );
};

/// Fetch attribute from XML element. Tag \c e_name is there just in case of trouble.
double getValue( const tinyxml2::XMLElement& e, const char* str, std::string e_name )
{
	double value;
	if( tinyxml2::XML_SUCCESS != e.QueryDoubleAttribute( str, &value ) )
		throw "h2d::svg::import error, failed to read attribute " + std::string{str} + " while reading element " + e_name + "\n";
	return value;
}

/// This is the place where actual SVG data is converted and stored into vector
bool Visitor::VisitExit( const tinyxml2::XMLElement& e )
{
	std::string n = e.Name();
	std::cout << "PROCESS n="<< n << " s="<< n.size() <<"\n";
	if( n == "circle" )
	{
		Circle* c = new Circle( getValue( e, "cx", n ), getValue( e, "cy", n ), getValue( e, "r", n ) );
		vec.push_back( c );
	}
	if( n == "rect" )
	{
		auto x1 = getValue( e, "x", n );
		auto y1 = getValue( e, "y", n );
		auto w  = getValue( e, "width", n );
		auto h  = getValue( e, "height", n );
		FRect* r = new FRect( x1, y1, x1+w, y1+h );
		vec.push_back( r );
	}
	if( n == "line" )
	{
		Segment* s = new Segment( getValue( e, "x1", n ), getValue( e, "y1", n ), getValue( e, "x2", n ), getValue( e, "y2", n ) );
		vec.push_back( s );
	}
	if( n == "polygon" )
	{
		CPolyline* p = new CPolyline();
		const char *pts = e.Attribute( "points" );
		auto vec_pts = parsePoints( pts );
		std::cout << "importing " << vec_pts.size() << " pts\n";
		p->set( vec_pts );
		vec.push_back( p );
	}
	if( n == "polyline" )
	{
		OPolyline* p = new OPolyline();
		const char *pts = e.Attribute( "points" );
		auto vec_pts = parsePoints( pts );
		std::cout << "importing " << vec_pts.size() << " pts\n";
		p->set( vec_pts );
		vec.push_back( p );


	}

	if( n == "ellipse" ) // TODO: handle ellipse angle
	{
		auto x  = getValue( e, "cx", n );
		auto y  = getValue( e, "cy", n );
		auto rx = getValue( e, "rx", n );
		auto ry = getValue( e, "ry", n );
		Ellipse* p = new Ellipse( x, y, rx, ry );
		vec.push_back( p );
	}

	return true;
}

#if 0
enum Type {
	Letter,DigitOrDot,Space,MinusOrPlus
};

enum RelMode
{ Relative, Absolute };

/// SVG commands (subset)
enum class Action: uint8_t
{
	LineTo,     ///< L
	Horizontal, ///< H
	Vertical,   ///< V
	MoveTo,     ///< M
	ClosePath   ///< Z
};

//------------------------------------------------------------------
Action getAction( char c )
{
	switch( c )
	{
		case 'L':
		case 'l':
			return Action::LineTo;
		break;
		case 'H':
		case 'h':
			return Action::Horizontal;
		break;
		case 'V':
		case 'v':
			return Action::Vertical;
		break;
		case 'M':
		case 'm':
			return Action::MoveTo;
		break;
		case 'Z':
		case 'z':
			return Action::ClosePath;
		break;
	}
}

Type type( char c )
{
	if( c >= '0' && c <= '9' || c == '.' )
		return DigitOrDot;
	if( c == ' ' )
		return Space;
	if( c == '-' || c == '+' )
		return MinusOrPlus;
	return Letter;
}


double fetchValueAndClear( std::ostringstream& sstr )
{
	double value = std::atof( sstr.str().c_str() );
	std::cout << "val=" << value << '\n';
	sstr.clear();
	sstr.str("");
	return value;
}

bool isAllowedLetter( char c )
{
	if(
		c=='S' || c=='s' || c=='C'  || c=='c'
		|| c=='Q' || c=='q' || c=='T'  || c=='t'
		|| c=='A' || c=='a'
	)
		return false;
	if(
		c=='L' || c=='l' || c=='H' || c=='h'
		|| c=='V' || c=='v' || c=='M' || c=='m'
		|| c=='Z' || c=='z'
	)
		return true;
	return false;
}

//------------------------------------------------------------------
struct SvgPathData
{
private:
	bool hasNumValue   = false;
	bool hasPointValue = false;
	bool isFirstValue  = true;
	std::vector<Point2d> vpts;
	std::vector<double>  v_values;

	bool isX = true;
	std::ostringstream sstr;
	Point2d pt;
	RelMode _mode;
	Action  _action;
	bool _nextValueIsNeg = false;


public:
	size_t getNbArgs()
	{
		if( _action == Action::Horizontal || _action == Action::Vertical )
			return 1;
		if( _action == Action::ClosePath )
			return 0;
		return 2;
	}

	void addDigit( char current )
	{
		sstr << current;
		hasNumValue = true;
	}

	void processCurrentValue() // we have a value that we want to process
	{
		if( hasNumValue ) // if we have already parsed a num value previously,
		{                 // then store it
			hasNumValue = false;
			auto value = fetchValueAndClear(sstr);
			if( _nextValueIsNeg )
			{
				value = -value;
				_nextValueIsNeg = false;
			}
			v_values.push_back( value );
			if( v_values.size() == getNbArgs() ) // we are done
			{
				switch( v_values.size() )
				{
					case 0:
					break;
					case 1:
						assert( _action == Action::Horizontal || _action == Action::Vertical );
					break;
					case 2:
						pt.set( v_values[0], v_values[1] ); // \todo handle relative !!!
						hasPointValue = true;
					break;
					default:assert(0);
				}
				v_values.clear();
			}
			if( hasPointValue )
			{
				vpts.push_back( pt );
				std::cout << "adding pt " << pt << ", size=" << vpts.size() << '\n';
				hasPointValue = false;
			}
		}
	}
	void processLetter( char current )
	{
		std::cout << "process letter " << current << '\n';
		if( !isAllowedLetter( current ) )
			throw std::runtime_error("invalid letter:" + current );

		_mode = Absolute;
		if( islower(current) )
			_mode = Relative;
		_action = getAction( current );

	}
	void nextValueIsNeg()
	{
		_nextValueIsNeg = true;
	}
};

//------------------------------------------------------------------

void parsePath( const std::string& s )
{
	bool Done = false;
	size_t idx = 0;
	SvgPathData data;
	double value = 0.;
	do
	{
		std::cout << "loop start, idx=" << idx << '\n';
		char current = s[idx];
		switch( type(current) )
		{
			case Letter:
				data.processCurrentValue();
				data.processLetter( current );
			break;
			case DigitOrDot:
				data.addDigit( current );
			break;
			case Space:
				data.processCurrentValue();
			break;
			case MinusOrPlus:
				data.nextValueIsNeg();
				data.processCurrentValue();
			break;

		}
		idx++;
	}
	while( !Done && idx < s.size() );
   }

#endif


} // namespace svg

} // namespace h2d
