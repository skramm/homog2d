// tmp_svgimport.hpp

// https://www.w3.org/TR/SVG/paths.html
#include "tinyxml2.h"


namespace h2d {
namespace svg {


//------------------------------------------------------------------
template<typename FPT>
class XmlVisitor: public tinyxml2::XMLVisitor
{
private:
	std::vector<detail::Common<FPT>*> vec;
public:
	std::vector<detail::Common<FPT>*> get() const
	{
		return vec;
	}
	void processElem( const tinyxml2::XMLElement& e );

	bool VisitExit( const tinyxml2::XMLElement& elem )
	{

		std::cout << "-exit: name=" << elem.Name() << '\n';
		processElem( elem );
		return true;
	}
	bool VisitEnter( const tinyxml2::XMLElement& elem, const tinyxml2::XMLAttribute* a )
	{
//		cout << __PRETTY_FUNCTION__ << '\n';
		std::cout << "-start elem: name=" << elem.Name() << '\n';
/*		if( a )
		{
			cout << "   first attr: " << a->Name() << ":" << a->Value() << '\n';
		}*/


		std::cout << "Attrib list:\n";

		for( const tinyxml2::XMLAttribute* a = elem.FirstAttribute(); a; a=a->Next() )
		{
			std::cout << " -attr: " << a->Name() << ":" << a->Value() << '\n';
		}
		return true;
	}

};

template<typename FPT>
void XmlVisitor<FPT>::processElem( const tinyxml2::XMLElement& e )
{
	std::string n = e.Name();
std::cout << "PROCESS n="<< n << " s="<< n.size() <<"\n";
	if( n == "circle" )
	{
		std::cout << "creating circle!\n";
		double cx,cy,rad;
		if( tinyxml2::XML_SUCCESS != e.QueryDoubleAttribute( "cx", &cx ) )
			throw "failed to read attribute x\n";
		if( tinyxml2::XML_SUCCESS != e.QueryDoubleAttribute( "cy", &cy ) )
			throw "failed to read attribute cy\n";
		if( tinyxml2::XML_SUCCESS != e.QueryDoubleAttribute( "r", &rad ) )
			throw "failed to read attribute r\n";
//		cout << "x=" << value << '\n';
		Circle* c = new Circle( cx, cy, rad );
		vec.push_back( c );
	}

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
