// tmp_svgimport.hpp

// https://www.w3.org/TR/SVG/paths.html

namespace h2d {
namespace svg {

enum Type {
	Letter,DigitOrDot,Space,Minus
};

enum RelMode
{ Relative, Absolute };

/// SVG commands (subset)
enum class Action
{
	LineTo,     ///< L
	Horizontal, ///< H
	Vertical,   ///< V
	MoveTo,     ///< M
	ClosePath   ///< Z
};

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

int getNbArgs( Action action )
{
	if( action == Horizontal || action == Vertical )
		return 1;
	if( action == ClosePath )
		return 0;
	return 2;
}

Type type( char c )
{
	if( c >= '0' && c <= '9' || c == '.' )
		return DigitOrDot;
	if( c == ' ' )
		return Space;
	if( c == '-' )
		return Minus;
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

struct SvgPathData
{
private:
	bool hasNumValue   = false;
	bool hasPointValue = false;
	bool isFirstValue  = true;
	std::vector<Point2d> vpts;

	bool isX = true;
	std::ostringstream sstr;
	Point2d pt;
	RelMode _mode;
	Action  _action;
	bool _nextValueIsNeg = false;

public:
	void addDigit( char current )
	{
		sstr << current;
		hasNumValue = true;
	}

	void processCurrentValue() // we have a value !
	{
		if( hasNumValue ) // if we have already parsed a num value previously,
		{                 // then store it
			auto value = fetchValueAndClear(sstr);
			if( isX )
			{
				isX  = false;
				pt.x = value;
			}
			else
			{
				isX = true;
				pt.y = value;
				hasPointValue = true; // now we have a point !
				hasNumValue   = false;
			}
			if( hasPointValue )
			{
				vpts.push_back( pt );
				std::cout << "adding pt " << pt.x << "-" << pt.y << ", size=" << vpts.size() << '\n';
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
			_mode = Relative
		_action = getAction( current );

	}
	void nextValueIsNeg()
	{
		_nextValueIsNeg = true;
	}
};

void parsePath( const std::string& s )
{

	bool Done = false;
	size_t idx = 0;
	Data data;
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
			case Minus:
				data.nextValueIsNeg();
				data.processCurrentValue();
			break;

		}
		idx++;
	}
	while( !Done && idx < s.size() );
   }




} // namespace svg

} // namespace h2d
