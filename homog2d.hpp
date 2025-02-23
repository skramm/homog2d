/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points, see https://github.com/skramm/homog2d

    Author & Copyright 2019-2025 Sebastien Kramm

    Contact: firstname.lastname@univ-rouen.fr

    Licence: MPL v2

	This Source Code Form is subject to the terms of the Mozilla Public
	License, v. 2.0. If a copy of the MPL was not distributed with this
	file, You can obtain one at https://mozilla.org/MPL/2.0/.

**************************************************************************/

/**
\file homog2d.hpp
\brief single header file, implements some 2D homogeneous stuff.
See https://github.com/skramm/homog2d
*/

#ifndef HG_HOMOG2D_HPP
#define HG_HOMOG2D_HPP

#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <array>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <type_traits>
#include <functional>
#include <limits>
#include <cstdint> // required for uint8_t
#include <memory>  // required for std::unique_ptr

#ifdef HOMOG2D_USE_SVG_IMPORT
	#define HOMOG2D_ENABLE_VRTP
//	#include <cctype> // why was that needed in the first place?
	#include "tinyxml2.h"
#endif // HOMOG2D_USE_SVG_IMPORT

#ifdef HOMOG2D_ENABLE_VRTP
	#include <variant>
#endif

#ifdef HOMOG2D_USE_EIGEN
	#include <Eigen/Dense>
#endif

#ifdef HOMOG2D_USE_TTMATH
	#include <ttmath/ttmath.h>
	#ifndef HOMOG2D_INUMTYPE
		#define HOMOG2D_INUMTYPE ttmath::Big<2,2>
	#endif

	#define homog2d_abs  ttmath::Abs
	#define homog2d_sin  ttmath::Sin
	#define homog2d_cos  ttmath::Cos
	#define homog2d_acos ttmath::ACos
	#define homog2d_asin ttmath::ASin
	#define homog2d_atan ttmath::ATan
	#define homog2d_sqrt ttmath::Sqrt
#else
	#define homog2d_abs  std::abs
	#define homog2d_sin  std::sin
	#define homog2d_cos  std::cos
	#define homog2d_acos std::acos
	#define homog2d_asin std::asin
	#define homog2d_atan std::atan
	#define homog2d_sqrt std::sqrt
#endif

#ifdef HOMOG2D_USE_OPENCV
	#include "opencv2/imgproc.hpp"
	#include "opencv2/highgui.hpp"
#endif

#ifdef HOMOG2D_USE_BOOSTGEOM
	#include <boost/geometry.hpp>
#endif

#ifdef _MSC_VER
	#define HOMOG2D_PRETTY_FUNCTION __FUNCSIG__
#else
	#define HOMOG2D_PRETTY_FUNCTION __PRETTY_FUNCTION__
#endif

#ifdef HOMOG2D_DEBUGMODE
	#define HOMOG2D_START std::cout << "START: line:" << __LINE__ \
		<< " func=\n" << HOMOG2D_PRETTY_FUNCTION << "\n"
//	#define HOMOG2D_START std::cout << __FUNCTION__ << "()\n"
#else
	#define HOMOG2D_START
#endif

// 		<< std::scientific << std::setprecision(10)
#ifdef HOMOG2D_DEBUGMODE
	#define HOMOG2D_LOG(a) \
		std::cout << '-' << __FUNCTION__ << "(), line " << __LINE__ << ": " \
		<< a << std::endl;
#else
	#define HOMOG2D_LOG(a) {;}
#endif

#define HOMOG2D_ASSERT( a ) \
	if( !(a) ) \
	{ \
		std::cerr << "ASSERT FAILURE, line=" << __LINE__ << std::endl; \
		std::exit(1); \
	}

/// Assert debug macro, used internally if \c HOMOG2D_DEBUGMODE is defined
#define HOMOG2D_DEBUG_ASSERT(a,b) \
	{ \
		if( (a) == false ) \
		{ \
			std::cerr << "Homog2d assert failure, version:" << HOMOG2D_VERSION \
				<< ", line:" << __LINE__ << "\n -details: " << b << '\n'; \
			std::cout << "homog2d: internal failure, please check stderr and report this on https://github.com/skramm/homog2d/issues\n"; \
			std::exit(1); \
		} \
	}

#define HOMOG2D_CHECK_ROW_COL \
	if( r > 2 ) \
		throw std::runtime_error( "Error: invalid row value: r=" + std::to_string(r) ); \
	if( c > 2 ) \
		throw std::runtime_error( "Error: invalid col value: r=" + std::to_string(r) )

#ifdef HOMOG2D_USE_TTMATH
#	define HOMOG2D_CHECK_IS_NUMBER(T)
#else
#	define HOMOG2D_CHECK_IS_NUMBER(T) \
		static_assert( (std::is_arithmetic<T>::value && !std::is_same<T, bool>::value), "Type of value must be numerical" )
#endif

#ifndef HOMOG2D_NOWARNINGS
#define HOMOG2D_LOG_WARNING( a ) \
	if( err::printWarnings() == true ) \
		std::cerr << "homog2d warning (" << ++err::warningCount() << "), l. " << __LINE__ << ": " << a << "\n";
#else
#define HOMOG2D_LOG_WARNING
#endif

/*
\todo 20230212 ttmath support: this definition does not work, I don't know why !!! see namespace \ref trait
\verbatim
#define HOMOG2D_CHECK_IS_NUMBER(T) \
	static_assert( \
	((std::is_arithmetic<T>::value && !std::is_same<T, bool>::value) || trait::IsBigNumType<T>::value), \
	"Type of value must be numerical" )
\endverbatim
*/

/// Internal type used for numerical computations, possible values: \c double, <code>long double</code>
#if !defined(HOMOG2D_INUMTYPE)
	#define HOMOG2D_INUMTYPE double
#endif

#if !defined(HOMOG2D_BIND_X)
	#define HOMOG2D_BIND_X x
#endif
#if !defined(HOMOG2D_BIND_Y)
	#define HOMOG2D_BIND_Y y
#endif

/// Error throw wrapper macro
#define HOMOG2D_THROW_ERROR_1( msg ) \
	{ \
		std::ostringstream oss; \
		oss << "homog2d: line " <<  __LINE__  << ", function:" << __FUNCTION__ << "(): " \
			<< msg << "\n -full function name: " << HOMOG2D_PRETTY_FUNCTION \
			<< "\n -Error count=" << ++err::errorCount(); \
		throw std::runtime_error( oss.str() ); \
	}

/// Error throw wrapper macro, first arg is the function name
#define HOMOG2D_THROW_ERROR_2( func, msg ) \
	{ \
		std::ostringstream oss; \
		oss << "homog2d: line " <<  __LINE__  << ", function:" << func << "(): " \
			<< msg << "\n -full function name: " << HOMOG2D_PRETTY_FUNCTION \
			<< "\n -Error count=" << ++err::errorCount(); \
		throw std::runtime_error( oss.str() ); \
	}


///////////////////////////////////////
// Default values for thresholds
#ifndef HOMOG2D_THR_ZERO_DIST
	#define HOMOG2D_THR_ZERO_DIST 1E-10
#endif

#ifndef HOMOG2D_THR_ZERO_ORTHO_DIST
	#define HOMOG2D_THR_ZERO_ORTHO_DIST 1E-14
#endif

// default value: 1 thousand of a radian (tan = 0.001 too)
#ifndef HOMOG2D_THR_ZERO_ANGLE
	#define HOMOG2D_THR_ZERO_ANGLE 0.001
#endif

#ifndef HOMOG2D_THR_ZERO_DENOM
	#define HOMOG2D_THR_ZERO_DENOM 1E-10
#endif

#ifndef HOMOG2D_THR_ZERO_DETER
	#define HOMOG2D_THR_ZERO_DETER 1E-15
#endif
///////////////////////////////////////

/// Max number of iterations for "Point inside Polygon" algorithm.
/// May be adjusted, see manual
#ifndef HOMOG2D_MAXITER_PIP
	#define HOMOG2D_MAXITER_PIP 5
#endif

#define HOMOG2D_VERSION "2.12.2"

// some MS environments seem to lack Pi definition, even if _USE_MATH_DEFINES is defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


namespace h2d {

/// Holds static counters, for runtime errors and warnings
namespace err {

/// Used to count the errors
/**
This is used in the HOMOG2D_THROW_ERROR_1 macros. Some user code could catch the exceptions, thus
this will enable the counting of errors

\todo 20250123: for some reason, the error count is always 42! Need to investigate this.
*/
inline size_t& errorCount()
{
	static size_t c_err;
	return c_err;
}

/// Used in macro HOMOG2D_LOG_WARNING
inline size_t& warningCount()
{
	static size_t c_war;
	return c_war;
}

/// User can use this to silence warnings at runtime.
inline bool& printWarnings()
{
	static bool b_war = true;
	return b_war;
}


} //namespace err

/// Holds the types needed for policy based design
namespace typ {

/// Used to determine if line or point, see base::LPBase
struct IsLine   {};
struct IsPoint  {};
struct IsHomogr {};
struct IsEpipmat {};

/// Used to determine the type of "point pair (segment or vector), see base::SegVec
struct IsSegment {};
/// \sa IsSegment
struct IsOSeg {};

/// Used to determine the type of polyline (\ref CPolyline_ or \ref OPolyline_), see base::PolylineBase
struct IsClosed {};
/// \sa IsClosed
struct IsOpen {};

/// \name These are used as static member type SType
///@{
struct T_Point    {};
struct T_Line     {};
struct T_Circle   {};
struct T_FRect    {};
struct T_Segment  {};
struct T_OSeg     {};
struct T_OPol     {};
struct T_CPol     {};
struct T_Ellipse  {};
///@}

} // namespace typ


namespace detail {

	template<typename FPT> class Matrix_;

	/// Helper class for Root (Point/Line) type, used as a trick to allow partial specialization of member functions
	template<typename> struct BaseHelper {};

/// Helper class for used to get the underlying floating-point type, see Dtype and Common::dtype()
	template<typename> struct DataFpType {};

/// Helper class for PolylineBase, used as a trick to allow partial specialization of member functions
	template<typename> struct PlHelper {};

#ifdef HOMOG2D_FUTURE_STUFF
	/// Helper class for Matrix type
	template<typename T1>
	struct HelperMat {};

	template<>
	struct HelperMat<typ::IsHomogr>
	{
		using M_OtherType = IsEpipmat;
	};

	template<>
	struct HelperMat<typ::IsEpipmat>
	{
		using M_OtherType = IsHomogr;
	};
#endif

    template<typename> struct HelperPL;

    template<>
    struct HelperPL<typ::IsPoint>
    {
        using OtherType = typ::IsLine;
    };

    template<>
    struct HelperPL<typ::IsLine>
    {
        using OtherType = typ::IsPoint;
    };

	/// A trick used in static_assert, so it aborts only if function is instanciated
	template<typename T>
	struct AlwaysFalse {
		enum { value = false };
	};

} // namespace detail

/// Holds base classes, not part of API
namespace base {
	template<typename PLT,typename FPT> class PolylineBase;
	template<typename LP, typename FPT> class LPBase;
	template<typename SV, typename FPT> class SegVec;
}

template<typename LP,typename FPT> class Hmatrix_;

template<typename T>
using Homogr_  =  Hmatrix_<typ::IsHomogr,T>;
#ifdef HOMOG2D_FUTURE_STUFF
template<typename T>
using Epipmat_ =  Hmatrix_<typ::IsEpipmat,T>;
#endif

template<typename FPT> class Circle_;
template<typename FPT> class FRect_;
template<typename FPT> class Ellipse_;

namespace img {
struct SvgImage; // forward declaration
}

template<typename T>
using Point2d_ = base::LPBase<typ::IsPoint,T>;
template<typename T>
using Line2d_  = base::LPBase<typ::IsLine,T>;

template<typename T>
using Segment_ = base::SegVec<typ::IsSegment,T>;
template<typename T>
using OSegment_  = base::SegVec<typ::IsOSeg,T>;


template<typename T1,typename T2>
using PointPair2_ = std::pair<Point2d_<T1>,Point2d_<T2>>;
template<typename T>
using PointPair_  = std::pair<Point2d_<T>,Point2d_<T>>;

template<typename T>
using CPolyline_ = base::PolylineBase<typ::IsClosed,T>;
template<typename T>
using OPolyline_ = base::PolylineBase<typ::IsOpen,T>;


#ifdef HOMOG2D_ENABLE_VRTP
/// A variant type, holding all possible types. Used to achieve runtime polymorphism
/**
See https://github.com/skramm/homog2d/blob/master/docs/homog2d_manual.md#section_rtp
*/
template<typename FPT>
using CommonType_ = std::variant<
	Segment_<FPT>,
	OSegment_<FPT>,
	Point2d_<FPT>,
	Line2d_<FPT>,
	Circle_<FPT>,
	Ellipse_<FPT>,
	FRect_<FPT>,
	CPolyline_<FPT>,
	OPolyline_<FPT>
>;
#endif // HOMOG2D_ENABLE_VRTP

//------------------------------------------------------------------
/// Holds drawing related code, independent of back-end library
namespace img {

/// Color type , see DrawParams
struct Color
{
	uint8_t r = 80;
	uint8_t g = 80;
	uint8_t b = 80;
	Color( uint8_t rr, uint8_t gg, uint8_t bb ): r(rr),g(gg),b(bb) {}
	Color() = default;

	friend std::ostream& operator << ( std::ostream& f, const Color& c )
	{
		f << "Color:" << (int)c.r << '-' << (int)c.g << '-' << (int)c.b;
		return f;
	}
};

/// Helper function, will generate a vector of \c nb random RGB colors
/**
- RGB values will be between \c minval and \c minval+coeff
*/
inline
std::vector<img::Color>
genRandomColors( size_t nb, int minval=20, int maxval=250 )
{
	if( maxval<=minval )
		HOMOG2D_THROW_ERROR_1( "Illegal values for minval and maxval" );
	std::vector<img::Color> vcol( nb );
	std::srand( std::time(nullptr) );

	for( size_t i=0; i<nb; i++ )
	{
		auto colR = 1.0*std::rand() / RAND_MAX * (maxval-minval) + minval;
		auto colG = 1.0*std::rand() / RAND_MAX * (maxval-minval) + minval;
		auto colB = 1.0*std::rand() / RAND_MAX * (maxval-minval) + minval;
		vcol[i] = img::Color(colR,colG,colB);
	}
	return vcol;
}

/// A svg image as a wrapper around a string, see manual, "Drawing things" section
struct SvgImage
{
	std::ostringstream _svgString; ///< the svg objects
};


//------------------------------------------------------------------
/// Point drawing style, see DrawParams
/**
Demo: https://github.com/skramm/homog2d/blob/master/docs/homog2d_manual.md#drawing_params

\warning Check nextPointStyle() in case of added values here!
*/
enum class PtStyle: uint8_t
{
	Plus,   ///< \c + symbol
	Times,  ///< \c X symbol
	Star,   ///< \c * symbol
	Diam,   ///< diamond
	Squ,    ///< square (new 20241101 !)
	Dot     ///< dot (circle)
};

inline
const char*
getString( PtStyle t )
{
	const char* s=0;
	switch( t )
	{
		case PtStyle::Plus:  s="Plus";  break;
		case PtStyle::Times: s="Times"; break;
		case PtStyle::Star:  s="Star";  break;
		case PtStyle::Diam:  s="Diam";  break;
		case PtStyle::Squ:   s="Square";break;
		case PtStyle::Dot:   s="Dot";   break; // WARNING: keep this as last one (assumed to be last one in some code)
		default: assert(0);
	}
	return s;
}

//------------------------------------------------------------------
/// Draw parameters, independent of back-end library
class DrawParams
{
	template<typename T> friend class h2d::Circle_;
	template<typename T> friend class h2d::FRect_;
	template<typename T> friend class h2d::Ellipse_;
	template<typename T,typename U> friend class h2d::base::PolylineBase;
	template<typename T,typename U> friend class h2d::base::LPBase;
	template<typename T,typename U> friend class h2d::base::SegVec;

/// Inner struct, holds the values. Needed so we can assign a default value as static member
/// \todo 20240329 maybe we can merge parameters _ptDelta and _pointSize into a single one?
	struct Dp_values
	{
		Color       _color;
		int         _lineThickness = 1;
		int         _pointSize     = 4;
		int         _lineType      = 1; /// if OpenCv: 1 for cv::LINE_AA, 2 for cv::LINE_8
		uint8_t     _ptDelta       = 5;           ///< pixels, used for drawing points
		PtStyle     _ptStyle       = PtStyle::Plus;
		bool        _enhancePoint  = false;     ///< to draw selected points
		bool        _showPoints    = false;     ///< show the points (useful only for Segment_ and Polyline_)
		bool        _showIndex     = false;     ///< show the index as number
		bool        _showAngles    = false;
		int         _fontSize      = 20;        ///< font size for drawText()
		std::string _attrString;                ///< added attributes (SVG only)

/// Returns the point style following the current one
		PtStyle nextPointStyle() const
		{
			if( _ptStyle == PtStyle::Dot )
				return PtStyle::Plus;
			auto curr = static_cast<int>(_ptStyle);
			return static_cast<PtStyle>(curr+1);
		}
	};

	friend std::ostream& operator << ( std::ostream& f, const DrawParams& dp )
	{
		f << "-" << dp._dpValues._color
			<< "\n-line width=" << dp._dpValues._lineThickness
			<< "\n-pointSize=" << dp._dpValues._pointSize
			<< "\n-showPoints=" << dp._dpValues._showPoints
			<< "\n-fontSize=" << dp._dpValues._fontSize
			<< '\n';
		return f;
	}


public:
	Dp_values _dpValues;

private:
	static Dp_values& p_getDefault()
	{
		static Dp_values s_defValue;
		return s_defValue;
	}

public:
	DrawParams()
	{
		_dpValues = p_getDefault();
	}
	void setDefault()
	{
		p_getDefault() = this->_dpValues;
	}
	static void resetDefault()
	{
		p_getDefault() = Dp_values();
	}
	DrawParams& setPointStyle( PtStyle ps )
	{
		if( (int)ps > (int)PtStyle::Dot )
			throw std::runtime_error( "Error: invalid value for point style");
		_dpValues._ptStyle = ps;
		return *this;
	}
	DrawParams& setPointSize( uint8_t ps )
	{
		if( ps%2 == 0 )
			HOMOG2D_THROW_ERROR_1( "odd number required" );
		_dpValues._pointSize = ps;
		_dpValues._ptDelta = ps;
		return *this;
	}
	DrawParams& setThickness( uint8_t t )
	{
		_dpValues._lineThickness = t;
		return *this;
	}
	DrawParams& setColor( uint8_t r, uint8_t g, uint8_t b )
	{
		_dpValues._color = Color{r,g,b};
		return *this;
	}
	DrawParams& setColor( Color col )
	{
		_dpValues._color = col;
		return *this;
	}
	DrawParams& selectPoint()
	{
		_dpValues._enhancePoint = true;
		return *this;
	}
/// Set or unset the drawing of points (useful only for Segment_ and Polyline_)
	DrawParams& showPoints( bool b=true )
	{
		_dpValues._showPoints = b;
		return *this;
	}
/// Set font size for drawText()
	DrawParams& setFontSize( int value /* pixels */ )
	{
		assert( value > 1 );
		_dpValues._fontSize = value;
		return *this;
	}
/// Set or unset the drawing of points (useful only for Segment_ and Polyline_)
	DrawParams& showIndex( bool b=true )
	{
		_dpValues._showIndex = b;
		return *this;
	}
/// Set or unset the drawing of angles of polylines
	DrawParams& showAngles( bool b=true )
	{
		_dpValues._showAngles = b;
		return *this;
	}

/// Add some specific SVG attributes (ignored for Opencv renderings)
/** \sa getAttrString() */
	DrawParams& setAttrString( std::string attr )
	{
		_dpValues._attrString = attr;
		return *this;
	}

	Color color() const
	{
		return _dpValues._color;
	}

#ifdef HOMOG2D_USE_OPENCV
	cv::Scalar cvColor() const
	{
		return cv::Scalar( _dpValues._color.b, _dpValues._color.g, _dpValues._color.r );
	}
#endif // HOMOG2D_USE_OPENCV

private:
/// Checks if the user-given SVG attribute string (with \ref setAttrString() ) holds the fill="none" mention.
/**
This is because to have no filling, the object needs to have the fill="none" attribute, so the default
behavior is to always add that attribute.<br>
But then if the user wants some filling, then we would have both fill="none" and fill="somecolor",
and that would render the svg invalid.<br>
So the drawing code checks if user has added some filling, and if so, does not add the fill="none" attribute.
*/
	bool holdsFill() const
	{
		if( !_dpValues._attrString.empty() )
			if( _dpValues._attrString.find("fill=") != std::string::npos )
				return true;
		return false;
	}
/// \sa setAttrString()
	std::string getAttrString() const
	{
		if( _dpValues._attrString.empty() )
			return std::string();
		return _dpValues._attrString + ' ';
	}

	std::string getSvgRgbColor() const
	{
		std::ostringstream oss;
		oss << "rgb("
			<< (int)_dpValues._color.r << ','
			<< (int)_dpValues._color.g << ','
			<< (int)_dpValues._color.b
			<< ')';
		return oss.str();
	}

}; // class DrawParams

//------------------------------------------------------------------
/// Opaque data structure, will hold the image type, depending on back-end library.
/// This type is the one used in all the drawing functions.
/**
At present the two allowed types are cv::Mat
(external Opencv library, requires the symbol HOMOG2D_USE_OPENCV to be defined)
or SvgImage (no dependency)
*/
template<typename T>
class Image
{
	friend std::ostream& operator << ( std::ostream&, const Image<SvgImage>& );

private:
	T      _realImg;
	size_t _width  = 500;
	size_t _height = 500;

public:
	Image() = default;
	Image( T& m ): _realImg(m)
	{}
/// Returns a reference on the underlying image
	T& getReal()
	{
		return _realImg;
	}
/// Returns a const reference on the underlying image
	const T& getReal() const
	{
		return _realImg;
	}

	std::pair<size_t,size_t> size() const
	{
		return std::make_pair( _width, _height );
	}
/// That constructor is the default, shouln't be instanciated, see specializations
	Image( size_t, size_t )
	{
		assert(0);
//		static_assert( detail::AlwaysFalse<std::false_type>::value, "no concrete implementation available" );
//		static_assert( std::false_type, "no concrete implementation available" );
	}

	void setSize( size_t width, size_t height );
	template<typename F>
	void setSize( const std::pair<F,F>& );

	void write( std::string ) const // will be specialized
	{
		assert(0);
	}

	int cols() const { return _width; }
	int rows() const { return _height; }
	void clear( Color c=Color(255,255,255) )                  { clear(c.r,c.g,c.b); }

	void clear( uint8_t, uint8_t, uint8_t )
	{
		assert(0);
	}
	void clear( uint8_t )
	{
		assert(0);
	}
	void drawText( std::string, Point2d_<float>, img::DrawParams dp=img::DrawParams() );

	template<typename U>
	void draw( const U& object, img::DrawParams dp=img::DrawParams() );
	template<typename U,typename V>
	void draw( const std::pair<U,V>& p_objects, img::DrawParams dp=img::DrawParams() );


#ifdef HOMOG2D_USE_OPENCV
/// Show image on window \c wname (not available for SVG !)
	void show( std::string wname )
	{
		cv::imshow( wname, _realImg );
	}
private:
	void p_setSize( size_t width, size_t height )
	{
		_width  = width;
		_height = height;
		_realImg.create( (int)height, (int)width, CV_8UC3 );
		clear();
	}
#endif
}; // class Image

#ifdef HOMOG2D_USE_OPENCV
template <>
inline
Image<cv::Mat>::Image( size_t width, size_t height )
{
	p_setSize( width, height );
}

template <>
inline
void
Image<cv::Mat>::setSize( size_t width, size_t height )
{
	p_setSize( width, height );
}
#endif

template <typename IMG>
void
Image<IMG>::setSize( size_t width, size_t height )
{
	_width = width;
	_height = height;
}

template <typename IMG>
template <typename T>
void
Image<IMG>::setSize( const std::pair<T,T>& pa )
{
	setSize( pa.first, pa.second );
}

template <>
inline
Image<SvgImage>::Image( size_t width, size_t height )
{
	setSize( width, height );
}

template <>
inline
void
Image<SvgImage>::write( std::string fname ) const
{
	std::ofstream file( fname );
	if( !file.is_open() )
	{
		HOMOG2D_THROW_ERROR_1( "unable to open output file '" + fname + "'" );
	}
	file << "<svg version=\"1.1\" width=\"" << _width
		<< "\" height=\"" << _height
		<< "\" style=\"background-color:white;\" xmlns=\"http://www.w3.org/2000/svg\">\n"
		<< "<style>\n"
		<< ".txt1 { font: bold 12px sans-serif; };\n"   // text style, you can change or add classes as required
		<< "</style>\n";

	file << _realImg._svgString.str();
	file << "</svg>\n";
}

template <>
inline
void
Image<SvgImage>::clear( uint8_t, uint8_t, uint8_t )
{
	_realImg._svgString.str("");
	_realImg._svgString.clear();
}

#ifdef HOMOG2D_USE_OPENCV
template <>
inline
void
Image<cv::Mat>::clear( uint8_t r, uint8_t g, uint8_t b )
{
	_realImg = cv::Scalar(b,g,r);
}
template <>
inline
void
Image<cv::Mat>::clear( uint8_t col )
{
	_realImg = cv::Scalar(col,col,col);
}

template <>
inline
void
Image<cv::Mat>::write( std::string fname ) const
{
	cv::imwrite( fname, _realImg );
}
#endif // HOMOG2D_USE_OPENCV

template<typename IMG>
template<typename U>
void Image<IMG>::draw( const U& object, img::DrawParams dp )
{
	object.draw( *this, dp );
}

template<typename IMG>
template<typename U,typename V>
void Image<IMG>::draw( const std::pair<U,V>& pairp, img::DrawParams dp )
{
	pairp.first.draw( *this, dp );
	pairp.second.draw( *this, dp );
}


} // namespace img


/////////////////////////////////////////////////////////////////////////////
// SECTION  - PUBLIC ENUM DECLARATIONS
/////////////////////////////////////////////////////////////////////////////

/// Used in base::PolylineBase_::rotate() member function
enum class Rotate: int8_t
{
	CCW,      ///< Counter ClockWise rotation
	CW,       ///< ClockWise rotation
	Full,     ///< 180° rotation
	VMirror,  ///< vertical symmetry
	HMirror   ///< horizontal symmetry
};

enum class CardDir: int8_t { Bottom,Top, Left, Right };

/// Used in Line2d::getValue() and getOrthogLine()
enum class GivenCoord: uint8_t { X, Y };

/// Used in line constructor, to instanciate a H or V line, see base::LPBase( LineDir, T )
enum class LineDir: uint8_t { H, V };

/// Type of Root object, see rtp::Root::type().
/// Maybe printed out with getString()
/// \sa type()
enum class Type: uint8_t { Line2d, Point2d, Segment, OSegment, FRect, Circle, Ellipse, OPolyline, CPolyline };

/// Type of underlying floating point, see LPBase::dtype().
/// Maybe printed out with getString()
enum class Dtype: uint8_t {
	Float, Double, LongDouble,
	Other,  // ?
#ifdef HOMOG2D_USE_TTMATH
	Ttmath  ///< only if HOMOG2D_USE_TTMATH is defined, see manual
#endif
};

/// Returns stringified version of \ref type()
inline
const char*
getString( Type t )
{
	const char* s=0;
	switch( t )
	{
		case Type::Line2d:     s="Line2d";    break;
		case Type::Point2d:    s="Point2d";   break;
		case Type::Segment:    s="Segment";   break;
		case Type::OSegment:   s="OSegment";  break;
		case Type::FRect:      s="FRect";     break;
		case Type::Circle:     s="Circle";    break;
		case Type::Ellipse:    s="Ellipse";   break;
		case Type::OPolyline:  s="OPolyline"; break;
		case Type::CPolyline:  s="CPolyline"; break;
		assert(0);
	}
	return s;
}

#ifdef HOMOG2D_ENABLE_VRTP
//------------------------------------------------------------------
/// Holds functors, used to  manage runtime polymorphism using \c std::variant.
/// See https://github.com/skramm/homog2d/blob/master/docs/homog2d_manual.md#section_rtp
namespace fct {

//------------------------------------------------------------------
/// A functor to get the type of an object in a std::variant, call with std::visit()
/**
\sa CommonType_
\sa type()
*/
struct TypeFunct
{
	template<typename T>
	Type operator ()(const T& a)
	{
		return a.type();
	}
};

struct DTypeFunct
{
	template<typename T>
	Dtype operator ()(const T& a)
	{
		return a.dtype();
	}
};

//------------------------------------------------------------------
/// A functor to get the length of an object in a std::variant, call with std::visit()
/**
\sa CommonType_
\sa length()
*/
struct LengthFunct
{
	template<typename T>
	HOMOG2D_INUMTYPE operator ()(const T& a)
	{
		return a.length();
	}
};

//------------------------------------------------------------------
/// A functor to get the area of an object in a std::variant, call with std::visit()
/**
\sa CommonType_
\sa area()
*/
struct AreaFunct
{
	template<typename T>
	HOMOG2D_INUMTYPE operator ()(const T& a)
	{
		return a.area();
	}
};

//------------------------------------------------------------------
/// A functor to get the size  (nb of points) of an object in a std::variant, call with std::visit()
/**
\sa CommonType_
\sa size()
*/
struct SizeFunct
{
	template<typename T>
	HOMOG2D_INUMTYPE operator ()(const T& a)
	{
		return a.size();
	}
};


//------------------------------------------------------------------
/// A functor used to apply a homography matrix to an object
template<typename FPT>
class TransformFunct
{
public:
	TransformFunct( const Homogr_<FPT>& h ): _h(h)
	{}

	template<typename T>
	CommonType_<FPT> operator ()(const T& a)
	{
		return CommonType_<FPT>{_h * a};
	}

private:
	const Homogr_<FPT>& _h;
};

//------------------------------------------------------------------
/// A functor used to draw objects. To use with std::variant and std::visit()
/**
The constructor has a third optional parameter that can be used to pass drawing parameters
*/
template<typename IMG>
struct DrawFunct
{
	DrawFunct(
		img::Image<IMG>& img,
		img::DrawParams dp=img::DrawParams()
	): _img(img), _drawParams(dp)
	{}

	img::Image<IMG>&      _img;
	const img::DrawParams _drawParams;

	template<typename T>
	void operator ()(const T& a)
	{
		a.draw( _img, _drawParams );
	}
};


//------------------------------------------------------------------
/// Convert std::variant object into the underlying type
/**
source: https://stackoverflow.com/a/72955535/193789
*/
template<typename... Ts>
struct VariantUnwrapper
{
    const std::variant<Ts...>& var;

    template <typename T>
    operator T() { return std::get<T>(var); }
};

#if __cplusplus < 202002L
/// Fix for the above VariantUnwrapper for C++17
/**
(may be removed when we switch to C++20)
\sa VariantUnwrapper
*/
template<typename... Ts>
VariantUnwrapper( const std::variant<Ts...>& ) -> VariantUnwrapper<Ts...>;
#endif

} // namespace fct
#endif // HOMOG2D_ENABLE_VRTP


inline
const char*
getString( Dtype t )
{
	const char* s=0;
	switch( t )
	{
		case Dtype::Float:      s="Float";      break;
		case Dtype::Double:     s="Double";     break;
		case Dtype::LongDouble: s="LongDouble"; break;
		case Dtype::Other:      s="Other";      break;
#ifdef HOMOG2D_USE_TTMATH
		case Dtype::Ttmath:     s="ttmath";     break;
#endif
		assert(0);
	}
	return s;
}


//------------------------------------------------------------------
/// Holds private stuff
namespace priv {

/// Implementation of dsize(), returns nb of bits of mantissa and exponent (default implementation)
	template<typename T>
	inline
	std::pair<int,int> impl_dsize( const detail::DataFpType<T>& )
	{
		return std::make_pair(
			std::numeric_limits<T>::digits,
			sizeof(T)*8-std::numeric_limits<T>::digits-1
		);
	}

	inline
	Dtype impl_dtype( const detail::DataFpType<float>& )
	{
		return Dtype::Float;
	}
	inline
	Dtype impl_dtype( const detail::DataFpType<double>& )
	{
		return Dtype::Double;
	}
	inline
	Dtype impl_dtype( const detail::DataFpType<long double>& )
	{
		return Dtype::LongDouble;
	}
	template<typename T>
	inline
	Dtype impl_dtype( const detail::DataFpType<T>& )
	{
		return Dtype::Other;
	}
#ifdef HOMOG2D_USE_TTMATH
/// Implementation for ttmath types
	template<long unsigned int M, long unsigned int E>
	inline
	Dtype impl_dtype( const detail::DataFpType<ttmath::Big<M,E>>& )
	{
		return Dtype::Ttmath;
	}
/// Implementation for ttmath types
	template<long unsigned int M, long unsigned int E>
	inline
	std::pair<int,int> impl_dsize( const detail::DataFpType<ttmath::Big<M,E>>& )
	{
		return std::make_pair( M*sizeof(size_t)*8, E*sizeof(size_t)*8 );
	}

#endif
} // namespace priv


//------------------------------------------------------------------
/// Holds threshold values and api to access these
namespace thr {

static HOMOG2D_INUMTYPE& nullDistance()
{
	static HOMOG2D_INUMTYPE s_zeroDistance = HOMOG2D_THR_ZERO_DIST;
	return s_zeroDistance;
}
static HOMOG2D_INUMTYPE& nullOrthogDistance()
{
	static HOMOG2D_INUMTYPE s_zeroOrthoDistance = HOMOG2D_THR_ZERO_ORTHO_DIST;
	return s_zeroOrthoDistance;
}
static HOMOG2D_INUMTYPE& nullAngleValue()
{
	static HOMOG2D_INUMTYPE s_zeroAngleValue = HOMOG2D_THR_ZERO_ANGLE;
	return s_zeroAngleValue;
}

static HOMOG2D_INUMTYPE& nullDenom()
{
	static HOMOG2D_INUMTYPE _zeroDenom = HOMOG2D_THR_ZERO_DENOM;
	return _zeroDenom;
}

static HOMOG2D_INUMTYPE& nullDeter()
{
	static HOMOG2D_INUMTYPE _zeroDeter = HOMOG2D_THR_ZERO_DETER;
	return _zeroDeter;
}

/// This one is used for the Welzl minimum enclosing circle
static bool& doNotCheckRadius()
{
	static bool _doNotCheckRadius = false;
	return _doNotCheckRadius;
}

/// Helper function, could be needed
inline
void printThresholds( std::ostream& f )
{
	f << "homog2d: current threshold values:"
		<< "\n  -nullDistance()="       << nullDistance()
		<< "\n  -nullOrthogDistance()=" << nullOrthogDistance()
		<< "\n  -nullAngleValue()="     << nullAngleValue()
		<< "\n  -nullDenom()="          << nullDenom()
		<< "\n  -nullDeter()="          << nullDeter()
		<< '\n';
}

} // namespace thr

// forward declaration
/// \todo 20250201: why do we need this and why isn't the same required for \c OSegment_ ?
namespace base {
template<typename LP,typename FPT>
auto
operator << ( std::ostream&, const h2d::base::LPBase<LP,FPT>& )
-> std::ostream&;

template<typename T1,typename T2>
auto
operator << ( std::ostream&, const h2d::base::PolylineBase<T1,T2>& )
-> std::ostream&;
} // namespace base

// forward declaration, related to https://github.com/skramm/homog2d/issues/2
template<typename T,typename U>
Line2d_<T>
operator * ( const Homogr_<U>&, const Line2d_<T>& );

// forward declaration
template<typename PLT,typename FPT>
std::vector<Line2d_<HOMOG2D_INUMTYPE>>
getBisectorLines( const base::PolylineBase<PLT,FPT>& );

template<typename T1,typename T2>
HOMOG2D_INUMTYPE
getAngle( const T1&, const T2& );


namespace detail {

// forward declaration
template<typename FPT1,typename FPT2,typename FPT3>
void
product( Matrix_<FPT1>&, const Matrix_<FPT2>&, const Matrix_<FPT3>& );


//------------------------------------------------------------------
/// Returns true if one of the points share a common coordinate
/// (thus making them unable to define a bounding box)
template<typename FPT1,typename FPT2>
bool
shareCommonCoord( const Point2d_<FPT1>& p1, const Point2d_<FPT2>& p2 )
{
	if(
		   homog2d_abs( p1.getX() - p2.getX() ) < thr::nullOrthogDistance()
		|| homog2d_abs( p1.getY() - p2.getY() ) < thr::nullOrthogDistance()
	)
		return true;
	return false;
}

//------------------------------------------------------------------
/// Returns true if one of the points share a common coordinate
/// (thus making them unable to define a bounding box)
template<typename FPT1,typename FPT2>
bool
shareCommonCoord( const std::pair<Point2d_<FPT1>,Point2d_<FPT2>>& ppts )
{
	return shareCommonCoord( ppts.first , ppts.second );
}

//------------------------------------------------------------------
/// Private free function, get top-left and bottom-right points from two arbitrary points
/** Throws if one of the coordinates is equal to the other (x1=x2 or y1=y2)*/
template<typename FPT>
PointPair_<FPT>
getCorrectPoints( const Point2d_<FPT>& p0, const Point2d_<FPT>& p1 )
{
#ifndef HOMOG2D_NOCHECKS
	if( shareCommonCoord( p0, p1 ) )
		HOMOG2D_THROW_ERROR_1(
			"a coordinate of the 2 points is identical, does not define a rectangle:\n p0=" << p0 << " p1=" << p1
		);
#endif
	Point2d_<FPT> p00( std::min(p0.getX(), p1.getX()), std::min(p0.getY(), p1.getY()) );
	Point2d_<FPT> p11( std::max(p0.getX(), p1.getX()), std::max(p0.getY(), p1.getY()) );
	return std::make_pair( p00, p11 );
}

/// An alias used to hold data of a 3x3 matrix, see detail::Matrix_
template<typename T>
using matrix_t = std::array<std::array<T,3>,3>;


//------------------------------------------------------------------
/// Common templated class for all the geometric primitives
template<typename FPT>
class Common
{
public:
/// Get numerical data type as a Dtype value, can be stringified with h2d::getString(Dtype)
/// \sa h2d::dtype( const T& )
	Dtype dtype() const
	{
		return priv::impl_dtype( detail::DataFpType<FPT>() );
	}

/// Get data size expressed as number of bits for, respectively, mantissa and exponent.
/// \sa h2d::dsize(const T&)
	std::pair<int,int> dsize() const
	{
		return priv::impl_dsize( detail::DataFpType<FPT>() );
	}

/// This function is a fallback for all sub-classes that do not provide such a method.
/**
It is necessary in a runtime polymorphism context, as we would have build failures if a given type
disallows providing such a method
(for example, when trying to check if some object is inside an open polyline, which makes no sense).
*/
	template<typename T>
	constexpr bool isInside( const Common<T>& ) const
	{
		HOMOG2D_START;
		return false;
	}

#if 0
/// Fallback for classes not implementing this
/// \todo 20240327: this class is inherited by HMatrix... on which the concept of bounding box makes no sense!
/// Fix this.
	template<typename T>
	FRect_<T> getBB() const
	{
		HOMOG2D_THROW_ERROR_1( "unable to compute BB for object of type " << getString(this->type) );
		return FRect_<T>(); // to avoid a compile warning
	}
#endif

	size_t size() const
	{
		return 0;
	}
}; // class Common

} // namespace detail

//------------------------------------------------------------------
#ifdef HOMOG2D_ENABLE_PRTP

/// Holds pointer-based runtime polymorphism stuff
namespace rtp {

/// Non-templated root class, to achieve dynamic (runtime) polymorphism
/**
Only exists if symbol \c HOMOG2D_ENABLE_PRTP is defined, see
<a href="md_docs_homog2d_manual.html#build_options">build options</a>.
*/
class Root
{
public:
	virtual void draw( img::Image<img::SvgImage>&, img::DrawParams dp=img::DrawParams() ) const = 0;
#ifdef HOMOG2D_USE_OPENCV
	virtual void draw( img::Image<cv::Mat>&, img::DrawParams dp=img::DrawParams() ) const = 0;
#endif
	virtual HOMOG2D_INUMTYPE length() const = 0;
	virtual HOMOG2D_INUMTYPE area()   const = 0;
	virtual Type             type()   const = 0;
	virtual size_t           size()   const = 0;

	friend std::ostream& operator << ( std::ostream& f, const Root& p );
	virtual ~Root() {}
};

} // namespace rtp

#endif

namespace detail {
//------------------------------------------------------------------
/// A simple wrapper over a 3x3 matrix, provides root functionalities
/**
Homogeneous (thus the 'mutable' attribute).

\todo 20240326: we might need to add another level of inheritance.
This class inherits \c Common, which is designed to be inherited geometric primitives and as such holds
member function that cannot be used on a matrix !
(example: isInside() )
<br>
So either we remove the latter function and find a way to put it somewhere else, either we create another intermediate class.
*/
template<typename FPT>
class Matrix_: public Common<FPT>
{
	template<typename T> friend class Matrix_;

	template<typename T1,typename T2,typename FPT1,typename FPT2>
	friend void
	product( base::LPBase<T1,FPT1>&, const detail::Matrix_<FPT2>&, const base::LPBase<T2,FPT1>& );

	template<typename FPT1,typename FPT2,typename FPT3>
	friend void
	product( Matrix_<FPT1>&, const Matrix_<FPT2>&, const Matrix_<FPT3>& );

protected:
	mutable matrix_t<FPT> _mdata;
	mutable bool          _isNormalized = false;

public:
/// Constructor
	Matrix_()
	{
		p_fillZero();
	}

/// Copy-Constructor
	template<typename FPT2>
	Matrix_( const Matrix_<FPT2>& other )
	{
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				_mdata[i][j] = other._mdata[i][j];
		_isNormalized = other._isNormalized;
	}

	matrix_t<FPT>&       getRaw()       { return _mdata; }
	const matrix_t<FPT>& getRaw() const { return _mdata; }

	template<typename T>
	void set( size_t r, size_t c, T v )
	{
		#ifndef HOMOG2D_NOCHECKS
			HOMOG2D_CHECK_ROW_COL;
		#endif
		_mdata[r][c] = v;
	}

	const FPT& value( size_t r, size_t c ) const
	{
		#ifndef HOMOG2D_NOCHECKS
			HOMOG2D_CHECK_ROW_COL;
		#endif
		return _mdata[r][c];
	}
	FPT& value( size_t r, size_t c )
	{
		#ifndef HOMOG2D_NOCHECKS
			HOMOG2D_CHECK_ROW_COL;
		#endif
		return _mdata[r][c];
	}

/// Return determinant of matrix
/**
See https://en.wikipedia.org/wiki/Determinant
*/
	HOMOG2D_INUMTYPE determ() const
	{
		auto det = _mdata[0][0] * p_det2x2( {1,1, 1,2, 2,1, 2,2} );
		det     -= _mdata[0][1] * p_det2x2( {1,0, 1,2, 2,0, 2,2} );
		det     += _mdata[0][2] * p_det2x2( {1,0, 1,1, 2,0, 2,1} );
		return det;
	}

/// Transpose and return matrix
	Matrix_& transpose()
	{
		matrix_t<FPT> out;
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				out[i][j] = _mdata[j][i];
		_mdata = out;
		_isNormalized = false;
		return *this;
	}

/// Inverse matrix
	Matrix_& inverse()
	{
		auto det = determ();
		if( homog2d_abs(det) < thr::nullDeter() )
			HOMOG2D_THROW_ERROR_1( "matrix is not invertible, det=" << std::scientific << homog2d_abs(det) );

		auto adjugate = p_adjugate();
		p_divideAll(adjugate, det);
		_mdata = adjugate._mdata;
		_isNormalized = false;
		return *this;
	}
	bool isNormalized() const { return _isNormalized; }

protected:
	void p_normalizeMat( int r, int c ) const
	{
#ifndef HOMOG2D_NOCHECKS
		if( std::fabs(_mdata[r][c]) < thr::nullDenom() )
			HOMOG2D_THROW_ERROR_1(
				"Unable to normalize matrix, value at ("
					<< r << ',' << c << ") less than " << thr::nullDenom()
			);
#endif
		p_divideBy( r, c );
		if( std::signbit( _mdata[r][c] ) )
			for( auto& li: _mdata )
				for( auto& e: li )
					e = -e;
		_isNormalized = true;
	}

/// Divide all elements by the value at (r,c), used for normalization.
/** No need to check value, done by caller */
	void p_divideBy( size_t r, size_t c ) const
	{
//		assert( std::fabs( _mdata[r][c] ) > 1000*std::numeric_limits<FPT>::epsilon() );
		for( auto& li: _mdata )
			for( auto& e: li )
				e /= _mdata[r][c];
	}

	void p_fillZero()
	{
		for( auto& li: _mdata )
			for( auto& e: li )
				e = 0.;
	}
	void p_fillEye()
	{
		p_fillZero();
		_mdata[0][0] = 1.;
		_mdata[1][1] = 1.;  // "eye" matrix => unit transformation
		_mdata[2][2] = 1.;
	}

	template<typename T>
	void p_fillWith( const T& in )
	{
		for( auto i=0; i<3; i++ )
			for( auto j=0; j<3; j++ )
				_mdata[i][j] = in[i][j];
		_isNormalized = false;
	}

/// Divide all elements of \c mat by \c value
	template<typename FPT2>
	void p_divideAll( detail::Matrix_<FPT>& mat, FPT2 value ) const
	{
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				mat._mdata[i][j] /= value;
		_isNormalized = false;
	}
	template<typename T1,typename T2>
	friend Matrix_<T1> operator * ( const Matrix_<T1>&, const Matrix_<T2>& );

private:
	HOMOG2D_INUMTYPE p_det2x2( const std::vector<int>& v ) const
	{
		auto det = static_cast<HOMOG2D_INUMTYPE>( _mdata[v[0]][v[1]] ) * _mdata[v[6]][v[7]];
		det -=     static_cast<HOMOG2D_INUMTYPE>( _mdata[v[2]][v[3]] ) * _mdata[v[4]][v[5]];
		return det;
	}

/// Computes adjugate matrix, see https://en.wikipedia.org/wiki/Adjugate_matrix#3_%C3%97_3_generic_matrix
	detail::Matrix_<FPT> p_adjugate() const
	{
		detail::Matrix_<FPT> mat_out;
		matrix_t<FPT>& out = mat_out._mdata;

		out[ 0 ][ 0 ] =  p_det2x2( {1,1, 1,2, 2,1, 2,2} );
		out[ 0 ][ 1 ] = -p_det2x2( {0,1, 0,2, 2,1, 2,2} );
		out[ 0 ][ 2 ] =  p_det2x2( {0,1, 0,2, 1,1, 1,2} );

		out[ 1 ][ 0 ] = -p_det2x2( {1,0, 1,2, 2,0, 2,2} );
		out[ 1 ][ 1 ] =  p_det2x2( {0,0, 0,2, 2,0, 2,2} );
		out[ 1 ][ 2 ] = -p_det2x2( {0,0, 0,2, 1,0, 1,2} );

		out[ 2 ][ 0 ] =  p_det2x2( {1,0, 1,1, 2,0, 2,1} );
		out[ 2 ][ 1 ] = -p_det2x2( {0,0, 0,1, 2,0, 2,1} );
		out[ 2 ][ 2 ] =  p_det2x2( {0,0, 0,1, 1,0, 1,1} );

		return mat_out;
	}

	friend std::ostream&
	operator << ( std::ostream& f, const Matrix_& h )
	{
		for( const auto& li: h._mdata )
		{
			f << "| ";
			for( const auto& e: li )
				f << std::setw(6) << e << ' ';
			f << " |\n";
		}
		return f;
	}

}; // class Matrix_

template<typename T1,typename T2,typename FPT1,typename FPT2>
void
product( base::LPBase<T1,FPT1>&, const detail::Matrix_<FPT2>&, const base::LPBase<T2,FPT1>& );

template<typename T1,typename T2>
Matrix_<T1> operator * ( const Matrix_<T1>& h1, const Matrix_<T2>& h2 )
{
//	HOMOG2D_START;
	Matrix_<T1> out;
	product( out, h1, h2 );
	return out;
}

} // namespace detail

namespace trait {
template<typename> struct IsBigNumType : std::false_type {};
#ifdef HOMOG2D_USE_TTMATH
template<int T1,int T2> struct IsBigNumType<ttmath::Big<T1,T2>> : std::true_type {};
#endif

} // namespace trait

//------------------------------------------------------------------
/// A 2D homography, defining a planar transformation
/**
To define an affine or rigid transformation, you can use:
- setRotation()
- setTranslation()
- setScale()

To add an affine or rigid transformation to the current one, you can use:
- addRotation()
- addTranslation()
- addScale()

To return to unit transformation, use init()

Implemented as a 3x3 matrix

Templated by Floating-Point Type (FPT) and by type M (typ::IsEpipmat or typ::IsHomogr)
 */
template<typename M,typename FPT>
class Hmatrix_ : public detail::Matrix_<FPT>
{
	template<typename T1,typename T2> friend class LPBase;

	template<typename T,typename U>
	friend Line2d_<T>
	operator * ( const Homogr_<U>&, const Line2d_<T>& );

	template<typename T,typename U>
	friend Point2d_<T>
	operator * ( const Homogr_<U>&, const Point2d_<T>& );

	template<typename T,typename U,typename V>
	friend base::LPBase<typename detail::HelperPL<T>::OtherType,V>
	operator * ( const Hmatrix_<typ::IsEpipmat,U>& h, const base::LPBase<T,V>& in );

public:

/// \name Constructors
///@{

	/// Default constructor, initialize to unit transformation
	Hmatrix_()
	{
		init();
	}

/// Constructor, set homography to a rotation matrix of angle \c val
	template<typename T>
	explicit Hmatrix_( T val )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		setRotation( val );
	}

/// Constructor, set homography to a translation matrix ( see Hmatrix_( T ) )
	template<typename T1,typename T2>
	explicit Hmatrix_( T1 tx, T2 ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		init();
		setTranslation( tx, ty );
	}

/// Constructor, used to fill with a vector of vector matrix
/** \warning
- Input matrix \b must be 3 x 3, but type can be anything that can be copied to \c double
- no checking is done on validity of matrix as an homography.
Thus some assert can get triggered elsewhere.
*/
	template<typename T>
	Hmatrix_( const std::vector<std::vector<T>>& in )
	{
#ifndef HOMOG2D_NOCHECKS
		HOMOG2D_CHECK_IS_NUMBER(T);
		if( in.size() != 3 )
			HOMOG2D_THROW_ERROR_1( "Invalid line size for input: " << in.size() );
		for( auto li: in )
			if( li.size() != 3 )
				HOMOG2D_THROW_ERROR_1( "Invalid column size for input: " << li.size() );
#endif
		detail::Matrix_<FPT>::p_fillWith( in );
		normalize();
	}

/// Constructor, used to fill with a std::array
	template<typename T>
	Hmatrix_( const std::array<std::array<T,3>,3>& in )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		detail::Matrix_<FPT>::p_fillWith( in );
		normalize();
	}

/// Copy-constructor
	Hmatrix_( const Hmatrix_<M,FPT>& other )
		: detail::Matrix_<FPT>( other)
		, _hasChanged   ( true )
		, _hmt          (  nullptr )
	{
		detail::Matrix_<FPT>::getRaw() = other.getRaw();
	}

#ifdef HOMOG2D_USE_OPENCV
/// Constructor used to initialise with a cv::Mat, call the assignment operator
	Hmatrix_( const cv::Mat& mat )
	{
		*this = mat;
	}
#endif

///@}

/// Assignment operator
	Hmatrix_& operator = ( const Hmatrix_<M,FPT>& other )
	{
		if( this != &other )
			detail::Matrix_<FPT>::getRaw() = other.getRaw();
		_hasChanged = true;
		return *this;
	}

/// Inverse matrix
	Hmatrix_& inverse()
	{
		detail::Matrix_<FPT>::inverse();
		normalize();
		return *this;
	}

#if 0
/// Setter \warning No normalization is done, as this can be done
/// several times to store values, we therefore must not normalize in between
	template<typename T>
	void set(
		size_t r, ///< row
		size_t c, ///< col
		T      v  ///< value
	)
	{
		#ifndef HOMOG2D_NOCHECKS
			HOMOG2D_CHECK_ROW_COL;
		#endif
		_data[r][c] = v;
		_isNormalized = false;
		_hasChanged = true;
	}

/// Getter
	FPT get( size_t r, size_t c ) const
	{
		#ifndef HOMOG2D_NOCHECKS
			HOMOG2D_CHECK_ROW_COL;
		#endif
		return _data[r][c];
	}

	detail::Matrix_<FPT>&       getMat()       { return static_cast<detail::Matrix_<FPT>>(*this); }
	const detail::Matrix_<FPT>& getMat() const { return static_cast<detail::Matrix_<FPT>>(*this); }
#endif // 0

	void init()
	{
		impl_mat_init0( detail::BaseHelper<M>() );
	}

/// \name Adding/assigning a transformation
///@{

/// Adds a translation \c tx,ty to the matrix
	template<typename T>
	Hmatrix_& addTranslation( T tx, T ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		Hmatrix_ out;
		out.setTranslation( tx, ty );
		*this = out * *this;
		return *this;
	}
/// Sets the matrix as a translation \c tx,ty
	template<typename T1,typename T2>
	Hmatrix_& setTranslation( T1 tx, T2 ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		init();
		auto& mat = detail::Matrix_<FPT>::_mdata;
		mat[0][2] = tx;
		mat[1][2] = ty;
		detail::Matrix_<FPT>::_isNormalized = true;
		_hasChanged = true;
		return *this;
	}
/// Adds a rotation with an angle \c theta (radians) to the matrix
	template<typename T>
	Hmatrix_& addRotation( T theta )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		Hmatrix_ out;
		out.setRotation( theta );
		*this = out * *this;
		return *this;
	}
/// Sets the matrix as a rotation with an angle \c theta (radians)
	template<typename T>
	Hmatrix_& setRotation( T theta )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		auto& mat = detail::Matrix_<FPT>::_mdata;
		init();
		mat[0][0] = mat[1][1] = std::cos(theta);
		mat[1][0] = std::sin(theta);
		mat[0][1] = -mat[1][0];
		detail::Matrix_<FPT>::_isNormalized = true;
		_hasChanged = true;
		return *this;
	}
/// Adds the same scale factor to the matrix
	template<typename T>
	Hmatrix_& addScale( T k )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		return this->addScale( k, k );
	}
/// Adds a scale factor to the matrix
	template<typename T1,typename T2>
	Hmatrix_& addScale( T1 kx, T2 ky )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		Hmatrix_ out;
		out.setScale( kx, ky );
		*this = out * *this;
		_hasChanged = true;
		return *this;
	}
/// Sets the matrix as a scaling transformation (same on two axis)
	template<typename T>
	Hmatrix_& setScale( T k )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		return setScale( k, k );
	}
/// Sets the matrix as a scaling transformation
	template<typename T1,typename T2>
	Hmatrix_& setScale( T1 kx, T2 ky )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		init();
		auto& mat = detail::Matrix_<FPT>::_mdata;
		mat[0][0] = kx;
		mat[1][1] = ky;
		detail::Matrix_<FPT>::_isNormalized = true;
		_hasChanged = true;
		return *this;
	}
///@}

	template<typename T>
	void applyTo( T& ) const;

#ifdef HOMOG2D_USE_OPENCV
	void copyTo( cv::Mat&, int type=CV_64F ) const;
	Hmatrix_& operator = ( const cv::Mat& );
#endif

/// Homography normalisation
/// (const because flag \c _hasChanged declared as \c mutable)
	void normalize() const
	{
		detail::Matrix_<FPT>::p_normalizeMat(2,2);
		_hasChanged = true;
	}

	void buildFrom4Points( const std::vector<Point2d_<FPT>>&, const std::vector<Point2d_<FPT>>&, int method=1 );

/// Matrix multiplication, call the base class product
	friend Hmatrix_ operator * ( const Hmatrix_& h1, const Hmatrix_& h2 )
	{
		Hmatrix_ out;
		detail::product( out, static_cast<detail::Matrix_<FPT>>(h1), static_cast<detail::Matrix_<FPT>>(h2) ) ;
		out.normalize();
		out._hasChanged = true;
		return out;
	}

/// Comparison operator. Does normalization if required
/**
This does an absolute comparison of all matrix elements, one by one,
and if one differs more than the threshold, it will return false
*/
	bool operator == ( const Hmatrix_& h ) const
	{
		auto& data = detail::Matrix_<FPT>::_mdata;

		if( !detail::Matrix_<FPT>::isNormalized() )
			normalize();
		if( !h.isNormalized() )
			h.normalize();

		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				if( std::fabs(
					static_cast<HOMOG2D_INUMTYPE>( data[i][j] ) - h.value(i,j) )
					>= thr::nullDeter()
				)
					return false;
		return true;
	}

/// Comparison operator. Does normalization if required
	bool operator != ( const Hmatrix_& h ) const
	{
		return !(*this == h);
	}

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////

private:
#ifdef HOMOG2D_FUTURE_STUFF
/// Implementation for epipolar matrices: initialize to aligned axis
	void impl_mat_init0( const detail::BaseHelper<typ::IsEpipmat>& )
	{
		_data.fillZero();
		_data[2][1] = 1.;
		_data[1][2] = 1.;
		_isNormalized = true;
	}
#endif

/// Implementation for homographies: initialize to unit transformation
	void impl_mat_init0( const detail::BaseHelper<typ::IsHomogr>& )
	{
		detail::Matrix_<FPT>::p_fillEye();
		detail::Matrix_<FPT>::_isNormalized = true;
	}

//////////////////////////
//      DATA SECTION    //
//////////////////////////
private:
	mutable bool _hasChanged   = true;
	mutable std::unique_ptr<detail::Matrix_<FPT>> _hmt; ///< used to store \f$ H^{-1} \f$, but only if required

	friend std::ostream& operator << ( std::ostream& f, const Hmatrix_& h )
	{
		f << "Hmatrix:\n"
			<< static_cast<const detail::Matrix_<FPT>&>(h);
		return f;
	}

}; // class Hmatrix_


//------------------------------------------------------------------
/// Holds traits classes
namespace trait {

/// Traits class, used in generic draw() function
/**
\todo 20240504 this is only used in the draw() free function but actually not really needed:
as that function just calls the member function,
we can just let the build fail is no suitable member function is found.
*/
template<typename T> struct IsDrawable              : std::false_type {};
template<typename T> struct IsDrawable<Circle_<T>>  : std::true_type  {};
template<typename T> struct IsDrawable<FRect_<T>>   : std::true_type  {};
template<typename T> struct IsDrawable<Segment_<T>> : std::true_type  {};
template<typename T> struct IsDrawable<OSegment_<T>>  : std::true_type{};
template<typename T> struct IsDrawable<Line2d_<T>>  : std::true_type  {};
template<typename T> struct IsDrawable<Point2d_<T>> : std::true_type  {};
template<typename T> struct IsDrawable<Ellipse_<T>> : std::true_type  {};
template<typename T1,typename T2> struct IsDrawable<base::PolylineBase<T1,T2>>: std::true_type  {};

/// Traits class, used in intersects() for Polyline
template<typename T> struct IsShape              : std::false_type {};
template<typename T> struct IsShape<Circle_<T>>  : std::true_type  {};
template<typename T> struct IsShape<FRect_<T>>   : std::true_type  {};
template<typename T> struct IsShape<Segment_<T>> : std::true_type  {};
template<typename T> struct IsShape<OSegment_<T>>: std::true_type  {};
template<typename T> struct IsShape<Line2d_<T>>  : std::true_type  {};
template<typename T1,typename T2> struct IsShape<base::PolylineBase<T1,T2>>: std::true_type  {};
//template<typename T> struct IsShape<Ellipse_<T>>:  std::true_type  {};

/// Traits class, used to determine if we can use some "isInside()" function
template<typename T> struct HasArea              : std::false_type {};
template<typename T> struct HasArea<Circle_<T>>  : std::true_type  {};
template<typename T> struct HasArea<FRect_<T>>   : std::true_type  {};
template<typename T> struct HasArea<Ellipse_<T>> : std::true_type  {};
template<typename T> struct HasArea<base::PolylineBase<typename typ::IsClosed,T>>: std::true_type  {};

/// This one is used in base;;PolylineBase::isInside()
/// \todo 20250131: probably useless, check if this can be replaced by some "if constexpr"
template<typename T> struct PolIsClosed                                              : std::false_type {};
template<typename T> struct PolIsClosed<base::PolylineBase<typename typ::IsClosed,T>>: std::true_type  {};


/// Traits class used in operator * ( const Hmatrix_<typ::IsHomogr,FPT>& h, const Cont& vin ),
/// used to detect if container is valid
template <typename T>               struct IsContainer                     : std::false_type{};
template <typename T,std::size_t N> struct IsContainer<std::array<T,N>>    : std::true_type {};
template <typename... Ts>           struct IsContainer<std::vector<Ts...>> : std::true_type {};
template <typename... Ts>           struct IsContainer<std::list<Ts...  >> : std::true_type {};

/// Traits class used to detect if container \c T is a \c std::array
/** (because allocation is different, see \ref alloc() ) */
template <typename T>           struct IsArray                   : std::false_type {};
template <typename V, size_t n> struct IsArray<std::array<V, n>> : std::true_type {};

/// Traits class, used for getBB() set of functions
template<class>   struct IsSegment              : std::false_type {};
template<class T> struct IsSegment<Segment_<T>> : std::true_type  {};
template<class>   struct IsPoint                : std::false_type {};
template<class T> struct IsPoint<Point2d_<T>>   : std::true_type  {};

/// Traits class, used to check if type has a Bounding Box
/**
Difference with \c trait::HasArea is that this one is true for OPolyline, whereas the other is not
*/
template<class>   struct HasBB              : std::false_type {};
template<class T> struct HasBB<Ellipse_<T>> : std::true_type {};
template<class T> struct HasBB<FRect_<T>>   : std::true_type {};
template<class T> struct HasBB<Circle_<T>>  : std::true_type {};
template<typename T1,typename T2> struct HasBB<base::PolylineBase<T1,T2>>: std::true_type  {};

#ifdef HOMOG2D_ENABLE_VRTP
template<typename T>       struct IsVariant                       : std::false_type {};
template<typename ...Args> struct IsVariant<std::variant<Args...>>: std::true_type {};
#endif

} // namespace trait


//------------------------------------------------------------------
namespace detail {

/// Holds 9 parameters of Ellipse_
template<typename T>
struct EllParams
{
	template<typename U> friend struct EllParams;

	EllParams() = default;

	/// Copy-constructor is needed when converting from one type to another type
	template<typename U>
	EllParams( const EllParams<U>& p )
	{
		x0   = p.x0;
		y0   = p.y0;
		theta= p.theta;
		sint = p.sint;
		cost = p.cost;
		a    = p.a;
		b    = p.b;
		a2   = p.a2;
		b2   = p.b2;
	}

	T x0, y0;     ///< center
	T theta = 0.; ///< angle
	T sint, cost; ///< \f$ \sin( \theta), \cos( \theta) \f$
	T a, b;
	T a2, b2;     ///< squared values of a and b

	template<typename U>
	friend std::ostream& operator << ( std::ostream& f, const EllParams<U>& par );
};

template<typename U>
std::ostream& operator << ( std::ostream& f, const EllParams<U>& par )
{
	f << "EllParams: origin=" << par.x0 << "," << par.y0
		<< " angle=" << par.theta *180./M_PI
		<< " a=" << par.a << " b=" << par.b
		<< ' ';
	return f;
}

} // namespace detail

//------------------------------------------------------------------
/// Ellipse as a conic in matrix form.
/**
This enables its projection using homography

See:
- https://en.wikipedia.org/wiki/Ellipse#General_ellipse
- https://en.wikipedia.org/wiki/Matrix_representation_of_conic_sections

General equation of an ellipse:
\f[
A x^2 + B x y + C y^2 + D x + E y + F = 0
\f]
It can be written as a 3 x 3 matrix:
\f[
\begin{bmatrix}
  A & B/2 & D/2 \\
  B/2 & C & E/2 \\
  D/2 & E/2 & F
\end{bmatrix}
\f]

Matrix coefficients computed from center x0,y0, major and minor distances (a,b) and angle theta:
\f[
\begin{aligned}
  A &=   a^2 \sin^2\theta + b^2 \cos^2\theta \\
  B &=  2\left(b^2 - a^2\right) \sin\theta \cos\theta \\
  C &=   a^2 \cos^2\theta + b^2 \sin^2\theta \\
  D &= -2A x_\circ   -  B y_\circ \\
  E &= - B x_\circ   - 2C y_\circ \\
  F &=   A x_\circ^2 +  B x_\circ y_\circ + C y_\circ^2 - a^2 b^2
\end{aligned}
\f]

Homography projection: https://math.stackexchange.com/a/2320082/133647

\f[
Q' = H^{-T} \cdot Q \cdot H^{-1}
\f]

*/
template<typename FPT>
class Ellipse_: public detail::Matrix_<FPT>
#ifdef HOMOG2D_ENABLE_PRTP
, public rtp::Root
#endif
{
public:
	using FType = FPT;
	using detail::Common<FPT>::isInside;
	using SType = typ::T_Ellipse;

	Type type() const
	{
		return Type::Ellipse;
	}

	template<typename T> friend class Ellipse_;

	template<typename FPT1,typename FPT2>
	friend Ellipse_<FPT1>
	operator * ( const Homogr_<FPT2>&, const Circle_<FPT1>& );

	template<typename FPT1,typename FPT2>
	friend Ellipse_<FPT1>
	operator * ( const Homogr_<FPT2>&, const Ellipse_<FPT1>& );

public:
/// \name Constructors
///@{

/// Default constructor: centered at (0,0), major=2, minor=1
	Ellipse_(): Ellipse_( 0., 0., 2., 1., 0. )
	{}

/// Constructor 1
	template<typename T1,typename T2=double,typename T3=double>
	Ellipse_( const Point2d_<T1>& pt, T2 major=2., T2 minor=1., T3 angle=0. )
		: Ellipse_( pt.getX(), pt.getY(), major, minor, angle )
	{}

/// Constructor 2
	template<typename T1,typename T2=double,typename T3=double>
	explicit Ellipse_( T1 x, T1 y, T2 major=2., T2 minor=1., T3 angle=0. )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		HOMOG2D_CHECK_IS_NUMBER(T3);
		if( major<minor )
			std::swap( major, minor );
		p_init( x, y, major, minor, angle );
	}

/// Constructor 3, import from circle
	explicit Ellipse_( const Circle_<FPT>& cir )
	{
		p_init( cir.center().getX(), cir.center().getY(), cir.radius(), cir.radius(), 0. );
	}

/// Copy-Constructor
	template<typename FPT2>
	Ellipse_( const Ellipse_<FPT2>& other )
		: detail::Matrix_<FPT>( other )
	{}
///@}

/// Translate Ellipse
	template<typename TX, typename TY>
	void translate( TX dx, TY dy )
	{
		HOMOG2D_CHECK_IS_NUMBER( TX );
		HOMOG2D_CHECK_IS_NUMBER( TY );
		auto par = p_getParams<HOMOG2D_INUMTYPE>();
		p_init( par.x0+dx, par.y0+dy, par.a, par.b, par.theta );
	}
/// Translate Ellipse
	template<typename T1, typename T2>
	void translate( const std::pair<T1,T2>& ppt )
	{
		this->translate( ppt.first, ppt.second );
	}

/// Move Ellipse to new location
	template<typename TX, typename TY>
	void moveTo( TX x, TY y )
	{
		HOMOG2D_CHECK_IS_NUMBER( TX );
		HOMOG2D_CHECK_IS_NUMBER( TY );
		auto par = p_getParams<HOMOG2D_INUMTYPE>();
		p_init( x, y, par.a, par.b, par.theta );
	}

/// Move Ellipse to new location, given by \c new_org
	template<typename T1>
	void moveTo( const Point2d_<T1>& new_org )
	{
		this->moveTo( new_org.getX(), new_org.getY() );
	}

/// \name attributes of ellipse
///@{

/// Returns 1
	constexpr size_t size() const
	{
		return 1;
	}

	bool isCircle( HOMOG2D_INUMTYPE thres=1.E-10 )           const;
	Point2d_<FPT>                                getCenter() const;
	CPolyline_<FPT>                              getOBB()    const;
	auto                                         getBB()     const;
	HOMOG2D_INUMTYPE                             angle()     const;
	std::pair<HOMOG2D_INUMTYPE,HOMOG2D_INUMTYPE> getMajMin() const;

///@}

/// Area of ellipse
	HOMOG2D_INUMTYPE area() const
	{
		auto par = p_getParams<HOMOG2D_INUMTYPE>();
		return M_PI * par.a * par.b;
	}
// Circumference
	HOMOG2D_INUMTYPE length() const;

	std::pair<Line2d_<FPT>,Line2d_<FPT>> getAxisLines() const;

	template<typename FPT2>
	bool pointIsInside( const Point2d_<FPT2>& ) const;

//////////////////////////
//       OPERATORS      //
//////////////////////////

	template<typename T>
	friend std::ostream&
	operator << ( std::ostream& f, const Ellipse_<T>& ell );

/// Comparison operator. Does normalization if required
	bool operator == ( const Ellipse_& h ) const
	{
		auto& data = detail::Matrix_<FPT>::_mdata;

		if( !detail::Matrix_<FPT>::isNormalized() )
			detail::Matrix_<FPT>::p_normalizeMat(2,2);
		if( !h.isNormalized() )
			h.p_normalizeMat(2,2);

		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				if( std::fabs(
					static_cast<HOMOG2D_INUMTYPE>( data[i][j] ) - h.value(i,j) )
					>= thr::nullDeter()
				)
					return false;
		return true;
	}

/// Comparison operator. Does normalization if required
	bool operator != ( const Ellipse_& e ) const
	{
		return !(*this == e);
	}

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////
private:
/// private constructor, needed in friend function only
/**
This is not public, because: 1-useless, 2-no guarantee would be given that the input
is indeed a valid ellipse
*/
	explicit Ellipse_( const detail::Matrix_<FPT>& mat ): detail::Matrix_<FPT>( mat )
	{}

	template<typename T>
	detail::EllParams<T> p_getParams() const;
	template<typename T>
	detail::EllParams<T> p_computeParams() const;

/// Called by all the constructors, fills the matrix.
	void p_init( double x0, double y0, double a, double b, double theta=0. )
	{
		auto& data = detail::Matrix_<FPT>::_mdata;
		HOMOG2D_INUMTYPE sin1 = std::sin(theta);
		HOMOG2D_INUMTYPE cos1 = std::cos(theta);
		HOMOG2D_INUMTYPE sin2 = sin1 * sin1;
		HOMOG2D_INUMTYPE cos2 = cos1 * cos1;
		HOMOG2D_INUMTYPE a2   = a*a;
		HOMOG2D_INUMTYPE b2   = b*b;

		HOMOG2D_INUMTYPE A = a2*sin2 + b2*cos2;
		HOMOG2D_INUMTYPE B = (HOMOG2D_INUMTYPE)2.*(b2-a2) * std::sin(theta) * std::cos(theta);
		HOMOG2D_INUMTYPE C = a2 * cos2 + b2 * sin2;
		HOMOG2D_INUMTYPE D = (HOMOG2D_INUMTYPE)(-2.)*A * x0 -    B * y0;
		HOMOG2D_INUMTYPE E =   - B * x0 - (HOMOG2D_INUMTYPE)2.*C * y0;
		HOMOG2D_INUMTYPE F = A*x0*x0 + B*x0*y0 + C*y0*y0 - a2*b2;

		data[0][0] = A;
		data[1][1] = C;
		data[2][2] = F;

		data[0][1] = data[1][0] = B / 2.;
		data[0][2] = data[2][0] = D / 2.;
		data[1][2] = data[2][1] = E / 2.;

#ifdef HOMOG2D_OPTIMIZE_SPEED
		_epHasChanged = false;
		_par.a = a;
		_par.b = b;
		_par.a2 = a2;
		_par.b2 = b2;
		_par.theta = theta;
		_par.sint = sin1;
		_par.cost = cos1;
		_par.x0 = x0;
		_par.y0 = y0;
#endif
	}

public:
#ifdef HOMOG2D_USE_OPENCV
	void draw( img::Image<cv::Mat>&,       img::DrawParams dp=img::DrawParams() ) const;
#endif
	void draw( img::Image<img::SvgImage>&, img::DrawParams dp=img::DrawParams() ) const;

//////////////////////////
//      DATA SECTION    //
//////////////////////////
private:
// (matrix holding the data is inherited from base class)
#ifdef HOMOG2D_OPTIMIZE_SPEED
	mutable bool _epHasChanged = true;   ///< if true, means we need to recompute parameters
	mutable detail::EllParams<FPT> _par;
#endif
}; // class Ellipse


//------------------------------------------------------------------
namespace detail {

// forward declaration of template instanciation
template<typename T1,typename T2,typename FPT1,typename FPT2>
base::LPBase<T1,FPT1>
crossProduct( const base::LPBase<T2,FPT1>&, const base::LPBase<T2,FPT2>& );

template<typename FPT1,typename FPT2>
HOMOG2D_INUMTYPE
crossProductV( const base::SegVec<typ::IsOSeg,FPT1>&, const base::SegVec<typ::IsOSeg,FPT2>& );

class Inters_1 {};
class Inters_2 {};

/// Common stuff for intersection code
struct IntersectCommon
{
protected:
	bool _doesIntersect = false;
public:
	bool operator()() const
	{
		return _doesIntersect;
	}
};

/// Base class for intersection, gets specialized
template<typename T,typename FPT>
class Intersect {};

//------------------------------------------------------------------
/// One point intersection
template<typename FPT>
class Intersect<Inters_1,FPT>: public IntersectCommon
{
	template<typename T,typename U> friend class h2d::base::SegVec;

	public:
		Point2d_<FPT>
		get() const
		{
			if( !_doesIntersect )
				HOMOG2D_THROW_ERROR_1( "No intersection points" );
			return _ptIntersect;
		}
		void set( const Point2d_<FPT>& pt )
		{
			_ptIntersect = pt;
			_doesIntersect = true;
		}
		size_t size() const { return _doesIntersect?1:0; }

		Intersect() {}
		Intersect( const Point2d_<FPT>& ptInter )
			: _ptIntersect(ptInter)
		{
			_doesIntersect = true;
		}
/// To enable conversions from different floating-point types
		template<typename FPT2>
		Intersect( const Intersect<Inters_1,FPT2>& other )
			: IntersectCommon( other )
		{
			_ptIntersect   = other._ptIntersect;
		}

	private:
		Point2d_<FPT> _ptIntersect;
};

//------------------------------------------------------------------
/// Two points intersection
template<typename FPT>
class Intersect<Inters_2,FPT>: public IntersectCommon
{
	template<typename U,typename V>
	friend class ::h2d::base::LPBase;

	public:
		Intersect() {}
		Intersect( const Point2d_<FPT>& p1, const Point2d_<FPT>& p2 )
			: _ptIntersect_1(p1), _ptIntersect_2(p2)
		{
				_doesIntersect = true;
		}
/// To enable conversions from different floating-point types
		template<typename FPT2>
		Intersect( const Intersect<Inters_2,FPT2>& other )
			: IntersectCommon( other )
		{
			auto ppts = other.get();
			_ptIntersect_1 = ppts.first;
			_ptIntersect_2 = ppts.second;
		}
		size_t size() const { return _doesIntersect?2:0; }

		PointPair_<FPT>
		get() const
		{
			if( !_doesIntersect )
				HOMOG2D_THROW_ERROR_1( "No intersection points" );
			return std::make_pair( _ptIntersect_1, _ptIntersect_2 );
		}

	private:
		Point2d_<FPT> _ptIntersect_1, _ptIntersect_2;

	friend std::ostream& operator << ( std::ostream& f, const Intersect<Inters_2,FPT>& inters )
	{
		f << "bool=" << inters._doesIntersect
			<< " p1:" << inters._ptIntersect_1
			<< " p2:" << inters._ptIntersect_2;
		return f;
	}
};

//------------------------------------------------------------------
/// Multiple points intersections
template<typename FPT>
class IntersectM
{
private:
	mutable std::vector<Point2d_<FPT>> _vecInters; ///< mutable, because it can get sorted in const functions
public:
	IntersectM() {}
/// To enable conversions from different floating-point types
	template<typename FPT2>
	IntersectM( const IntersectM<FPT2>& other )
	{
		_vecInters.resize( other.size() );
		auto it = _vecInters.begin();
		for( const auto& elem: other.get() )
			*it++ = elem; // automatic type conversion
	}

	bool operator()() const
	{
		return !_vecInters.empty();
	}
	size_t size() const { return _vecInters.size(); }
	void add( const Point2d_<FPT>& pt )
	{
		_vecInters.push_back(pt);
	}

	void add( const std::vector<Point2d_<FPT>>& vpt )
	{
		for( const auto& pt: vpt )
			_vecInters.push_back(pt);
	}

/// Returns the intersection points, sorted
	std::vector<Point2d_<FPT>> get() const
	{
		std::sort( std::begin(_vecInters), std::end(_vecInters) );
		return _vecInters;
	}
	friend std::ostream&
	operator << ( std::ostream& f, const IntersectM& i )
	{
		f << "IntersectM: size=" << i.size() << '\n' << i._vecInters;
		return f;
	}
};

//------------------------------------------------------------------
/// Helper class, holds result of intersections of two FRect_
/// \sa FRect_::intersectArea()
/**
FPT: Floating Point Type, is defined by the rectangle on which member function \c intersectArea() is called.
*/
template<typename FPT>
class RectArea
{
private:
	bool        _success = false;
	FRect_<FPT> _area;

public:
	RectArea() = default;
	RectArea( const FRect_<FPT>& r ) : _success(true), _area(r)
	{}
	bool operator()() const
	{
		return _success;
	}

	FRect_<FPT> get() const
	{
		if( !_success )
			HOMOG2D_THROW_ERROR_1( "unable, no intersection between the two rectangles" );
		return _area;
	}
};

} // namespace detail

//------------------------------------------------------------------
/// A Flat Rectangle, modeled by its two opposite points
template<typename FPT>
class FRect_: public detail::Common<FPT>
#ifdef HOMOG2D_ENABLE_PRTP
, public rtp::Root
#endif
{
public:
	using FType = FPT;
	using SType = typ::T_FRect;

	using detail::Common<FPT>::isInside;

	Type type() const
	{
		return Type::FRect;
	}

	template<typename T> friend class FRect_;

private:
	Point2d_<FPT> _ptR1,_ptR2;

public:
/** \name Constructors */
///@{
/// Default constructor, initialize rectangle to (0,0)-(1,1)
	FRect_()
	{
		_ptR2.set( 1., 1. );
	}
/// Constructor from 2 points
	template<typename FPT2>
	FRect_( const Point2d_<FPT2>& pa, const Point2d_<FPT2>& pb )
	{
		set( pa, pb );
	}

/// Constructor from pair of points
	template<typename FPT2>
	FRect_( const PointPair_<FPT2>& ppts )
	{
		set( ppts.first, ppts.second );
	}

/// Constructor from center point, width and height
	template<typename FPT2,typename T1, typename T2>
	FRect_( const Point2d_<FPT2>& p0, T1 w, T2 h )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		set(
			Point2d_<FPT>( p0.getX()-0.5L*w, p0.getY()-0.5L*h ),
			Point2d_<FPT>( p0.getX()+0.5L*w, p0.getY()+0.5L*h )
		);
	}

/// Constructor from x1, y1, x2, y2 (need to be all the same type)
	template<typename T>
	FRect_( T x1, T y1, T x2, T y2 )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		set( Point2d_<FPT>(x1,y1), Point2d_<FPT>(x2,y2) );
	}

/// Copy-Constructor
	template<typename FPT2>
	FRect_( const FRect_<FPT2>& other )
		: _ptR1(other._ptR1),_ptR2(other._ptR2)
	{}
///@}

private:
/// Private constructor from 4 points, used in intersectArea( const FRect_& )
	template<typename T>
	FRect_(
		const Point2d_<T>& pt1,
		const Point2d_<T>& pt2,
		const Point2d_<T>& pt3,
		const Point2d_<T>& pt4
	)
	{
//		HOMOG2D_LOG( "pt1" << pt1 << " pt2="<< pt2 << " pt3=" << pt3 << " pt4=" << pt4 );
		auto x0 = std::min( pt1.getX(), pt2.getX() );
		x0 = std::min( x0, pt3.getX() );
		x0 = std::min( x0, pt4.getX() );
		auto y0 = std::min( pt1.getY(), pt2.getY() );
		y0 = std::min( y0, pt3.getY() );
		y0 = std::min( y0, pt4.getY() );

		auto x1 = std::max( pt1.getX(), pt2.getX() );
		x1 = std::max( x1, pt3.getX() );
		x1 = std::max( x1, pt4.getX() );
		auto y1 = std::max( pt1.getY(), pt2.getY() );
		y1 = std::max( y1, pt3.getY() );
		y1 = std::max( y1, pt4.getY() );

		_ptR1 = Point2d_<FPT>(x0,y0);
		_ptR2 = Point2d_<FPT>(x1,y1);
//		HOMOG2D_LOG( "ptR1=" <<_ptR1 << " _ptR2=" << _ptR2 );
	}

public:
/// Assigns points \c pa and \c pb to rectangle
	template<typename FPT1,typename FPT2>
	void set( const Point2d_<FPT1>& pa, const Point2d_<FPT2>& pb )
	{
		auto ppts = detail::getCorrectPoints( pa, pb );
		_ptR1 = ppts.first;
		_ptR2 = ppts.second;
	}
/// Assigns points (x1,y1) and (x2,y2) to rectangle
	template<typename T>
	void set( T x1, T y1, T x2, T y2 )
	{
		set( Point2d_<T>(x1,y1), Point2d_<T>(x2,y2) );
	}

/// \name Attributes access
///@{
	HOMOG2D_INUMTYPE height() const { return  _ptR2.getY() - _ptR1.getY(); }
	HOMOG2D_INUMTYPE width()  const { return  _ptR2.getX() - _ptR1.getX(); }
	HOMOG2D_INUMTYPE area()   const { return height() * width(); }
	HOMOG2D_INUMTYPE length() const { return 2.*height() + 2.*width(); }

	constexpr size_t size() const
	{
		return 4;
	}

/// get BB of rectangle. Needed for getBB( pair of objects )
/// \todo 20250205: fix this so that it return a "full res" rectangle
	FRect_<FPT> getBB() const
	{ //FRect_<HOMOG2D_INUMTYPE> out;
//		out = *this;
		return *this;
//		return out;
	}

/// Returns the 2 major points of the rectangle
/// \sa getPts( const FRect_<FPT>& )
	PointPair_<FPT>
	getPts() const
	{
		return std::make_pair( _ptR1, _ptR2 );
	}

/// Returns center of rectangle
	Point2d_<FPT> getCenter() const
	{
		return Point2d_<FPT>(
			(static_cast<HOMOG2D_INUMTYPE>(_ptR1.getX() ) + _ptR2.getX() ) * 0.5,
			(static_cast<HOMOG2D_INUMTYPE>(_ptR1.getY() ) + _ptR2.getY() ) * 0.5
		);
	}

/// Return circle passing through 4 points of flat rectangle
/// \sa h2d::getBoundingCircle()
	Circle_<FPT> getBoundingCircle() const
	{
		auto middle_pt = getCenter();
		return Circle_<FPT>( middle_pt, middle_pt.distTo( _ptR1 ) );
	}

/// Return circle inscribed in rectangle
/// \sa h2d::getInscribedCircle()
	Circle_<FPT> getInscribedCircle() const
	{
		auto segs = getSegs();
		auto center = getCenter();
		Circle_<FPT> cir( center );
		cir.radius() = std::min(
			center.distTo( segs[0] ),
			center.distTo( segs[1] )
		);
		return cir;
	}

///@}

/// \name Modifying functions
///@{

/// Translate FRect
	template<typename TX, typename TY>
	void translate( TX dx, TY dy )
	{
		HOMOG2D_CHECK_IS_NUMBER( TX );
		HOMOG2D_CHECK_IS_NUMBER( TY );
		_ptR1.set( _ptR1.getX() + dx, _ptR1.getY() + dy );
		_ptR2.set( _ptR2.getX() + dx, _ptR2.getY() + dy );
	}

/// Translate FRect
	template<typename T1, typename T2>
	void translate( const std::pair<T1,T2>& pa )
	{
		this->translate( pa.first, pa.second );
	}

/// Move FRect to other location
	template<typename TX, typename TY>
	void moveTo( TX x, TY y )
	{
		HOMOG2D_CHECK_IS_NUMBER( TX );
		HOMOG2D_CHECK_IS_NUMBER( TY );

		auto s = size();
		_ptR1.set(x,y);
		_ptR2.set( _ptR1.getX() + s.first, _ptR1.getY() + s.second );
	}

/// Move FRect to other location, given by \c pt
	template<typename T1>
	void moveTo( const Point2d_<T1>& pt )
	{
		auto s = size();
		_ptR1 = pt;
		_ptR2.set( _ptR1.getX() + s.first, _ptR1.getY() + s.second );
	}

	template<typename FPT2>
	void rotate( Rotate, const Point2d_<FPT2>& );
	void rotate( Rotate );
///@}

	FRect_<FPT>
	getExtended() const
	{
		auto x1 = _ptR1.getX() * 2. - _ptR2.getX();
		auto y1 = _ptR1.getY() * 2. - _ptR2.getY();

		auto x2 = _ptR2.getX() * 2. - _ptR1.getX();
		auto y2 = _ptR2.getY() * 2. - _ptR1.getY();

		return FRect_<FPT>( x1, y1, x2, y2 );
	}

	std::pair<Segment_<FPT>,Segment_<FPT>>
	getDiagonals() const
	{
		auto x1 = _ptR1.getX();
		auto y1 = _ptR1.getY();
		auto x2 = _ptR2.getX();
		auto y2 = _ptR2.getY();

		return std::make_pair(
			Segment_<FPT>(x1,y1,x2,y2),
			Segment_<FPT>(x1,y2,x2,y1)
		);
	}


/// \name Union/intersection area
///@{
	template<typename FPT2>
	CPolyline_<FPT> unionArea( const FRect_<FPT2>& other ) const;
	template<typename FPT2>
	detail::RectArea<FPT> intersectArea( const FRect_<FPT2>& other ) const;
	template<typename FPT2>
	CPolyline_<FPT> operator | ( const FRect_<FPT2>& other ) const
	{
		return this->unionArea( other );
	}
	template<typename FPT2>
	detail::RectArea<FPT> operator & ( const FRect_<FPT2>& other ) const
	{
		return this->intersectArea( other );
	}
///@}

/// Returns the 4 points of the rectangle, starting from "smallest" one, and
/// in clockwise order
/**
\verbatim
 p1 +------+ p2
    |      |
    |      |
    |      |
 p0 +------+ p3

\endverbatim
\sa get4Pts( const FRect_<FPT>& )
*/
	std::array<Point2d_<FPT>,4>
	get4Pts() const
	{
		std::array<Point2d_<FPT>,4> arr;
		arr[0] = _ptR1;
		arr[1] = Point2d_<FPT>( _ptR1.getX(), _ptR2.getY() );
		arr[2] = _ptR2;
		arr[3] = Point2d_<FPT>( _ptR2.getX(), _ptR1.getY() );
		return arr;
	}

/// Returns the 4 segments of the rectangle, starting with the first vertical one
/**
\verbatim
      s1
   +------+p2
   |      |
s0 |      | s2
   |      |
 p1+------+
      s3
\endverbatim
\sa \ref h2d::getSegs( const FRect_& )
*/
	std::array<Segment_<FPT>,4>
	getSegs() const
	{
		auto pts = get4Pts();
		std::array<Segment_<FPT>,4> out;
		out[0] = Segment_<FPT>( pts[0], pts[1] );
		out[1] = Segment_<FPT>( pts[1], pts[2] );
		out[2] = Segment_<FPT>( pts[2], pts[3] );
		out[3] = Segment_<FPT>( pts[3], pts[0] );
		return out;
	}

private:
/// Returns as a pair the two main segments of the rectangle, with the largest one in "first"
/**
- If one of the 3 considered points is at infinity, order is not checked
*/
	std::pair<Segment_<FPT>,Segment_<FPT>>
	p_getPairSegs() const
	{
		auto pts = get4Pts();
		std::pair<Segment_<FPT>,Segment_<FPT>> out;
		out.first  = Segment_<FPT>( pts[0], pts[1] );
		out.second = Segment_<FPT>( pts[1], pts[2] );

		if( !pts[0].isInf() && !pts[1].isInf() && !pts[2].isInf() )
			if( out.first.length() < out.second.length() )
				std::swap( out.first, out.second );
		return out;
	}

public:

/// \name Enclosing functions
///@{

/// Returns true if rectangle is inside \c shape (Circle_ or FRect_ or base::Polyline)
/// \todo add some SFINAE to enable only for allowed types?
	template<typename T>
	bool isInside( const T& shape ) const
	{
		HOMOG2D_START;
		for( const auto& pt: get4Pts() )
			if( !pt.isInside( shape ) )
				return false;
		return true;
	}

/// A FRect is never inside an open polyline
	template<typename FPT2>
	constexpr bool isInside( const OPolyline_<FPT2>& ) const
	{
		return false;
	}

/// For a rectangle to be inside a closed Polyline, two conditions are necessary:
/**
- all the points must be inside
- no intersections
*/
	template<typename FPT2>
	bool isInside( const CPolyline_<FPT2>& poly ) const
	{
		for( const auto& seg: getSegs() )
			if( seg.intersects(poly)() )
				return false;

		for( const auto& pt: get4Pts() )
			if( !pt.isInside( poly ) )
				return false;
		return true;
	}
///@}

/// \name Intersection functions
///@{

/// FRect/Line intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const Line2d_<FPT2>& line ) const
	{
		return line.intersects( *this );
	}

/// FRect/Segment intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const Segment_<FPT2>& seg ) const
	{
		detail::IntersectM<FPT> out;
		for( const auto& rseg: getSegs() )
		{
			auto inters = rseg.intersects( seg ); // call of Segment/Segment
			if( inters() )
			{
				auto pt =  inters.get();
				bool addPoint = true;
				if( out.size() == 1 ) // if we have already one
					if( out.get()[0] == pt )
						addPoint = false;
				if( addPoint )
					out.add( pt );
				if( out.size() == 2 )
					break;
			}
		}
		return out;
	}

/// FRect/Circle intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const Circle_<FPT2>& circle ) const
	{
//		HOMOG2D_START;
		return p_intersects_R_C( circle );
	}

/// FRect/Polyline intersection
	template<typename PLT2,typename FPT2>
	detail::IntersectM<FPT> intersects( const base::PolylineBase<PLT2,FPT2>& pl ) const
	{
//		HOMOG2D_START;
		return pl.intersects( *this );
	}

/// FRect/FRect intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const FRect_<FPT2>& rect ) const
	{
//		HOMOG2D_START;
		if( *this == rect )                    // if rectangles are the same,
			return detail::IntersectM<FPT>();  // no intersection
		return p_intersects_R_C( rect );
	}
///@}

/// \name Operators
///@{
	template<typename FPT2>
	bool operator == ( const FRect_<FPT2>& other ) const
	{
		if( _ptR1 != other._ptR1 )
			return false;
		if( _ptR2 != other._ptR2 )
			return false;
		return true;
	}
	template<typename FPT2>
	bool operator != ( const FRect_<FPT2>& other ) const
	{
		return !( *this == other );
	}
///@}

	template<typename T>
	friend std::ostream&
	operator << ( std::ostream& f, const FRect_<T>& r );

private:
	template<typename FPT2>
	std::vector<Point2d_<FPT>> p_pointsInside( const FRect_<FPT2>& other ) const
	{
		std::vector<Point2d_<FPT>> out;
		for( const auto& pt: get4Pts() )
			if( pt.isInside( other ) )
				out.push_back( pt );
		return out;
	}

/// Intersection of FRect vs FRect or Circle
/**
- We use a \c std::set to avoid having multiple times the same point.
- second arg is used to fetch the indexes of intersecting segments, when needed
*/
	template<typename T>
	detail::IntersectM<FPT> p_intersects_R_C( const T& other ) const
	{
		std::set<Point2d_<FPT>> pts;
		for( const auto& rseg: getSegs() )
		{
			auto inters = rseg.intersects( other ); // call of Segment/FRect => FRect/Segment, or Segment/Circle
			if( inters() )
			{
				auto vpts = inters.get();
				assert( vpts.size() < 3 );
				if( vpts.size() > 0 )
					pts.insert( vpts[0] );
				if( vpts.size() > 1 )
					pts.insert( vpts[1] );
			}
		}
		detail::IntersectM<FPT> out; // convert std::set to output container
		for( const auto& elem: pts )
			out.add( elem );
		return out;
	}

public:
#ifdef HOMOG2D_USE_OPENCV
	void draw( img::Image<cv::Mat>&,       img::DrawParams dp=img::DrawParams() ) const;
#endif
	void draw( img::Image<img::SvgImage>&, img::DrawParams dp=img::DrawParams() ) const;

}; // class FRect_

//------------------------------------------------------------------
/// A circle
template<typename FPT>
class Circle_: public detail::Common<FPT>
#ifdef HOMOG2D_ENABLE_PRTP
, public rtp::Root
#endif
{
public:
	using FType = FPT;
	using SType = typ::T_Circle;
	using detail::Common<FPT>::isInside;

	Type type() const
	{
		return Type::Circle;
	}

	template<typename T> friend class Circle_;

private:
	FPT           _radius;
	Point2d_<FPT> _center;

public:
/// \name Constructors
///@{

/// Default constructor, unit-radius circle at (0,0)
	Circle_() : _radius(1.)
	{}

/// 1-arg constructor 1, given radius circle at (0,0)
	template<
		typename T,
		typename std::enable_if<
			std::is_arithmetic<T>::value
			,T
		>::type* = nullptr
	>
	explicit Circle_( T rad )
		: Circle_( Point2d_<FPT>(), rad )
	{}

/// 1-arg constructor 2, given center point, radius = 1.0
	template<typename FPT2>
	explicit Circle_( Point2d_<FPT2> center )
		: Circle_( center, 1. )
	{}

/// 1-arg constructor 3, build circle from a set of 2, 3, or more points
/// (Minimum Enclosing Circle, aka MEC)
	template<
		typename T,
		typename std::enable_if<
			trait::IsContainer<T>::value
			,T
		>::type* = nullptr
	>
	explicit Circle_( const T& pts )
	{
		set( pts );
	}

/// 2-arg constructor 1: point and radius
	template<typename T1, typename T2>
	Circle_( const Point2d_<T1>& center, T2 rad=1.0 )
		: _radius(rad), _center(center)
	{
#ifndef HOMOG2D_NOCHECKS
		if( homog2d_abs(rad) < thr::nullDistance() && !h2d::thr::doNotCheckRadius() )
			HOMOG2D_THROW_ERROR_1( "radius value too small: " << std::scientific << homog2d_abs(rad) );
		if( rad < 0. )
			HOMOG2D_THROW_ERROR_1( "radius must not be <0" );
#endif
	}

/// 2-arg constructor 2: circle from 2 points (may be of different types)
	template<typename T1, typename T2>
	Circle_( const Point2d_<T1>& pt1, const Point2d_<T2>& pt2 )
	{
		set( pt1, pt2 );
	}

/// 3-arg constructor 1: build circle from 3 floating-point values: x, y, radius
/**
We need Sfinae because there is another 3-args constructor (circle from 3 points)
*/
	template<
		typename T1,
		typename T2,
		typename std::enable_if<
			(std::is_arithmetic<T1>::value && !std::is_same<T1,bool>::value)
			,T1
		>::type* = nullptr
	>
	Circle_( T1 x, T1 y, T2 rad )
		: Circle_( Point2d_<FPT>(x,y), rad )
	{
//		HOMOG2D_CHECK_IS_NUMBER(T1); // not needed, done by sfinae above
		HOMOG2D_CHECK_IS_NUMBER(T2);
	}

/// 3-arg constructor 2: builds a circle from 3 points
	template<typename T1, typename T2, typename T3>
	Circle_( const Point2d_<T1>& pt1, const Point2d_<T2>& pt2, const Point2d_<T3>& pt3 )
	{
		set( pt1, pt2, pt3 );
	}

/// Copy-Constructor
	template<typename FPT2>
	Circle_( const Circle_<FPT2>& other )
		: _radius(other._radius), _center(other._center)
	{}
///@}

/// \name Attributes access
///@{
	FPT&       radius()       { return _radius; }
	const FPT& radius() const { return _radius; }

	Point2d_<FPT>&       center()       { return _center; }
	const Point2d_<FPT>& center() const { return _center; }
	const Point2d_<FPT>& getCenter() const { return _center; }

	constexpr size_t size() const
	{
		return 1;
	}

/// Area of circle
	HOMOG2D_INUMTYPE area() const
	{
		return static_cast<HOMOG2D_INUMTYPE>(_radius) * _radius * M_PI;
	}

/// Perimeter of circle
	HOMOG2D_INUMTYPE length() const
	{
		return static_cast<HOMOG2D_INUMTYPE>(_radius) * M_PI * 2.0;
	}

/// Returns Bounding Box of circle
	auto getBB() const
	{
		return FRect_<HOMOG2D_INUMTYPE>(
			static_cast<HOMOG2D_INUMTYPE>( _center.getX() ) - _radius,
			static_cast<HOMOG2D_INUMTYPE>( _center.getY() ) - _radius,
			static_cast<HOMOG2D_INUMTYPE>( _center.getX() ) + _radius,
			static_cast<HOMOG2D_INUMTYPE>( _center.getY() ) + _radius
		);
	}
///@}

/// \name Edit values
///@{

/// Set circle center point, radius unchanged
	template<typename PT>
	void set( const Point2d_<PT>& center )
	{
		_center = center;
	}

/// Set circle radius, center point unchanged
/**
Use of Sfinae so it can be selected only for arithmetic types
*/
	template<
		typename T,
		typename std::enable_if<
			(std::is_arithmetic<T>::value && !std::is_same<T,bool>::value)
			,T
		>::type* = nullptr
	>
	void set( T rad )
	{
		_radius = rad;
	}

/// Set circle from center point and radius
	template<typename FPT2,typename FPT3>
	void set( const Point2d_<FPT2>& center, FPT3 rad )
	{
		Circle_<FPT> c( center, rad );
		std::swap( c, *this ); /// \todo 20211216: replace with move
	}

/// Set circle from 3 values (x0,y0,radius)
	template<
		typename FPT2,
		typename std::enable_if<
			std::is_arithmetic<FPT2>::value
			,FPT2
		>::type* = nullptr
	>
	void set( FPT2 x, FPT2 y, FPT2 rad )
	{
		set( Point2d_<FPT2>(x,y), rad );
	}

	template<typename T1, typename T2>
	void set( const Point2d_<T1>&, const Point2d_<T2>& );
	template<typename T1, typename T2, typename T3>
	void set( const Point2d_<T1>&, const Point2d_<T2>&, const Point2d_<T3>& );

// set Minimum Enclosing Circle (MEC) from a set of points
	template<
		typename T,
		typename std::enable_if<
			trait::IsContainer<T>::value
			,T
		>::type* = nullptr
	>
	void set( const T& );

private:
	template<typename T>             // helper function
	Circle_<HOMOG2D_INUMTYPE>
	p_WelzlHelper( std::vector<T>&, std::vector<T>, size_t ) const;

	template<typename T>             // helper function
	Circle_<HOMOG2D_INUMTYPE>
	p_min_circle_trivial( const std::vector<T>& P ) const;

public:
/// Translate Circle
	template<typename TX, typename TY>
	void translate( TX dx, TY dy )
	{
		HOMOG2D_CHECK_IS_NUMBER( TX );
		HOMOG2D_CHECK_IS_NUMBER( TY );
		_center.translate( dx, dy );
	}

/// Translate Circle
	template<typename T1, typename T2>
	void translate( const std::pair<T1,T2>& pa )
	{
		this->translate( pa.first, pa.second );
	}

/// Move Circle to other location
	template<typename TX, typename TY>
	void moveTo( TX x, TY y )
	{
		HOMOG2D_CHECK_IS_NUMBER( TX );
		HOMOG2D_CHECK_IS_NUMBER( TY );
		set( Point2d_<FPT>(x, y) );
	}

/// Move Circle to other location, geiven by \c pt
	template<typename T1>
	void moveTo( const Point2d_<T1>& pt )
	{
		set( pt );
	}

///@}

/// \name Enclosing functions
///@{

/// Returns true if circle is inside \c other circle
	template<typename FPT2>
	bool isInside( const Circle_<FPT2>& other ) const
	{
		return( _radius + _center.distTo( other.center() ) < other.radius() );
	}

/// Returns true if circle is inside rectangle defined by \c p1 and \c p2
	template<typename FPT2>
	bool isInside( const Point2d_<FPT2>& p1, const Point2d_<FPT2>& p2 ) const
	{
		return implC_isInside( detail::getCorrectPoints( p1, p2 ) );
	}

/// Returns true if circle is inside flat rectangle \c rect
	template<typename FPT2>
	bool isInside( const FRect_<FPT2>& rect ) const
	{
		return implC_isInside( rect.getPts() );
	}

// Returns true if circle is inside close polyline \c poly
	template<typename FPT2,typename PTYPE>
	bool isInside( const base::PolylineBase<PTYPE,FPT2>& poly ) const;

///@}

/// \name Intersection functions
///@{

/// Circle/Line intersection
	template<typename FPT2>
	detail::Intersect<detail::Inters_2,FPT>
	intersects( const Line2d_<FPT2>& li ) const
	{
		return li.intersects( *this );
	}

/// Circle/Segment intersection
	template<typename FPT2>
	detail::IntersectM<FPT>
	intersects( const Segment_<FPT2>& seg ) const
	{
		return seg.intersects( *this );
	}

// Circle/Circle intersection
	template<typename FPT2>
	detail::IntersectM<FPT>
	intersects( const Circle_<FPT2>& ) const;

/// Circle/FRect intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const FRect_<FPT2>& rect ) const
	{
		return rect.intersects( * this );
	}

/// Circle/Polyline intersection
	template<typename PLT,typename FPT2>
	detail::IntersectM<FPT> intersects( const base::PolylineBase<PLT,FPT2>& pl ) const
	{
		return pl.intersects( * this );
	}
///@}

private:
	template<typename FPT2>
	bool implC_isInside( const PointPair_<FPT2>& ppts ) const
	{
		const auto& p1 = ppts.first;
		const auto& p2 = ppts.second;
		HOMOG2D_INUMTYPE rad = _radius;   // convert to highest precision
		if( _center.getX() + rad < p2.getX() )
			if( _center.getX() - rad > p1.getX() )
				if( _center.getY() + rad < p2.getY() )
					if( _center.getY() - rad > p1.getY() )
						return true;
		return false;
	}

public:
/// \name Operators
///@{
	template<typename FPT2>
	bool operator == ( const Circle_<FPT2>& other ) const
	{
		if( _radius != other._radius )
			return false;
		if( _center != other._center )
			return false;
		return true;
	}
	template<typename FPT2>
	bool operator != ( const Circle_<FPT2>& other ) const
	{
		return !( *this == other );
	}
///@}

	template<typename T>
	friend std::ostream&
	operator << ( std::ostream& f, const Circle_<T>& r );

#ifdef HOMOG2D_USE_OPENCV
	void draw( img::Image<cv::Mat>&,       img::DrawParams=img::DrawParams() ) const;
#endif
	void draw( img::Image<img::SvgImage>&, img::DrawParams=img::DrawParams() ) const;

}; // class Circle_


//------------------------------------------------------------------
/// Holds private stuff
namespace priv {

/// Private free function, swap the points so that \c ptA.x <= \c ptB.x, and if equal, sorts on y
template<typename FPT>
void
fix_order( Point2d_<FPT>& ptA, Point2d_<FPT>& ptB )
{
	if( !(ptA < ptB) )
		std::swap( ptA, ptB );
}

/// Get sign of value
/** source: https://stackoverflow.com/a/4609795/ */
template <typename T>
int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

//------------------------------------------------------------------
/// Free function, squared distance between points (sqrt not needed for comparisons, and can save some time)
/// \sa Point2d_::distTo()
/// \sa dist( const Point2d_&, const Point2d_& )
template<typename FPT1,typename FPT2>
HOMOG2D_INUMTYPE
sqDist( const Point2d_<FPT1>& pt1, const Point2d_<FPT2>& pt2 )
{
	auto dx = (HOMOG2D_INUMTYPE)pt1.getX() - pt2.getX();
	auto dy = (HOMOG2D_INUMTYPE)pt1.getY() - pt2.getY();
	return dx*dx + dy*dy;
}

//------------------------------------------------------------------
/// Helper function, used to check for colinearity of three points
/**
This will return the same points as given in input but ordered as:
- the pair that has the largest distance in [0] and [1]
- the third point in [2]
- the point closest to the third point in [1], the farthest in [0]

\sa bool areCollinear()

\todo 20220520: needs some optimization, once it has been extensively tested

We have theses 6 situations described on below diagrams A through F, with the desired output order:
\verbatim
 1 +                       1 +
   |    A => 3,1,2           |     B => 2,1,3
   |                         |
 2 +---------------+ 3     3 +---------------+ 2

 2 +                       2 +
   |    C => 3,2,1           |     D => 1,2,3
   |                         |
 1 +---------------+ 3     3 +---------------+ 1

 3 +                       3 +
   |    E => 2,3,1           |     F => 1,3,2
   |                         |
 1 +---------------+ 2     2 +---------------+ 1
\endverbatim
*/
template<typename PT>
std::array<PT,3>
getLargestDistancePoints( PT pt1, PT pt2, PT pt3 )
{
	auto d12 = sqDist( pt1, pt2 );
	auto d13 = sqDist( pt1, pt3 );
	auto d23 = sqDist( pt2, pt3 );

	PT* pA = 0;
	PT* pB = 0;
	PT* pM = 0;

	if( d12 > d13 ) // case B, D, E
	{
		pA = &pt2;
		if( d12 > d23 ) // case B, D
		{
			pB = &pt1;
			pM = &pt3;
			if( d13 > d23 )
				std::swap( *pA, *pB );
		}
		else          // case E
		{
			pB = &pt3;
			pM = &pt1;
		}
	}
	else             // case A, C, F
	{
		pA = &pt3;
		if( d13 > d23 ) // A, F
		{
			pB = &pt1;
			pM = &pt2;
			if( d12 > d23 )
				std::swap( *pA, *pB );
		}
		else           // case C
		{
			pB = &pt2;
			pM = &pt1;
		}
	}

	return std::array<PT,3>{ *pA, *pB, *pM };
}

} // namespace priv


/// Free function template, product of two points, returns a line
template<typename FPT,typename FPT2>
Line2d_<FPT>
operator * ( const Point2d_<FPT>& lhs, const Point2d_<FPT2>& rhs )
{
#ifndef HOMOG2D_NOCHECKS
	if( lhs == rhs )
		HOMOG2D_THROW_ERROR_1( "points are identical, unable to compute product:" << lhs );
#endif
	Line2d_<FPT> line = detail::crossProduct<typ::IsLine,typ::IsPoint,FPT>(lhs, rhs);
	line.p_normalizePL();
	return line;
}

//------------------------------------------------------------------
/// Returns true if the 3 points are on the same line
/**
\todo at present, defined by the distance between third point and line.
Need to change that, and replace by computation of the angle between the two lines
*/
template<typename FPT>
bool
areCollinear( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2, const Point2d_<FPT>& pt3 )
{
	if( pt1 == pt2 || pt2 == pt3 || pt1 == pt3 )
		return true;

	auto pt_arr = priv::getLargestDistancePoints( pt1, pt2, pt3 );

	auto li = pt_arr[0] * pt_arr[1];
	if( li.distTo( pt_arr[2] ) < thr::nullDistance() )
		return true;
	return false;
}

/*
/// Constructor: build Ellipse from Circle
/// \todo finish this
template<typename FPT>
Ellipse_<FPT>::Ellipse_( const Circle_<FPT>& cir )
{
	p_init( cir.center().getX(), cir.center().getY(), radius(), radius(), 0. );
}
*/

//------------------------------------------------------------------
/// Holds private stuff
namespace priv {

/// Helper function, factorized here for the two impl_getPoints_A() implementations
template<typename FPT, typename FPT2,  typename FPT3>
auto
getPoints_B2( const Point2d_<FPT>& pt, FPT2 dist, const Line2d_<FPT3>& li )
{
	auto arr = li.get();
	const HOMOG2D_INUMTYPE a = static_cast<HOMOG2D_INUMTYPE>(arr[0]);
	const HOMOG2D_INUMTYPE b = static_cast<HOMOG2D_INUMTYPE>(arr[1]);
	auto coeff = static_cast<HOMOG2D_INUMTYPE>(dist) / homog2d_sqrt( a*a + b*b );

	Point2d_<FPT> pt1(
        pt.getX() -  b * coeff,
        pt.getY() +  a * coeff
	);
	Point2d_<FPT> pt2(
        pt.getX() +  b * coeff,
        pt.getY() -  a * coeff
	);
	fix_order( pt1, pt2 );
	return std::make_pair( pt1, pt2 );
}

/// Helper function for impl_getOrthogonalLine_A() and impl_getOrthogonalLine_B(),
/// Compute orthogonal line to \c li at point \c pt (that must lie on the line)
template<typename T1,typename T2>
Line2d_<T1>
getOrthogonalLine_B2( const Point2d_<T2>& pt, const Line2d_<T1>& li )
{
	auto arr = li.get(); // get array of 3 values
	Line2d_<T1> out(
		-arr[1],
		arr[0],
		arr[1] * pt.getX() - arr[0] * pt.getY()
	);
	out.p_normalizePL();
	return out;
}

#ifdef HOMOG2D_DEBUGMODE
template<typename T>
void printVector( const std::vector<T>& v, std::string msg=std::string(), bool linefeed=false )
{
	std::cout << "vector: ";
	if( !msg.empty() )
		std::cout << msg;
	std::cout << " #=" << v.size() << '\n';
	size_t c=0;
	for( const auto& elem: v )
	{
		if( linefeed )
			std::cout << c++ << ": ";
		std::cout << elem << (linefeed?'\n':'-');
	}
	std::cout << '\n';
}
template<typename T,size_t N>
void printArray( const std::array<T,N>& v, std::string msg=std::string() )
{
	std::cout << "array: " << msg << " #=" << N<< '\n';
	for( const auto& elem: v )
		std::cout << elem << "-";
	std::cout << '\n';
}
template<typename T>
void printVectorPairs( const std::vector<std::pair<T,T>>& v )
{
	std::cout << "vector of pairs: #=" << v.size() << '\n';
	for( const auto& elem: v )
		std::cout << " [" << (int)elem.first << "-" << (int)elem.second << "] ";
	std::cout << '\n';
}
#endif
} // namespace priv

// forward declaration
template<typename FPT1,typename FPT2>
Line2d_<FPT1>
operator * ( const Point2d_<FPT1>&, const Point2d_<FPT2>& );

// forward declaration
template<typename FPT1,typename FPT2>
Point2d_<FPT1>
operator * ( const Line2d_<FPT1>&, const Line2d_<FPT2>& );

namespace base {

//------------------------------------------------------------------
/// Base class, will be instanciated as \ref Point2d_ or \ref Line2d_
/**
Type parameters:
- LP: typ::IsPoint or typ::IsLine
- FPT: Floating Point Type (float, double or long double)
*/
template<typename LP,typename FPT>
class LPBase: public detail::Common<FPT>
#ifdef HOMOG2D_ENABLE_PRTP
, public rtp::Root
#endif
{
public:
	using FType = FPT;
// The following line enables defining the SType based on how the class is instanciated: Line or Point
	using SType = LP;
//	using SType = std::conditional<std::is_same_v<LP,typ::IsPoint>,typ::T_Point,typ::T_Line>;
	using detail::Common<FPT>::isInside;

private:
	template<typename U,typename V> friend class Hmatrix_;

// This is needed so we can convert from, say, Point2d_<float> to Point2d_<double>
	template<typename U,typename V> friend class LPBase;

	template<typename FPT1,typename FPT2>
	friend Point2d_<FPT1>
	h2d::operator * ( const h2d::Line2d_<FPT1>&, const h2d::Line2d_<FPT2>& );

	template<typename FPT1,typename FPT2>
	friend auto
	h2d::operator * ( const h2d::Point2d_<FPT1>&, const h2d::Point2d_<FPT2>& )
	-> h2d::Line2d_<FPT1>;

	template<typename T,typename U>
	friend auto
	h2d::operator * ( const h2d::Homogr_<U>&, const h2d::Line2d_<T>& )
	-> h2d::Line2d_<T>;

	template<typename T1,typename T2,typename FPT1,typename FPT2>
	friend base::LPBase<T1,FPT1>
	detail::crossProduct( const base::LPBase<T2,FPT1>&, const base::LPBase<T2,FPT2>& );

	template<typename U,typename V>
	friend auto
	operator << ( std::ostream&, const h2d::base::LPBase<U,V>& )
	-> std::ostream&;

	template<typename T1,typename T2,typename FPT1,typename FPT2>
	friend void
	detail::product( base::LPBase<T1,FPT1>&, const detail::Matrix_<FPT2>&, const base::LPBase<T2,FPT1>& );

	template<typename T1,typename T2>
	friend Line2d_<T1>
	priv::getOrthogonalLine_B2( const Point2d_<T2>&, const Line2d_<T1>& );

public:

/// Constructor: build a point from two lines
	template<typename FPT2>
	LPBase( const Line2d_<FPT2>& v1, const Line2d_<FPT2>& v2 )
	{
#ifndef HOMOG2D_NOCHECKS
		if( v1.isParallelTo(v2) )
			HOMOG2D_THROW_ERROR_1( "unable to build point from these two lines, are parallel" );
#endif
		*this = detail::crossProduct<typ::IsPoint>( v1, v2 );
		p_normalizePL();
	}

/// Constructor: build a line from two points
	template<typename FPT2>
	LPBase( const Point2d_<FPT2>& v1, const Point2d_<FPT2>& v2 )
	{
#ifndef HOMOG2D_NOCHECKS
		if( v1 == v2 )
			HOMOG2D_THROW_ERROR_1( "unable to build line from these two points, are the same: " << v1 );
#endif
		*this = detail::crossProduct<typ::IsLine>( v1, v2 );
		p_normalizePL();
	}

/// Constructor: copy-constructor for lines
/**
\todo We should be able to declare this "explicit". This fails at present when attempting
to convert a line (or point) from double to float, but I don't get why...
*/
	template<typename T>
//		explicit
	LPBase( const Line2d_<T>& li )
	{
		static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot build a point from a line" );
		p_copyFrom( li );
	}

/// Constructor with single arg of type "Point"
/**
This will call one of the two overloads of \c impl_init_1_Point(), depending on type of object:
- if type is a point, then it can be seen as a copy-constructor
- if type is a line, this will build a line from (0,0] to \c pt
*/
	template<typename T>
	LPBase( const Point2d_<T>& pt )
	{
		impl_init_1_Point<T>( pt );
	}

/// Constructor: build from two numerical values, depends on the type
	template<typename T1,typename T2>
	LPBase( const T1& v1, const T2& v2 )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		p_init_2( v1, v2 );
	}

/// Constructor of line/point from 3 values
	template<typename T0,typename T1,typename T2>
	LPBase( T0 v0, T1 v1, T2 v2 )
	{
		set( v0, v1, v2 );
	}

/// Assign homogeneous values
	template<typename T0,typename T1,typename T2>
	void set( T0 v0, T1 v1, T2 v2 )
	{
		HOMOG2D_CHECK_IS_NUMBER(T0);
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		_v[0] = v0;
		_v[1] = v1;
		_v[2] = v2;
		p_normalizePL();
	}

/// Constructor of line from 4 values x1,y1,x2,y2
	template<typename T1,typename T2,typename T3,typename T4>
	LPBase( T1 x1, T2 y1, T3 x2, T4 y2 )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		HOMOG2D_CHECK_IS_NUMBER(T3);
		HOMOG2D_CHECK_IS_NUMBER(T4);
		static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot build a point from 4 values" );
		*this = Point2d_<HOMOG2D_INUMTYPE>(x1, y1) * Point2d_<HOMOG2D_INUMTYPE>(x2, y2);
	}

#if 0
/// Constructor of Point/Line from random type holding x,y values, see manual, section
	template<
		typename T,
		typename std::enable_if<
			! trait::IsContainer<T>::value,
			T
		>::type* = nullptr
	>
	LPBase( T val )
	{
		p_init_2( val.HOMOG2D_BIND_X, val.HOMOG2D_BIND_Y );
	}
#endif

/// Constructor from an array holding 3 values of same type (a direct copy can be done)
	template<
		typename T,
		typename std::enable_if<
			std::is_same<T,std::array<FPT,3>>::value,
			T
		>::type* = nullptr
	>
	LPBase( const T& arr )
	{
		_v = arr;
		p_normalizePL();
	}

/// Constructor from an array/vector holding 3 values of different type
	template<
		typename T,
		typename std::enable_if<
			(
				!std::is_same<T,std::array<FPT,3>>::value &&
				(
					std::is_same<T,std::array<typename T::value_type,3>>::value
					|| std::is_same<T,std::vector<typename T::value_type>>::value
				)
			),
			T
		>::type* = nullptr
	>
	LPBase( const T& arr )
	{
		_v[0] = static_cast<FPT>(arr[0]);
		_v[1] = static_cast<FPT>(arr[1]);
		_v[2] = static_cast<FPT>(arr[2]);
		p_normalizePL();
	}

/// Default constructor, depends on the type
	LPBase()
	{
		if constexpr( std::is_same_v<LP,typ::IsLine> )
		{
			_v[0] = 1.;
			_v[1] = 0.;
			_v[2] = 0.;
		}
		else
		{
			_v[0] = 0.;
			_v[1] = 0.;
			_v[2] = 1.;
		}
	}

/// Constructor of horizontal/vertical line
	template<typename T>
	LPBase( LineDir orient, T value )
	{
		impl_init_or( orient, value );
	}


#ifdef HOMOG2D_USE_BOOSTGEOM

public:
/// Constructor from boost::geometry point type
/**
\note Although this one should work also for the other point type (`bg::model::d2::point_xy`),
as that latter one is inherited from the first one),
it does not, because there is another truely generic single-arg constructor, and the compiler
will select that one first, leading to a build error.
Thus, we need the second one.
*/
	template<typename BFPT> // Boost Floating Point Type
	LPBase( const boost::geometry::model::point<BFPT, 2, boost::geometry::cs::cartesian>& pt )
	{
		init_BoostGeomPoint( pt );
	}

/// Constructor from boost::geometry second point type
	template<typename BFPT> // Boost Floating Point Type
	LPBase( const boost::geometry::model::d2::point_xy<BFPT>& pt )
	{
		init_BoostGeomPoint( pt );
	}

/// Set from boost::geometry point type
	template<typename BFPT> // Boost Floating Point Type
	void set( const boost::geometry::model::point<BFPT, 2, boost::geometry::cs::cartesian>& pt )
	{
		set( boost::geometry::get<0>(pt), boost::geometry::get<1>(pt), 1.0 );
	}

#endif

private:
	template<typename T,typename U>
	void p_copyFrom( const LPBase<T,U>& other )
	{
		_v[0] = static_cast<FPT>(other._v[0]);
		_v[1] = static_cast<FPT>(other._v[1]);
		_v[2] = static_cast<FPT>(other._v[2]);
	}

/// init function. if object is a point => copy-constructor,
///	else  we build the line passing though (0,0) ant the given point
	template<typename T>
	void impl_init_1_Point( const Point2d_<T>& pt )
	{
		if constexpr( std::is_same_v<LP,typ::IsPoint> )
			p_copyFrom( pt );
		else
		{
			*this = detail::crossProduct<typ::IsLine>( pt, Point2d_<FPT>() );
			p_normalizePL();
		}
	}

	template<typename T>
	void impl_init_or( LineDir, T );
	template<typename T>
	void impl_init_or( LineDir, const Point2d_<T>& );

public:
	Type type() const
	{
		if constexpr( std::is_same_v<LP,typ::IsPoint> )
			return Type::Point2d;
		else
			return Type::Line2d;
	}

	size_t size() const
	{
		if constexpr( std::is_same_v<LP,typ::IsPoint> )
			return 1;
		else
			return 0;
	}

	template<typename FPT2>
	HOMOG2D_INUMTYPE
	getCoord( GivenCoord gc, FPT2 other ) const;

	template<typename FPT2>
	Point2d_<FPT>
	getPoint( GivenCoord gc, FPT2 other ) const
	{
		static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot call on a point" );
		HOMOG2D_CHECK_IS_NUMBER( FPT2 );

		auto coord = getCoord( gc, other );
		if( gc == GivenCoord::X )
			return Point2d_<FPT>( other, coord );
		return Point2d_<FPT>( coord, other );
	}

	/// Returns a pair of points that are lying on line at distance \c dist from a point defined by one of its coordinates.
	template<typename FPT2,typename FPT3>
	PointPair_<FPT>
	getPoints( GivenCoord gc, FPT2 coord, FPT3 dist ) const
	{
		static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot call on a point" );
		HOMOG2D_CHECK_IS_NUMBER(FPT2);
		HOMOG2D_CHECK_IS_NUMBER(FPT3);

		const auto pt = getPoint( gc, coord );
		return priv::getPoints_B2( pt, dist, *this );
	}

	/// Returns a pair of points that are lying on line at distance \c dist from point \c pt, assuming that one is lying on the line.
	template<typename FPT2,typename FPT3>
	PointPair_<FPT>
	getPoints( const Point2d_<FPT2>& pt, FPT3 dist ) const;

	template<typename FPT2>
	Line2d_<FPT>
	getOrthogLine( GivenCoord gc, FPT2 other ) const;

	template<typename FPT2>
	Line2d_<FPT>
	getOrthogLine( const Point2d_<FPT2>& pt ) const;

	/// Returns a line rotated at point \c pt with angle \c theta
	template<typename FPT2, typename T>
	Line2d_<FPT>
	getRotatedLine( const Point2d_<FPT2>& pt, T theta ) const;

	/// Returns the segment from the point (not on line) to the line, shortest path
	template<typename FPT2>
	OSegment_<FPT>
	getOrthogSegment( const Point2d_<FPT2>& pt ) const;

	/// Returns an parallel line to the one it is called on, with \c pt lying on it.
	/// \todo clarify orientation: on which side will that line appear?
	template<typename FPT2>
	Line2d_<FPT>
	getParallelLine( const Point2d_<FPT2>& ) const;

	/// Returns the pair of parallel lines at a distance \c dist from line.
	template<typename T>
	std::pair<Line2d_<FPT>,Line2d_<FPT>>
	getParallelLines( T dist ) const;

	HOMOG2D_INUMTYPE getX() const
	{
		static_assert( std::is_same_v<LP,typ::IsPoint>, "Invalid: cannot get x for a line" );
		return static_cast<HOMOG2D_INUMTYPE>(_v[0]) / _v[2];
	}
	HOMOG2D_INUMTYPE getY() const
	{
		static_assert( std::is_same_v<LP,typ::IsPoint>, "Invalid: cannot get y for a line" );
		return static_cast<HOMOG2D_INUMTYPE>(_v[1]) / _v[2];
	}

/// Translate Point2d, does nothing for Line2d
	template<typename T1,typename T2>
	void translate( T1 dx, T2 dy )
	{
		HOMOG2D_CHECK_IS_NUMBER( T1 );
		HOMOG2D_CHECK_IS_NUMBER( T2 );

		if constexpr( std::is_same_v<LP,typ::IsPoint> )
		{
			_v[0] = static_cast<HOMOG2D_INUMTYPE>(_v[0]) / _v[2] + dx;
			_v[1] = static_cast<HOMOG2D_INUMTYPE>(_v[1]) / _v[2] + dy;
			_v[2] = 1.;
			p_normalizePL();
		}
	}

/// Translate Point2d, does nothing for Line2d
	template<typename T1, typename T2>
	void translate( const std::pair<T1,T2>& pa )
	{
		this->translate( pa.first, pa.second );
	}

/// Move point to other location (same as set(), but this one will be virtual).
/// Does nothing for lines => NO, FAILS TO BUILD !
	template<typename T1>
	void moveTo( const Point2d_<T1>& pt )
	{
		static_assert( std::is_same_v<LP,typ::IsPoint>, "Invalid: cannot move a line" );
		*this = pt;
	}

#ifdef HOMOG2D_ENABLE_VRTP
/// Needed so the function getBB(T1,T2) builds, whatever the types
/// and because of variant (\sa CommonType)
	FRect_<HOMOG2D_INUMTYPE> getBB() const
	{
		HOMOG2D_THROW_ERROR_1( "invalid call, Point/Line has no Bounding Box" );
	}
#endif

private:
	template<typename ANY>
	ANY impl_getPt( const detail::BaseHelper<typename typ::IsPoint>& ) const
	{
		return ANY( getX(), getY() );
	}
	template<typename ANY>
	ANY impl_getPt( const detail::BaseHelper<typename typ::IsLine>& ) const
	{
		static_assert( detail::AlwaysFalse<LP>::value, "Invalid call for lines" );
	}

public:
/// Generic transformation into any other point type, as long as it provides a 2-args constructor
/// (is the case for Opencv and Boost Geometry).
/// \sa h2d::getPt()
	template<typename ANY>
	ANY getPt() const
	{
		return impl_getPt<ANY>( detail::BaseHelper<LP>() );
	}

	std::array<FPT,3> get() const
	{
		return std::array<FPT,3> { _v[0], _v[1], _v[2] };
	}

/// Set point from 2 euclidean values
	template<typename T1,typename T2>
	void set( T1 x, T2 y )
	{
		HOMOG2D_CHECK_IS_NUMBER( T1 );
		HOMOG2D_CHECK_IS_NUMBER( T2 );
		static_assert( std::is_same_v<LP,typ::IsPoint>, "Invalid call for lines" );

		_v[0] = x;
		_v[1] = y;
		_v[2] = 1.;
		p_normalizePL();
	}

	template<typename FPT2>
	HOMOG2D_INUMTYPE distTo( const Point2d_<FPT2>& pt ) const
	{
		return impl_distToPoint( pt, detail::BaseHelper<LP>() );
	}

	template<typename FPT2>
	HOMOG2D_INUMTYPE distTo( const Line2d_<FPT2>& ) const;
	template<typename FPT2>
	HOMOG2D_INUMTYPE distTo( const Segment_<FPT2>& ) const;

	template<typename FPT2>
	bool isParallelTo( const Line2d_<FPT2>& ) const;

	template<typename T>
	bool isParallelTo( const Segment_<T>& seg ) const
	{
		return isParallelTo( seg.getLine() );
	}

	template<typename T,typename FPT2>
	HOMOG2D_INUMTYPE getAngle( const LPBase<T,FPT2>& other ) const;

/// Returns angle in rad. between line and segment \c seg. \sa  h2d::getAngle()
	template<typename T,typename FPT2>
	HOMOG2D_INUMTYPE getAngle( const base::SegVec<T,FPT2>& seg ) const
	{
		return getAngle( seg.getLine() );
	}

/// Returns true if point is at infinity (third value less than thr::nullDenom() )
	bool isInf() const
	{
		if constexpr( std::is_same_v<LP,typ::IsLine> )
			return false;
		else
			return homog2d_abs( _v[2] ) < thr::nullDenom();
	}

/// A point has a null length, a line has an infinite length
	HOMOG2D_INUMTYPE length() const
	{
		if constexpr( std::is_same_v<LP,typ::IsLine> )
		{
			HOMOG2D_THROW_ERROR_1( "unable, a line has an infinite length" );
		}
		else
			return 0.;
	}

/// Neither lines nor points have an area
	constexpr HOMOG2D_INUMTYPE area() const
	{ return 0.; }

private:
	template<typename FPT2>
	HOMOG2D_INUMTYPE impl_distToPoint( const Point2d_<FPT2>&, const detail::BaseHelper<typename typ::IsPoint>& ) const;
	template<typename FPT2>
	HOMOG2D_INUMTYPE impl_distToPoint( const Point2d_<FPT2>&, const detail::BaseHelper<typename typ::IsLine>&  ) const;

public:
/// Line/Line intersection
	template<typename FPT2>
	detail::Intersect<detail::Inters_1,FPT> intersects( const Line2d_<FPT2>& other ) const
	{
		detail::Intersect<detail::Inters_1,FPT> out;
		if( this->isParallelTo( other ) )
			return out;
		 out.set( *this * other );
		 return out;
	}
/// Line/FRect intersection (rectangle defined by \c pt1 and \c pt2)
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const Point2d_<FPT2>& pt1, const Point2d_<FPT2>& pt2 ) const
	{
		return intersects( FRect_<FPT2>( pt1, pt2 ) ) ;
	}
/// Line/FRect intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const FRect_<FPT2>& rect ) const
	{
		return impl_intersectsFRect( rect, detail::BaseHelper<LP>() );
	}

/// Line/Segment intersection
/** \warning no implementation for points */
	template<typename FPT2>
	detail::Intersect<detail::Inters_1,FPT> intersects( const Segment_<FPT2>& seg ) const
	{
		return seg.intersects( *this );
	}

/// Line/Circle intersection
/** <br>The Sfinae below is needed to avoid ambiguity with the other 2 args "intersects()" function
(with 2 points defining a FRect, see above) */
	template<
		typename T,
		typename std::enable_if<
			(std::is_arithmetic<T>::value && !std::is_same<T,bool>::value)
			,T
		>::type* = nullptr
	>
	detail::Intersect<detail::Inters_2,FPT>
	intersects( const Point2d_<FPT>& pt0, T radius ) const
	{
		return impl_intersectsCircle( pt0, radius, detail::BaseHelper<LP>() );
	}

/// Line/Circle intersection
	template<typename T>
	detail::Intersect<detail::Inters_2,FPT> intersects( const Circle_<T>& cir ) const
	{
		return impl_intersectsCircle( cir.center(), cir.radius(), detail::BaseHelper<LP>() );
	}

/// Line/Polyline intersection
	template<typename PLT,typename FPT2>
	detail::IntersectM<FPT> intersects( const base::PolylineBase<PLT,FPT2>& pl ) const
	{
		return pl.intersects( *this );
	}

/// Point is inside flat rectangle
	template<typename T1,typename T2>
	bool isInside( const Point2d_<T1>& pt1, const Point2d_<T2>& pt2 ) const
	{
		HOMOG2D_START;
		return isInside( FRect_<FPT>(pt1, pt2) );
	}

/// Point is inside FRect
	template<typename FPT2>
	bool isInside( const FRect_<FPT2>& rect ) const;

/// Point is inside circle defined by center and radius
	template<
		typename T,
		typename std::enable_if<
			!std::is_same_v<T,Point2d_<FPT>>
			,T
		>::type* = nullptr
	>
	bool
	isInside( const Point2d_<FPT>& center, T radius ) const
	{
		HOMOG2D_CHECK_IS_NUMBER(T);

		if constexpr( std::is_same_v<LP,typ::IsLine> )
			return false;
		else
		{
			if( distTo( center ) < radius )
				return true;
			return false;
		}
	}




/// Point is inside Circle
	template<typename T>
	bool isInside( const Circle_<T>& cir ) const
	{
		return isInside( cir.center(), cir.radius() );
	}

/// Point is inside Ellipse
	template<typename FPT2>
	bool isInside( const Ellipse_<FPT2>& ell ) const;

	template<typename FPT2,typename PTYPE>
	bool isInside( const base::PolylineBase<PTYPE,FPT2>& poly ) const
	{
		HOMOG2D_START;
		return impl_isInsidePoly( poly, detail::BaseHelper<LP>() );
	}

//////////////////////////
//       OPERATORS      //
//////////////////////////
	bool operator == ( const base::LPBase<LP,FPT>& other ) const
	{
		return impl_op_equal( other, detail::BaseHelper<LP>() );
	}
	bool operator != ( const base::LPBase<LP,FPT>& other ) const
	{
		return !(*this == other);
	}
	bool operator < ( const base::LPBase<LP,FPT>& other ) const;

/// SVG draw function
	void draw( img::Image<img::SvgImage>& im, img::DrawParams dp=img::DrawParams() ) const
	{
		impl_draw_LP( im, dp, detail::BaseHelper<LP>() );
	}

#ifdef HOMOG2D_USE_OPENCV
private:
	template<typename OPENCVT>
	OPENCVT impl_getCvPt( const detail::BaseHelper<typ::IsPoint>&, const OPENCVT& ) const
	{
		return OPENCVT( getX(),getY() );
	}

public:
/// Opencv draw function
	void draw( img::Image<cv::Mat>& im, img::DrawParams dp=img::DrawParams() ) const
	{
		impl_draw_LP( im, dp, detail::BaseHelper<LP>() );
	}

	cv::Point2i getCvPti() const { return impl_getPt<cv::Point2i>( detail::BaseHelper<typename typ::IsPoint>() ); }
	cv::Point2i getCvPtd() const { return impl_getPt<cv::Point2d>( detail::BaseHelper<typename typ::IsPoint>() ); }
	cv::Point2i getCvPtf() const { return impl_getPt<cv::Point2f>( detail::BaseHelper<typename typ::IsPoint>() ); }

/// Constructor: build point or line from a single OpenCv point.
	template<typename T>
	LPBase( cv::Point_<T> pt )
	{
		if constexpr( std::is_same_v<LP,typ::IsPoint> )
		{
			p_init_2( pt.x, pt.y );
		}
		else
		{
			Point2d_<FPT> p(pt);
			impl_init_1_Point<FPT>( p );
		}
	}
#endif // HOMOG2D_USE_OPENCV

//////////////////////////
//      DATA SECTION    //
//////////////////////////

private:
	std::array<FPT,3> _v; ///< data, uses the template parameter FPT (for "Floating Point Type")

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////
private:
	void p_normalizePL() const
	{
		impl_normalize( detail::BaseHelper<LP>() );
	}
	void impl_normalize( const detail::BaseHelper<typ::IsLine>& ) const;
	void impl_normalize( const detail::BaseHelper<typ::IsPoint>& ) const;

	template<typename FPT2>
	detail::IntersectM<FPT>
	impl_intersectsFRect( const FRect_<FPT2>&, const detail::BaseHelper<typ::IsLine>& ) const;

	template<typename FPT2>
	constexpr detail::IntersectM<FPT>
	impl_intersectsFRect( const FRect_<FPT2>&, const detail::BaseHelper<typ::IsPoint>& ) const;

	template<typename T>
	detail::Intersect<detail::Inters_2,FPT>
	impl_intersectsCircle( const Point2d_<FPT>&, T r, const detail::BaseHelper<typ::IsLine>& ) const;

	template<typename T>
	constexpr detail::Intersect<detail::Inters_2,FPT>
	impl_intersectsCircle( const Point2d_<FPT>&, T r, const detail::BaseHelper<typ::IsPoint>& ) const;

	template<typename T,typename PTYPE>
	bool           impl_isInsidePoly( const base::PolylineBase<PTYPE,T>&, const detail::BaseHelper<typ::IsPoint>& ) const;
	template<typename T,typename PTYPE>
	constexpr bool impl_isInsidePoly( const base::PolylineBase<PTYPE,T>&, const detail::BaseHelper<typ::IsLine>&  ) const;

	bool impl_op_equal( const LPBase<LP,FPT>&, const detail::BaseHelper<typ::IsLine>&  ) const;
	bool impl_op_equal( const LPBase<LP,FPT>&, const detail::BaseHelper<typ::IsPoint>& ) const;

//	Point2d_<FPT> impl_op_product( const Line2d_<FPT>& , const Line2d_<FPT>& , const detail::BaseHelper<typ::IsPoint>& ) const;
//	Line2d_<FPT>  impl_op_product( const Point2d_<FPT>&, const Point2d_<FPT>&, const detail::BaseHelper<typ::IsLine>&  ) const;

	template<typename T>
	void impl_draw_LP( img::Image<T>&, img::DrawParams, const detail::BaseHelper<typ::IsPoint>& )  const;
	template<typename T>
	void impl_draw_LP( img::Image<T>&, img::DrawParams, const detail::BaseHelper<typ::IsLine>& )  const;

	template<typename T1,typename T2>
	void p_init_2( const T1&, const T2& );

#ifdef HOMOG2D_USE_BOOSTGEOM
	template<typename BFPT> // Boost Floating Point Type
	void init_BoostGeomPoint(
		const boost::geometry::model::point<BFPT, 2, boost::geometry::cs::cartesian>& pt
	)
	{
		static_assert( std::is_same_v<LP,typ::IsPoint>, "Invalid: you cannot build a Line2d using a boost::geometry point" );
		set( pt );
	}
#endif

}; // class LPBase

//------------------------------------------------------------------
/// Helper function for constructor of horizontal or vertical line
/// (overload for argument of type Point2d)
/// \sa LineDir
template<typename LP,typename FPT>
template<typename T>
void
LPBase<LP,FPT>::impl_init_or( LineDir dir, const Point2d_<T>& pt )
{
	static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot build a horiz/vertical point" );

	if( dir == LineDir::V )
	{
		_v[2] = -pt.getX();
		_v[0] = 1.; _v[1] = 0.;
	}
	else  // = LineDir::H
	{
		_v[2] = -pt.getY();
		_v[0] = 0.; _v[1] = 1.;
	}
}

//------------------------------------------------------------------
/// Helper function for constructor of horizontal or vertical line
/// (overload for numerical argument)
/// \sa LineDir
template<typename LP,typename FPT>
template<typename T>
void
LPBase<LP,FPT>::impl_init_or( LineDir dir, T arg )
{
	static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot build a horiz/vertical point" );

	HOMOG2D_CHECK_IS_NUMBER(T);
	_v[2] = -arg;
	if( dir == LineDir::V )
	{
		_v[0] = 1.; _v[1] = 0.;
	}
	else  // = LineDir::H
	{
		_v[0] = 0.; _v[1] = 1.;
	}
}


} // namespace base


/////////////////////////////////////////////////////////////////////////////
// SECTION  - OPENCV API CODE
/////////////////////////////////////////////////////////////////////////////

#ifdef HOMOG2D_USE_OPENCV
template<typename FPT>
FRect_<FPT> getFRect( cv::Mat& mat )
{
	if(  mat.cols == 0 || mat.rows == 0 )
		HOMOG2D_THROW_ERROR_1(
			"Illegal values: cols=" << mat.cols << ", rows=" << mat.rows
		);

	return FRect_<FPT>(
		Point2d_<FPT>(),                      // (0,0)
		Point2d_<FPT>( mat.cols, mat.rows )   // (w,h)
	);
}

/// Free function to return an OpenCv point (double)
template<typename FPT>
cv::Point2d
getCvPtd( const Point2d_<FPT>& pt )
{
	return pt.getCvPtd();
}
/// Free function to return an OpenCv point (float)
template<typename FPT>
cv::Point2f
getCvPtf( const Point2d_<FPT>& pt )
{
	return pt.getCvPtf();
}
/// Free function to return an OpenCv point (integer)
template<typename FPT>
cv::Point2i
getCvPti( const Point2d_<FPT>& pt )
{
	return pt.getCvPti();
}

#endif // HOMOG2D_USE_OPENCV


/////////////////////////////////////////////////////////////////////////////
// SECTION  - FREE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
/// Returns true is lines (or segments) are parallel
template<typename T1,typename T2>
bool
areParallel( const T1& t1, const T2& t2 )
{
	return t1.isParallelTo(t2);
}

//------------------------------------------------------------------
/// Generic free function to return a point of other type
/**
- RT: return type
- FPT: Floating Point Type

User code needs to provide the requested type as template argument, for example:
\code
auto p1 = getPt<cv::Point2di>( pt );                             // opencv type
auto p2 = getPt<boost::geometry::model::point_xy<double>>( pt ); // boost geometry type
\endcode
\sa LPBase::getPt()
*/
template<typename RT,typename FPT>
RT
getPt( const Point2d_<FPT>& pt )
{
	return pt.template getPt<RT>();
}

//------------------------------------------------------------------
/// Free function, returns a vector of points of other type from a vector of h2d points
/**
- RT: return type
- FPT: Floating Point Type

User code needs to provide the requested type as template argument.
\sa h2d::getPt<>()

\todo 20230219: sfinae this to accept other containers using trait::IsContainer
*/

template<typename RT,typename FPT>
std::vector<RT>
getPts( const std::vector<Point2d_<FPT>>& vpt )
{
	std::vector<RT> vout( vpt.size() );
	auto it = vout.begin();
	for( const auto& pt: vpt )
		*it++ = getPt<RT>(pt);
	return vout;
}

//------------------------------------------------------------------
/// This namespace holds some private stuff, types here are not to be used directly by end-user code.
namespace detail {

//------------------------------------------------------------------
/// Private free function, returns true if point \c pt is inside the rectangle defined by (\c p00 , \c p11)
template<typename FPT1,typename FPT2>
bool
ptIsInside( const Point2d_<FPT1>& pt, const Point2d_<FPT2>& p00, const Point2d_<FPT2>& p11 )
{
	if( pt.getX() > p00.getX() && pt.getX() < p11.getX() )
		if( pt.getY() > p00.getY() && pt.getY() < p11.getY() )
			return true;
	return false;
}

#ifdef HOMOG2D_USE_EIGEN
///  Build Homography from 2 sets of 4 points, using Eigen
/**
See
- https://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html
- https://eigen.tuxfamily.org/dox/group__DenseMatrixManipulation__chapter.html
*/
template<typename FPT>
Homogr_<FPT>
buildFrom4Points_Eigen(
	const std::vector<Point2d_<FPT>>& vpt1, ///< source points
	const std::vector<Point2d_<FPT>>& vpt2  ///< destination points
)
{
	Eigen::MatrixXd A = Eigen::MatrixXd::Zero(8,8);
	Eigen::VectorXd b(8);

	for( int i=0; i<4; i++ )
	{
		auto u1 = vpt1[i].getX();
		auto v1 = vpt1[i].getY();
		auto u2 = vpt2[i].getX();
		auto v2 = vpt2[i].getY();

		b(2*i)   = u2;
		b(2*i+1) = v2;

		A(i*2,0) = A(i*2+1,3) = u1;
		A(i*2,1) = A(i*2+1,4) = v1;
		A(i*2,2) = A(i*2+1,5) = 1.;

		A(i*2,6)   = - u1 * u2;
		A(i*2,7)   = - v1 * u2;
		A(i*2+1,6) = - u1 * v2;
		A(i*2+1,7) = - v1 * v2;
	}

#if 0
	Eigen::VectorXd X = A.ldlt().solve(b); // for some reason this does not work...
#else
	Eigen::MatrixXd Ai = A.inverse();
	Eigen::VectorXd X = Ai * b;
#endif

	Homogr_<FPT> H;
	for( int i=0; i<8; i++ )
		H.set( i/3, i%3, X(i) );
	H.set(2, 2, 1.);

	return H;
}
#endif // HOMOG2D_USE_EIGEN

//------------------------------------------------------------------
#ifdef HOMOG2D_USE_OPENCV
///  Build Homography from 2 sets of 4 points, using Opencv
/**
- see https://docs.opencv.org/master/d9/d0c/group__calib3d.html#ga4abc2ece9fab9398f2e560d53c8c9780

\note With current Opencv installed on current machine, it seems that \c cv::getPerspectiveTransform()
requires that the points are "CV_32F" (\c float), and NOT double.
*/
template<typename FPT>
Homogr_<FPT>
buildFrom4Points_Opencv (
	const std::vector<Point2d_<FPT>>& vpt1, ///< source points
	const std::vector<Point2d_<FPT>>& vpt2  ///< destination points
)
{
	const auto& src = getPts<cv::Point2f>( vpt1 );
	const auto& dst = getPts<cv::Point2f>( vpt2 );
	return cv::getPerspectiveTransform( src, dst ); // automatic type conversion to Hmatrix_
}
#endif

} // namespace detail

//------------------------------------------------------------------
/// Build Homography from 2 sets of 4 points (free function)
/// \sa Homogr_::buildFrom4Points()
template<typename FPT>
Homogr_<FPT>
buildFrom4Points(
	const std::vector<Point2d_<FPT>>& vpt1,     ///< source points
	const std::vector<Point2d_<FPT>>& vpt2,     ///< destination points
	int                               method=1  ///< 0: Eigen, 1: Opencv
)
{
	Homogr_<FPT> H;
	H.buildFrom4Points( vpt1, vpt2, method );
	return H;
}

//------------------------------------------------------------------
/// Build Homography from 2 sets of 4 points
/**
- Requires either Eigen or Opencv
- we build a 8x8 matrix A and a 8x1 vector B, and get the solution from X = A^-1 B
- see this for details:
https://skramm.lautre.net/files/misc/Kramm_compute_H_from_4pts.pdf

\sa free function: h2d::buildFrom4Points()

\todo fix this so that user can provide a std::array of points
*/
template<typename M,typename FPT>
void
Hmatrix_<M,FPT>::buildFrom4Points(
	const std::vector<Point2d_<FPT>>& vpt1,   ///< source points
	const std::vector<Point2d_<FPT>>& vpt2,   ///< destination points
	int                               method  ///< 0: Eigen, 1: Opencv (default)
)
{
	if( vpt1.size() != 4 )
		HOMOG2D_THROW_ERROR_1( "invalid vector size for source points, should be 4, value=" << vpt1.size() );
	if( vpt2.size() != 4 )
		HOMOG2D_THROW_ERROR_1( "invalid vector size for dest points, should be 4, value=" << vpt2.size() );
	assert( method == 0 || method == 1 );

	if( method == 0 )
	{
#ifdef HOMOG2D_USE_EIGEN
		*this = detail::buildFrom4Points_Eigen( vpt1, vpt2 );
#else
		throw std::runtime_error( "Unable, build without Eigen support" );
#endif
	}
	else
	{
#ifdef HOMOG2D_USE_OPENCV
		*this = detail::buildFrom4Points_Opencv( vpt1, vpt2 );
#else
		throw std::runtime_error( "Unable, build without Opencv support" );
#endif
	}
}

/// Point related to a OSegment
///\sa SegVec::getPointSide()
enum class PointSide: uint8_t
{
	Left,Right,Neither
};

inline
const char*
getString( PointSide t )
{
	const char* s=0;
	switch( t )
	{
		case PointSide::Left:    s="Left";    break;
		case PointSide::Right:   s="Right";   break;
		case PointSide::Neither: s="Neither"; break;
		assert(0);
	}
	return s;
}



namespace base {
//------------------------------------------------------------------
/// A line segment, oriented (\ref OSegment_) or not (\ref Segment_). Holds the two points.
/**
This will get instanciated as \ref Segment_ or \ref OSegment_

The difference between theses two types is that with \c Segment_,
the "smallest" point is always stored as first element (see constructor).
*/
template<typename SV,typename FPT>
class SegVec: public detail::Common<FPT>
#ifdef HOMOG2D_ENABLE_PRTP
, public rtp::Root
#endif
{
public:
	using FType = FPT;
//	using SType = std::conditional<std::is_same_v<SV,typ::IsSegment>,typ::T_Segment,typ::T_OSeg>;
	using SType = SV; //std::conditional<std::is_same_v<SV,typ::IsSegment>,typ::T_Segment,typ::T_OSeg>;
	using detail::Common<FPT>::isInside;

	template<typename T1,typename T2> friend class SegVec;


/// \todo 20250127: if this works, then generalize to all the other base type() member functions
	Type type() const
	{
		if constexpr( std::is_same_v<SV,typ::IsSegment> )
			return Type::Segment;
		else
			return Type::OSegment;
	}

private:
	Point2d_<FPT> _ptS1, _ptS2;

public:
/// \name Constructors
///@{

/// Default constructor: initializes segment to (0,0)--(1,1)
	SegVec(): _ptS2(1.,1.)
	{}

/// Constructor 2: build segment from two points, can hold different FP types
/**
Please note that the source (points) floating-point type is lost
*/
	template<typename FP1,typename FP2>
	SegVec( Point2d_<FP1> p1, Point2d_<FP2> p2 )
		: _ptS1(p1), _ptS2(p2)
	{
#ifndef HOMOG2D_NOCHECKS
		if( p1 == p2 )
			HOMOG2D_THROW_ERROR_1( "cannot build a segment with two identical points: " << p1 << " and " << p2 );
#endif
		if constexpr( std::is_same_v<SV,typ::IsSegment> )
			priv::fix_order( _ptS1, _ptS2 );
	}

/// Constructor 3: build segment from two points coordinates, calls constructor 2
	template<typename FP1,typename FP2,typename FP3,typename FP4>
	SegVec( FP1 x1, FP2 y1, FP3 x2, FP4 y2 )
		: SegVec( Point2d_<FPT>(x1,y1), Point2d_<FPT>(x2,y2) )
	{
		HOMOG2D_CHECK_IS_NUMBER(FP1);
		HOMOG2D_CHECK_IS_NUMBER(FP2);
		HOMOG2D_CHECK_IS_NUMBER(FP3);
		HOMOG2D_CHECK_IS_NUMBER(FP4);
	}

/// Constructor 4: build segment from pair of points
	template<typename FP1,typename FP2>
	SegVec( const PointPair2_<FP1,FP2>& ppts )
		: SegVec(ppts.first, ppts.second)
	{}

/// Copy-Constructor, behavior depends on concrete types
/**
- OSegment(OSegment): OK
- Segment(OSegment): OK, but loose orientation
- Segment(Segment):  OK
- OSegment(Segment): No build, because orientation would be arbitrary
*/
	template<typename SV2,typename FPT2>
	SegVec( const SegVec<SV2,FPT2>& other )
		: _ptS1(other._ptS1), _ptS2(other._ptS2)
	{
		static_assert(
			( std::is_same_v<SV,typ::IsSegment>
			||
			( std::is_same_v<SV,typ::IsOSeg> && std::is_same_v<SV2,typ::IsOSeg> ) ),
			"Cannot build a OSegment from a Segment"
		);
		if constexpr( std::is_same_v<SV2,typ::IsSegment> )
			priv::fix_order( _ptS1, _ptS2 );
	}

///@}

/// \name Modifying functions
///@{

/// Reverse oriented segment
	SegVec<SV,FPT> operator - ()
	{
		static_assert( !std::is_same_v<SV,typ::IsSegment>, "cannot reverse non-oriented segment" );
		std::swap( _ptS1, _ptS2 );
		return *this;
	}

/// Setter
	template<typename FP1,typename FP2>
	void set( const Point2d_<FP1>& p1, const Point2d_<FP2>& p2 )
	{
#ifndef HOMOG2D_NOCHECKS
		if( p1 == p2 )
			HOMOG2D_THROW_ERROR_1( "cannot define a segment with two identical points" << p1 << " and " << p2 );
#endif
		_ptS1 = p1;
		_ptS2 = p2;
		if constexpr( std::is_same_v<SV,typ::IsSegment> )
			priv::fix_order( _ptS1, _ptS2 );
	}

/// Setter from a std::pair
	template<typename FP1,typename FP2>
	void set( const PointPair2_<FP1,FP2>& ppts )
	{
		set( ppts.first, ppts.second );
	}

/// Setter from 4 raw point coordinates
	template<typename FP1,typename FP2,typename FP3,typename FP4>
	void set( FP1 x1, FP2 y1, FP3 x2, FP4 y2 )
	{
		HOMOG2D_CHECK_IS_NUMBER(FP1);
		HOMOG2D_CHECK_IS_NUMBER(FP2);
		HOMOG2D_CHECK_IS_NUMBER(FP3);
		HOMOG2D_CHECK_IS_NUMBER(FP4);
		set( Point2d_<FPT>(x1,y1), Point2d_<FPT>(x2,y2) );
	}

/// Translate Segment
	template<typename T1,typename T2>
	void translate( T1 dx, T2 dy )
	{
		HOMOG2D_CHECK_IS_NUMBER( T1 );
		HOMOG2D_CHECK_IS_NUMBER( T2 );
		_ptS1.translate( dx, dy );
		_ptS2.translate( dx, dy );
	}

/// Translate Segment, using pair of values
	template<typename T1, typename T2>
	void translate( const std::pair<T1,T2>& pa )
	{
		this->translate( pa.first, pa.second );
	}

/// Move Segment to other location
	template<typename TX, typename TY>
	void moveTo( TX x, TY y )
	{
		HOMOG2D_CHECK_IS_NUMBER( TX );
		HOMOG2D_CHECK_IS_NUMBER( TY );
		moveTo( Point2d_<FPT>(x,y) );
	}

/// Move Segment to other location, given by \c pt
	template<typename T1>
	void moveTo( const Point2d_<T1>& pt )
	{
		auto w = _ptS2.getX() - _ptS1.getX();
		auto h = _ptS2.getY() - _ptS1.getY();
		_ptS1 = pt;
		_ptS2.set( _ptS1.getX() + w, _ptS1.getY() + h );
	}

///@}

/// \name Attributes access
///@{

#ifdef HOMOG2D_ENABLE_VRTP
/// Get Bounding Box of segment, always throws but needed so the function getBB(T1,T2) builds, whatever the types
/// and because of variant (\sa CommonType) (HOMOG2D_ENABLE_VRTP)
	FRect_<HOMOG2D_INUMTYPE> getBB() const
	{
		HOMOG2D_THROW_ERROR_1( "invalid call, Segment has no Bounding Box" );
	}
#endif

/// Returns 2
	constexpr size_t size() const
	{
		return 2;
	}

/// Get segment length
	HOMOG2D_INUMTYPE length() const
	{
		return _ptS1.distTo( _ptS2 );
	}

/// A segment always has a null area
	constexpr HOMOG2D_INUMTYPE area() const
	{
		return 0.;
	}

/// Get vector of Oriented segment as a pair of values
	std::pair<HOMOG2D_INUMTYPE,HOMOG2D_INUMTYPE>
	getVector() const
	{
		static_assert( std::is_same_v<SV,typ::IsOSeg>, "cannot return vector of unoriented segment" );
		return std::make_pair(
			_ptS2.getX() - _ptS1.getX(),
			_ptS2.getY() - _ptS1.getY()
		);
	}

/// Get angle between segment and other segment
/**
- if either the object or the argument is not oriented, then this will return the line angles,
in the range [0:+PI]
- if both are oriented, will return a value in the range [-PI:+PI]
*/
	template<typename SV2,typename FPT2>
	HOMOG2D_INUMTYPE
	getAngle( const SegVec<SV2,FPT2>& other ) const
	{
// if one of the two is a (unoriented) segment, then we just return the lines angle
		if constexpr( std::is_same_v<SV,typ::IsSegment> || std::is_same_v<SV2,typ::IsSegment> )
			return other.getLine().getAngle( this->getLine() );
		else
		{                                          // both are oriented segments
			auto v1 = this->getVector();
			auto v2 = other.getVector();
			auto dx1 = v1.first;
			auto dx2 = v2.first;
			auto dy1 = v1.second;
			auto dy2 = v2.second;
			return std::atan2(
				dx1 * dy2 - dy1 * dx2,
				dx1 * dx2 + dy1 * dy2
			);
		}
	}

/// Get angle between segment and line
	template<typename LP,typename FPT2>
	HOMOG2D_INUMTYPE
	getAngle( const base::LPBase<LP,FPT2>& other ) const
	{
		static_assert( std::is_same_v<LP,typ::IsLine>, "cannot compute angle between segment and point" );
		return getLine().getAngle( other );
	}
///@}

/// \name Operators
///@{
	bool operator == ( const SegVec& s2 ) const
	{
		if( _ptS1 != s2._ptS1 )
			return false;
		if( _ptS2 != s2._ptS2 )
			return false;
		return true;
	}
	bool operator != ( const SegVec& s2 ) const
	{
		return !(*this == s2);
	}

	bool operator < ( const SegVec& other ) const
	{
		return _ptS1 < other._ptS1;
	}
///@}

private:
	template<typename S>
	bool p_bothPtsAreInside( const S& shape ) const
	{
		if( !_ptS1.isInside( shape ) )
			return false;
		if( !_ptS2.isInside( shape ) )
			return false;
		return true;
	}

public:
/// \name Enclosing functions
///@{

/// Segment is inside Circle
	template<typename FPT2>
	bool isInside( const Circle_<FPT2>& shape ) const
	{
		return p_bothPtsAreInside( shape );
	}

/// Segment is inside FRect
	template<typename FPT2>
	bool isInside( const FRect_<FPT2>& shape ) const
	{
		return p_bothPtsAreInside( shape );
	}

/// Segment is inside Ellipse
	template<typename FPT2>
	bool isInside( const Ellipse_<FPT2>& shape ) const
	{
		return p_bothPtsAreInside( shape );
	}

/// Segment is inside OPolyline_
	template<typename FPT2>
	constexpr bool isInside( const OPolyline_<FPT2>& ) const
	{
		return false;
	}

/// Segment is inside CPolyline_
/**
Requires both points inside AND no intersections
*/
	template<typename FPT2>
	bool isInside( const CPolyline_<FPT2>& cpoly ) const
	{
		if( !p_bothPtsAreInside( cpoly ) )
			return false;

		for( auto poly_seg: cpoly.getSegs() )
			if( poly_seg.intersects( *this )() )
				return false;
		return true;
	}
///@}

/// \name Other const functions
///@{

/// Returns the points of segment as a std::pair
/** If "Segment", then the one with smallest x coordinate will be returned as "first".
If x-coordinate are equal, then
the one with smallest y-coordinate will be returned first
*/
	PointPair_<FPT>
	getPts() const
	{
		return std::make_pair( _ptS1, _ptS2 );
	}

	template<typename T>
	std::pair<SegVec,SegVec>
	getParallelSegs( T dist ) const;

	template<typename FPT2>
	Point2d_<HOMOG2D_INUMTYPE>
	getPointAt( FPT2 dist ) const;

private:
/// Computes the 4 points orthogonal to the segment
	std::array<Point2d_<FPT>,4>
	p_getOrthog() const
	{
		auto x1 = static_cast<HOMOG2D_INUMTYPE>(_ptS1.getX());
		auto x2 = static_cast<HOMOG2D_INUMTYPE>(_ptS2.getX());
		auto y1 = static_cast<HOMOG2D_INUMTYPE>(_ptS1.getY());
		auto y2 = static_cast<HOMOG2D_INUMTYPE>(_ptS2.getY());
		auto dx = x1 - x2;
		auto dy = y1 - y2;
		std::array<Point2d_<FPT>,4> out;
		out[0] = Point2d_<FPT>( x1-dy, y1+dx );
		out[1] = Point2d_<FPT>( x1+dy, y1-dx );
		out[3] = Point2d_<FPT>( x2-dy, y2+dx );
		out[2] = Point2d_<FPT>( x2+dy, y2-dx );
		return out;
	}

public:
/// Returns the 4 points orthogonal to the segment/vector
	std::array<Point2d_<FPT>,4>
	getOrthogPts() const
	{
		return p_getOrthog();
	}

/// Returns the 4 segments/vectors orthogonal to the segment
	std::array<Segment_<FPT>,4>
	getOrthogSegs() const
	{
		std::array<Segment_<FPT>,4> out;
		auto pts = p_getOrthog();
		out[0] = Segment_<FPT>( _ptS1, pts[0] );
		out[1] = Segment_<FPT>( _ptS1, pts[1] );
		out[2] = Segment_<FPT>( _ptS2, pts[2] );
		out[3] = Segment_<FPT>( _ptS2, pts[3] );
		return out;
	}

/// Returns supporting line
	auto
	getLine() const
	{
		Point2d_<HOMOG2D_INUMTYPE> pt1( _ptS1 );
		Point2d_<HOMOG2D_INUMTYPE> pt2( _ptS2 );
		auto li = pt1 * pt2;
		return Line2d_<HOMOG2D_INUMTYPE>(li);
	}

	template<typename FPT2>
	PointSide
	getPointSide( const Point2d_<FPT2>& pt ) const;

	std::pair<SegVec<SV,FPT>,SegVec<SV,FPT>>
	split() const;

	template<typename T>
	std::pair<SegVec<typ::IsOSeg,FPT>,SegVec<typ::IsOSeg,FPT>>
	split( T dist ) const;

	template<typename FPT2>
	HOMOG2D_INUMTYPE
	distTo( const Point2d_<FPT2>&, int* segDistCase=0 ) const;

	template<typename T>
	constexpr bool
	isParallelTo( const T& other ) const
	{
		static_assert(
			std::is_same<T,SegVec<SV,FPT>>::value ||
			std::is_same<T,Line2d_<FPT>>::value,
			"type needs to be a segment, a vector, or a line" );
		return getLine().isParallelTo( other );
	}

	/// Returns point at middle distance between \c p1 and \c p2
	auto
	getCenter() const
	{
		return Point2d_<HOMOG2D_INUMTYPE>(
			( static_cast<HOMOG2D_INUMTYPE>(_ptS1.getX()) + _ptS2.getX() ) / 2.,
			( static_cast<HOMOG2D_INUMTYPE>(_ptS1.getY()) + _ptS2.getY() ) / 2.
		);
	}

	SegVec<SV,FPT> getExtended() const;

/// Returns the bisector line of the segment
/// \sa free function h2d::getBisector()
	auto
	getBisector() const
	{
		SegVec<SV,HOMOG2D_INUMTYPE> seg2 = *this; // convert to (possibly) enhance precision
		return seg2.getLine().getOrthogLine( seg2.getCenter() );
	}
///@}

/// \name Intersection functions
///@{
	template<typename SV2,typename FPT2>
	detail::Intersect<detail::Inters_1,FPT> intersects( const SegVec<SV2,FPT2>& ) const;
	template<typename FPT2>
	detail::Intersect<detail::Inters_1,FPT> intersects( const Line2d_<FPT2>&  ) const;
	template<typename FPT2>
	detail::IntersectM<FPT>                 intersects( const Circle_<FPT2>&  ) const;
/// Segment/FRect intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const FRect_<FPT2>& r ) const
	{
		return r.intersects( *this );
	}

/// Segment/Polyline intersection
	template<typename PLT,typename FPT2>
	detail::IntersectM<FPT> intersects( const base::PolylineBase<PLT,FPT2>& other ) const
	{
		return other.intersects( *this );
	}
///@}

	template<typename U,typename V>
	friend std::ostream&
	operator << ( std::ostream&, const SegVec<U,V>& );

#ifdef HOMOG2D_USE_OPENCV
	void draw( img::Image<cv::Mat>&,       img::DrawParams dp=img::DrawParams() ) const;
#endif
	void draw( img::Image<img::SvgImage>&, img::DrawParams dp=img::DrawParams() ) const;

}; // class SegVec

//------------------------------------------------------------------
/// Returns a pair of segments/vectors split by the middle
template<typename SV,typename FPT>
std::pair<SegVec<SV,FPT>,SegVec<SV,FPT>>
SegVec<SV,FPT>::split() const
{
	auto pt_mid = getCenter();
	return std::make_pair(
		SegVec<SV,FPT>( _ptS1,  pt_mid ),
		SegVec<SV,FPT>( pt_mid, _ptS2 )
	);
}

//------------------------------------------------------------------
/// Returns a pair of segments/vectors split by point at arbitrary distance
/// \note: distance value must be >0 and less than segment length
template<typename SV,typename FPT>
template<typename T>
std::pair<OSegment_<FPT>,OSegment_<FPT>>
SegVec<SV,FPT>::split( T dist ) const
{
	static_assert( std::is_same_v<SV,typ::IsOSeg>, "cannot use this on non-oriented segments" );
	HOMOG2D_CHECK_IS_NUMBER(T);
#ifndef HOMOG2D_NOCHECKS
	if( dist <= 0 )
		HOMOG2D_THROW_ERROR_1( "distance value must be >=0, current value=" << dist );
	if( dist >= length() )
		HOMOG2D_THROW_ERROR_1( "distance value must less than length, current value="
			<< dist << " length=" << length() );
#endif

	auto pt = getPointAt(dist);
	return std::make_pair(
		SegVec<SV,FPT>( _ptS1, pt ),
		SegVec<SV,FPT>( pt, _ptS2 )
	);
}


//------------------------------------------------------------------
/// Return side of point \c pt relatively to the segment (Oriented only)
template<typename SV,typename FPT>
template<typename T>
PointSide
SegVec<SV,FPT>::getPointSide( const Point2d_<T>& pt ) const
{
	static_assert( std::is_same_v<SV,typ::IsOSeg>, "unable to get side of point related to Segment" );

	if( pt == _ptS1 || pt == _ptS2 )
		return PointSide::Neither;

	OSegment_<FPT> other( _ptS1, pt );
	HOMOG2D_INUMTYPE cp = detail::crossProductV( other, *this );
	PointSide out;
	switch ( priv::sign(cp) )
	{
		case  0: out = PointSide::Neither; break;
		case -1: out = PointSide::Left;    break;
		case +1: out = PointSide::Right;   break;
		default: assert(0);
	}
	return out;
}

//------------------------------------------------------------------
/// Returns the point at a distance \c dist from starting point of oriented vector
template<typename SV,typename FPT>
template<typename FPT2>
Point2d_<HOMOG2D_INUMTYPE>
SegVec<SV,FPT>::getPointAt( FPT2 dist ) const
{
	static_assert( std::is_same_v<SV,typ::IsOSeg>, "Cannot apply on non-oriented segment" );
	HOMOG2D_CHECK_IS_NUMBER(FPT2);
#ifndef HOMOG2D_NOCHECKS
	if( dist < 0 )
		HOMOG2D_THROW_ERROR_1( "distance value must be >0, current value=" << dist );
#endif

	Line2d_<HOMOG2D_INUMTYPE> li = getLine();
	auto ppts = li.getPoints( _ptS1, dist );
	if( _ptS2.distTo( ppts.first ) < _ptS2.distTo( ppts.second ) )
		return ppts.first;
	else
		return ppts.second;
}

//------------------------------------------------------------------
/// Returns two parallel segments to the current one in a pair
/**
Behavior differs depending on type of segment:
 - if oriented (\ref OSegment_), the returned pair will hold as \c first the segment that is \b left of the original
 - if not oriented (\ref Segment_), the order will be unpredictable
*/
template<typename SV, typename FPT>
template<typename T>
std::pair<SegVec<SV,FPT>,SegVec<SV,FPT>>
SegVec<SV,FPT>::getParallelSegs( T dist ) const
{
	HOMOG2D_CHECK_IS_NUMBER( T );
#ifndef HOMOG2D_NOCHECKS
	if( dist <= 0 )
		HOMOG2D_THROW_ERROR_1( "Invalid value for distance:" << dist );
#endif

	auto plines = getLine().getParallelLines( dist );
	auto lo1 = getLine().getOrthogLine( _ptS1 );
	auto lo2 = getLine().getOrthogLine( _ptS2 );

	auto pA1 = lo1 * plines.first;
	auto pA2 = lo2 * plines.first;
	auto pB1 = lo1 * plines.second;
	auto pB2 = lo2 * plines.second;

	if constexpr( std::is_same_v<SV,typ::IsOSeg> )
		if( getPointSide(pB1) == PointSide::Left )
		{
			std::swap( pA1, pB1 );
			std::swap( pA2, pB2 );
		}

	return std::make_pair(
		SegVec( pA1, pA2 ),
		SegVec( pB1, pB2 )
	);
}


//------------------------------------------------------------------
/// Returns a segment with same support line but tripled length.
/**
With (1,0)-(2,0) as input, will return the segment (0,0)-(3,0)
*/
template<typename SV, typename FPT>
SegVec<SV,FPT>
SegVec<SV,FPT>::getExtended() const
{
	Segment_<HOMOG2D_INUMTYPE> seg(*this); // to get highest precision
	auto li = seg.getLine();
	auto rad1 = seg.length();
	auto c1 = Circle_<HOMOG2D_INUMTYPE>( _ptS1, rad1 );
	auto c2 = Circle_<HOMOG2D_INUMTYPE>( _ptS2, rad1 );

	auto int1 = li.intersects( c1 );
	auto int2 = li.intersects( c2 );
	assert( int1() );
	assert( int2() );
	auto ppt1 = int1.get();
	auto ppt2 = int2.get();

	return Segment_<FPT>(
		ppt1.first,
		ppt2.second
	);
}

//------------------------------------------------------------------
/// Distance from point to segment
/**
source: https://stackoverflow.com/a/6853926/193789

Temp implementation, until we get into this a bit more deeper

\todo 20250219: pass an enum instead as an int, for \c segDistCase
*/
template<typename SV,typename FPT>
template<typename FPT2>
HOMOG2D_INUMTYPE
SegVec<SV,FPT>::distTo(
	const Point2d_<FPT2>& pt, ///< the point
	int* segDistCase          ///< optional, may be used to know if its the orthogonal distance or not
) const
{
	auto ppts = getPts();
	auto x1 = static_cast<HOMOG2D_INUMTYPE>( ppts.first.getX() );
	auto y1 = static_cast<HOMOG2D_INUMTYPE>( ppts.first.getY() );
	auto x2 = static_cast<HOMOG2D_INUMTYPE>( ppts.second.getX() );
	auto y2 = static_cast<HOMOG2D_INUMTYPE>( ppts.second.getY() );

	auto A = static_cast<HOMOG2D_INUMTYPE>(pt.getX()) - x1;
	auto B = static_cast<HOMOG2D_INUMTYPE>(pt.getY()) - y1;
	auto C = x2 - x1;
	auto D = y2 - y1;

	auto dot    = A * C + B * D;
	auto len_sq = C * C + D * D;
	auto param  = dot / len_sq;

	HOMOG2D_INUMTYPE xx, yy;

	if( param < 0. )
	{
		if( segDistCase )
			*segDistCase = -1;
		xx = x1;
		yy = y1;
	}
	else
	{
		if( param > 1. )
		{
			if( segDistCase )
				*segDistCase = +1;
			xx = x2;
			yy = y2;
		}
		else
		{
			if( segDistCase )
				*segDistCase = 0;
			xx = x1 + param * C;
			yy = y1 + param * D;
		}
	}

	auto dx = pt.getX() - xx;
	auto dy = pt.getY() - yy;
	return homog2d_sqrt( dx * dx + dy * dy );
}

} // namespace base

//------------------------------------------------------------------
/// Set circle from 2 points
template<typename FPT>
template<typename T1, typename T2>
void
Circle_<FPT>::set( const Point2d_<T1>& pt1, const Point2d_<T2>& pt2 )
{
#ifndef HOMOG2D_NOCHECKS
	if( pt1 == pt2 )
		HOMOG2D_THROW_ERROR_1( "Unable, some points are identical" );
#endif

	Segment_<HOMOG2D_INUMTYPE> seg( pt1, pt2 );
	_center = seg.getCenter();
	_radius = seg.length() / 2.0;
}

//------------------------------------------------------------------
/// Set circle from 3 points
/**
Algorithm: we get the two largest segments and compute their bisector line.
Their intersection point will be the center of circle, and the radius is the distance between
center and any of the three points.

We consider the the largest segments to improve numerical stability.

Will throw if unable (numerical issue)

One could think that the first checking would be enough, but experience shows that some times,
the point are not colinear, but the two bisector lines are still parallel.
Thus the second checking.

\todo Check this other technique:
https://www.johndcook.com/blog/2023/06/18/circle-through-three-points/
*/
template<typename FPT>
template<typename T1,typename T2,typename T3>
void
Circle_<FPT>::set(
	const Point2d_<T1>& pt1,
	const Point2d_<T2>& pt2,
	const Point2d_<T3>& pt3
)
{
#ifndef HOMOG2D_NOCHECKS
	if( areCollinear( pt1, pt2, pt3 ) )
		HOMOG2D_THROW_ERROR_1( "Unable, points are colinear" );
#endif
	auto pts = priv::getLargestDistancePoints( pt1, pt2, pt3 );

	auto seg1 = Segment_<HOMOG2D_INUMTYPE>( pts[0], pts[1] );
	auto seg2 = Segment_<HOMOG2D_INUMTYPE>( pts[0], pts[2] );
	auto li1 = seg1.getBisector();
	auto li2 = seg2.getBisector();
#ifndef HOMOG2D_NOCHECKS
	if( li1.isParallelTo(li2) )
		HOMOG2D_THROW_ERROR_1( "unable, bisector lines are parallel" )
#endif
	center() = li1 * li2;
	radius() = center().distTo(pt1);
}

//------------------------------------------------------------------
namespace priv {

/// Free Function to check whether a circle encloses the given points
template<typename FPT1,typename PT>
bool is_valid_circle(
	const Circle_<FPT1>&   circ,
	const std::vector<PT>& pts
)
{
	for( const auto& pt: pts )
		if( !pt.isInside( circ ) )
			return false;
	return true;
}

} // namespace priv

//------------------------------------------------------------------
/// Helper function for Circle_<FPT>::set( const T& )
/**
Returns the minimum enclosing circle for N <= 3
*/
template<typename FPT>
template<typename T>
Circle_<HOMOG2D_INUMTYPE>
Circle_<FPT>::p_min_circle_trivial( const std::vector<T>& P ) const
{
	if( P.empty() )
		return Circle_<HOMOG2D_INUMTYPE>(0.,0.,0.); // dummy circle of null radius
	if( P.size() == 1 )
		return Circle_<HOMOG2D_INUMTYPE>( P[0], 0. );
	if( P.size() == 2 )
		return Circle_<HOMOG2D_INUMTYPE>( P[0], P[1] );

// To check if MEC can be determined by 2 points only
	for( int i=0; i<3; i++ )
		for( int j=i+1; j<3; j++ )
		{
			auto c = Circle_<HOMOG2D_INUMTYPE>( P[i], P[j] );
			if( priv::is_valid_circle( c, P ) )
				return c;
		}
	return Circle_<HOMOG2D_INUMTYPE>( P[0], P[1], P[2] ); // circle from 3 points
}

//------------------------------------------------------------------
/// Helper function for Circle_<FPT>::set( const T& ), recursive
/**
Returns the MEC using Welzl's algorithm.
Takes a set of input points P and a set R points on the circle boundary.

\c T is the point type
 */
template<typename FPT>
template<typename T>
Circle_<HOMOG2D_INUMTYPE>
Circle_<FPT>::p_WelzlHelper(
	std::vector<T>& P,  ///< input set of points
	std::vector<T>  R,
	size_t          n   ///< Number of points in P that are not yet processed
) const
{
	// Base case when all points processed or |R| = 3
	if( n == 0 || R.size() == 3 )
		return p_min_circle_trivial( R );

	// Pick a random point randomly
	int idx = std::rand() % n;
	auto p = P[idx];

	// Put the picked point at the end of P
	// since it's more efficient than
	// deleting from the middle of the vector
	std::swap( P[idx], P[n - 1] );

	// Get the MEC circle d from the set of points P - {p}
	auto d = p_WelzlHelper( P, R, n - 1 );
	if( p.isInside( d ) )
			return d;

	// Otherwise, must be on the boundary of the MEC
	R.push_back( p );

	// Return the MEC for P - {p} and R U {p}
	auto c = p_WelzlHelper( P, R, n - 1 );
	return c;
}

//------------------------------------------------------------------
/// Compute circle from a set of points (Minimum Enclosing Circle, aka MEC)
/// using the Welzl algorithm
/**
\c T may be std::vector, std::array or std::list holding points

References:
- https://en.wikipedia.org/wiki/Smallest-circle_problem
- https://www.geeksforgeeks.org/minimum-enclosing-circle-using-welzls-algorithm/
*/
template<typename FPT>
template<
	typename T,
	typename std::enable_if<
		trait::IsContainer<T>::value
		,T
	>::type*
>
void
Circle_<FPT>::set( const T& pts )
{
	if( pts.size() < 2 )
		HOMOG2D_THROW_ERROR_1( "unable to build a circle from a single point" );

	if( pts.size() == 2 )
	{
		set( pts[0], pts[1] );
		return;
	}

	if( pts.size() == 3 ) // todo: convert pts to HOMOG2D_INUMTYPE
	{
		set( pts[0], pts[1], pts[2] );
		return;
	}

	std::vector<Point2d_<HOMOG2D_INUMTYPE>> P_copy( std::begin(pts), std::end(pts) );
//	std::random_shuffle( P_copy.begin(), P_copy.end() ); // ? check: what happens if removed?
	std::vector<Point2d_<HOMOG2D_INUMTYPE>> R;

	h2d::thr::doNotCheckRadius() = true;
	auto cir = p_WelzlHelper( P_copy, R, P_copy.size() );
	set( cir.center(), cir.radius() );
	h2d::thr::doNotCheckRadius() = false;
}

//------------------------------------------------------------------
/// Returns true if circle is inside polyline
/**
Will be true if all these four conditions are met:
- the polyline object must be of type "closed" AND a polygon (no intersection points)
- center point is inside the polygon
- no intersection points
- any point of the polygon must be outside of the circle
*/
template<typename FPT>
template<typename FPT2,typename PTYPE>
bool
Circle_<FPT>::isInside( const base::PolylineBase<PTYPE,FPT2>& poly ) const
{
	HOMOG2D_START;
	if( !poly.isSimple() )
		return false;
	if( poly.getPts()[0].isInside(*this) ) // if a point of the polygon is inside the circle,
		return false;                      //  then the circle cannot be inside the polygon

	if( !_center.isInside( poly ) )
		return false;

	auto inters = intersects( poly );
	return( inters() == 0 );
}

//------------------------------------------------------------------
/// Circle/Circle intersection
/**
Ref:
- https://stackoverflow.com/questions/3349125/

Can return 0, 1, or 2 intersection points

\todo 20230219: in some situation, the difference below (x2 - x1) can be numericaly instable.
Check if things would get improved by multiplying first (by a/d and h/d), before proceeding the difference.
*/
template<typename FPT>
template<typename FPT2>
detail::IntersectM<FPT>
Circle_<FPT>::intersects( const Circle_<FPT2>& other ) const
{
	if( *this == other )
		return detail::IntersectM<FPT>();

	HOMOG2D_INUMTYPE r1 = _radius;
	HOMOG2D_INUMTYPE r2 = other._radius;
	Point2d_<HOMOG2D_INUMTYPE> pt1 = _center;
	Point2d_<HOMOG2D_INUMTYPE> pt2 = other._center;

	HOMOG2D_INUMTYPE x1 = pt1.getX();
	HOMOG2D_INUMTYPE y1 = pt1.getY();
	HOMOG2D_INUMTYPE x2 = pt2.getX();
	HOMOG2D_INUMTYPE y2 = pt2.getY();
	HOMOG2D_INUMTYPE d_squared = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);

	if( d_squared > r1*r1 + r2*r2 + (HOMOG2D_INUMTYPE)2.*r1*r2 )              // no intersection
		return detail::IntersectM<FPT>();

	if( d_squared < ( r1*r1 + r2*r2 - (HOMOG2D_INUMTYPE)2.*r1*r2 ) )          // no intersection: one circle inside the other
		return detail::IntersectM<FPT>();

	auto d = homog2d_sqrt( d_squared );
	auto a = (r1*r1 - r2*r2 + d_squared) / (HOMOG2D_INUMTYPE)2. / d;
	auto h = homog2d_sqrt( r1*r1 - a*a );

	Point2d_<FPT> P0(
		( x2 - x1 ) * a / d + x1,
		( y2 - y1 ) * a / d + y1
	);

	Point2d_<FPT> pt3(
		P0.getX() + ( y1 - y2 ) * h / d,
		P0.getY() - ( x1 - x2 ) * h / d
	);
	Point2d_<FPT> pt4(
		P0.getX() - ( y1 - y2 ) * h / d,
		P0.getY() + ( x1 - x2 ) * h / d
	);

	detail::IntersectM<FPT> out;
	out.add( pt3 );
	if( pt3 != pt4 )
		out.add( pt4 );
	return out;
}


namespace priv {
//------------------------------------------------------------------
/// A POD value that needs some computing, associated with its flag.
/// Used to be able to retain a value needing complex calculation.
template<typename T>
class ValueFlag
{
private:
	T    _value;
	bool _valIsCorrect = false;
public:
	ValueFlag() = default;
	void set( T v )
	{
		_value = v;
		_valIsCorrect = true;
	}
	T value() const    { return _value; }
	void setBad()      { _valIsCorrect = false; }
	bool isBad() const { return !_valIsCorrect; }
};


//------------------------------------------------------------------
/// Holds attribute of a Polyline, allows storage of last computed value through the use of ValueFlag
struct PolylineAttribs
{
	priv::ValueFlag<HOMOG2D_INUMTYPE> _length;
	priv::ValueFlag<HOMOG2D_INUMTYPE> _area;
	priv::ValueFlag<bool>             _isSimplePolyg;
	priv::ValueFlag<Point2d_<HOMOG2D_INUMTYPE>> _centroid;

	void setBad()
	{
		_length.setBad();
		_area.setBad();
		_isSimplePolyg.setBad();
		_centroid.setBad();
	}
};

//------------------------------------------------------------------
/// Returns the bounding box of points in vector/list/array of points \c vpts (free function)
/**
\todo This loops twice on the points. Maybe some improvement here.
*/
template<
	typename T,
	typename std::enable_if<
		trait::IsContainer<T>::value,
		T
	>::type* = nullptr
>
PointPair_<typename T::value_type::FType>
getBB_Points( const T& vpts )
{
	HOMOG2D_START;
	using FPT = typename T::value_type::FType;
	HOMOG2D_DEBUG_ASSERT( static_cast<bool>(vpts.size()), "cannot run with no points" );

	auto mm_x = std::minmax_element(
		std::begin( vpts ),
		std::end( vpts ),
		[]                  // lambda
		( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2 )
		{
			return pt1.getX() < pt2.getX();
		}
	);
	auto mm_y = std::minmax_element(
		std::begin( vpts ),
		std::end( vpts ),
		[]                  // lambda
		( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2 )
		{
			return pt1.getY() < pt2.getY();
		}
	);

	auto p1 = Point2d_<HOMOG2D_INUMTYPE>( mm_x.first->getX(),   mm_y.first->getY()  );
	auto p2 = Point2d_<HOMOG2D_INUMTYPE>( mm_x.second->getX(),  mm_y.second->getY() );

#ifndef HOMOG2D_NOCHECKS
	if( p1.distTo( p2 ) < thr::nullDistance() )
		HOMOG2D_THROW_ERROR_1(
			"unable to compute bounding box of set, identical points:\n -p1:"<< p1 << "\n -p2:" << p2
		);
	if( shareCommonCoord( p1, p2 ) )
		HOMOG2D_THROW_ERROR_1(
			"unable to compute bounding box of set, points share common coordinate:\n -p1:"
			<< p1 << "\n -p2:" << p2
		);
#endif // HOMOG2D_NOCHECKS

	return std::make_pair( p1, p2 );
}

//------------------------------------------------------------------
/// Returns the bounding box of segments in vector/list/array of points \c vsegs
template<
	typename T,
	typename std::enable_if<
		trait::IsContainer<T>::value,
		T
	>::type* = nullptr
>
FRect_<typename T::value_type::FType>
getBB_Segments( const T& vsegs )
{
	HOMOG2D_START;
	using FPT = typename T::value_type::FType;

	HOMOG2D_DEBUG_ASSERT( vsegs.size(), "cannot compute bounding box of empty set of segments" );
	std::vector<Point2d_<FPT>> vpts( vsegs.size()*2 );
	auto it = vpts.begin();
	for( const auto& seg: vsegs )
	{
		auto ppts = seg.getPts();
		*it++ = ppts.first;
		*it++ = ppts.second;
	}
	return FRect_<typename T::value_type::FType>( getBB_Points( vpts ) );
}

/// get BB for a set of FRect_ objects
/// \todo same as getBB_Segments() ???
template<typename FPT>
auto
getBB_FRect( const std::vector<FRect_<FPT>>& v_rects )
{
	HOMOG2D_START;
	HOMOG2D_DEBUG_ASSERT( v_rects.size(), "cannot compute bounding box of empty set of rectangles" );

	std::vector<Point2d_<FPT>> vpts( v_rects.size()*2 );
	auto it = vpts.begin();
	for( const auto& seg: v_rects )
	{
		auto ppts = seg.getPts();
		*it++ = ppts.first;
		*it++ = ppts.second;
	}
	return FRect_<FPT>( getBB_Points( vpts ) );
}

/// Private helper function for base::PolylineBase::getSegs() and base::PolylineBase::getOSegs()
template<typename ST,typename PLT, typename FPT> // Segment Type, PolyLine Type, Floating Point Type
std::vector<ST>
p_getSegs( const base::PolylineBase<PLT,FPT>& pl, const ST& )
{
	auto siz = pl.size();
	if( siz < 2 ) // nothing to return
		return std::vector<ST>();

	std::vector<ST> out;
	out.reserve( siz );
	for( size_t i=0; i<siz-1; i++ )
	{
		const auto& pt1 = pl.getPoint(i);
		const auto& pt2 = pl.getPoint(i+1);
		out.emplace_back( ST(pt1,pt2) );
	}
	if constexpr( std::is_same_v<PLT,typ::IsClosed> )
		out.push_back( ST(pl.getPoint(siz-1), pl.getPoint(0) ) );
	return out;
}

} // namespace priv

// Forward declaration
template<typename FPT1,typename FPT2>
CPolyline_<FPT1>
operator * ( const Homogr_<FPT2>&, const FRect_<FPT1>& );

// Forward declaration
///  \todo 20230215: check if this has to be moved in the 'base' namespace. Can it be called **without**
/// `using namespace std;`. This was the case for the streaming operator...
template<typename FPT1,typename FPT2,typename PLT2>
auto
operator * ( const Homogr_<FPT2>&, const base::PolylineBase<PLT2,FPT1>& ) -> base::PolylineBase<PLT2,FPT1>;


//------------------------------------------------------------------
/// Parameters for base::PolylineBase::getOffsetPoly()
/**
(Unused at present)
*/
struct OffsetPolyParams
{
	bool _angleSplit = false;
};


namespace base {

//------------------------------------------------------------------
/// Polyline, will be instanciated either as \ref OPolyline_ (open polyline) or \ref CPolyline_
/**
\warning When closed, in order to be able to compare two objects describing the same structure
but potentially in different order, the comparison operator will proceed a sorting.<br>
The consequence is that when adding points, if you have done a comparison before, you might not
add point after the one you thought!

template args:
 - PLT: PolyLine Type: typ::IsClosed or typ::IsOpen
 - FPT: Floating Point Type
*/
template<typename PLT,typename FPT>
class PolylineBase: public detail::Common<FPT>
#ifdef HOMOG2D_ENABLE_PRTP
, public rtp::Root
#endif
{
public:
	using FType = FPT;
//	using PType = PLT;
	using SType = std::conditional<std::is_same_v<PLT,typ::IsClosed>,typ::T_CPol,typ::T_OPol>;

	using detail::Common<FPT>::isInside;

	template<typename T1,typename T2> friend class PolylineBase;
	template<typename T1> friend class h2d::Ellipse_;

	template<typename FPT1,typename FPT2>
	friend CPolyline_<FPT1>
	h2d::operator * ( const h2d::Homogr_<FPT2>&, const h2d::FRect_<FPT1>& );

	template<typename FPT1,typename FPT2,typename PLT2>
	friend auto
	h2d::operator * ( const Homogr_<FPT2>&, const base::PolylineBase<PLT2,FPT1>& ) -> base::PolylineBase<PLT2,FPT1>;

	Type type() const
	{
		if constexpr( std::is_same_v<PLT,typ::IsClosed> )
			return Type::CPolyline;
		else
			return Type::OPolyline;
	}

private:
	mutable std::vector<Point2d_<FPT>> _plinevec;
	mutable bool _plIsNormalized = false;
	mutable priv::PolylineAttribs _attribs;    ///< Attributes. Will get stored upon computing.

public:
/// \name Constructors
///@{

/// Default constructor
	PolylineBase() = default;

/// Constructor from FRect. Enabled for "closed" type, disabled for "open" type
	template<typename FPT2>
	PolylineBase( const FRect_<FPT2>& rect )
	{
		static_assert( std::is_same_v<PLT,typ::IsClosed>, "cannot build an OPolyline object from a FRect");
		for( const auto& pt: rect.get4Pts() )
			_plinevec.push_back( pt );
	}

	template<typename FPT2>
	PolylineBase( const Segment_<FPT2>& seg )
	{
		const auto& ppts = seg.getPts();
		p_addPoint( ppts.first );
		p_addPoint( ppts.second );
	}

/// Constructor from a vector of points.
template<
	typename T,
	typename std::enable_if<
		trait::IsContainer<T>::value,
		T
	>::type* = nullptr
>
	PolylineBase( const T& vec )
	{
		set( vec );
	}

/// Constructor that builds a regular convex polygon of \c n points at a distance \c rad, centered at (0,0).
	template<typename FPT2,typename T>
	PolylineBase( FPT2 rad, T n )
	{
		set( rad, n );
	}

/// Copy-Constructor from Closed Polyline
	template<typename FPT2>
	constexpr PolylineBase( const CPolyline_<FPT2>& other )
	{
		static_assert(
			!(std::is_same<PLT,typ::IsOpen>::value),
			"Error, cannot build an Open Polyline from a closed one"
		);
		set( other._plinevec );
	}
/// Copy-Constructor from Open Polyline
	template<typename FPT2>
	PolylineBase( const OPolyline_<FPT2>& other )
	{
		set( other._plinevec );
	}

#ifdef HOMOG2D_USE_BOOSTGEOM
/// Constructor from a boost geometry polygon, see misc/test_files/bg_test_1.cpp
/**
\note Only imports the "outer" envelope, homog2d does not handle polygons with holes

Requirements: the Boost polygon must have 2-cartesian coordinates points,
either `bg::model::point` or `bg::model::d2::point_xy`
but the underlying numerical type is free.

\note At present, the 3th template parameter (bool) (Closed or Open) is ignored, because it is unclear how
this relates to the actual fact that last point is equal to first point.
\todo 20230216:  add some checking that the type BPT needs to fit certain requirements
(must have 2-dimensions, and use cartesian coordinates). Maybe we should add some Sfinae to check this.
*/
	template<typename BPT,bool CLKW,bool CLOSED> //
	PolylineBase(
		const boost::geometry::model::polygon<
			BPT,     ///< Boost Point Type (either `bg::model::point` or `bg::model::d2::point_xy`)
			CLKW,    ///< this one is ignored here
			CLOSED   ///< true: closed, false: open
		>& bgpol  ///< input boost geometry polygon
	)
	{
		const auto& outer = bgpol.outer();
		bool isClosed = false;
		auto ptf = outer.front();
		auto ptb = outer.back();
		Point2d_<HOMOG2D_INUMTYPE> pt_front( boost::geometry::get<0>(ptf), boost::geometry::get<1>(ptf) );
		Point2d_<HOMOG2D_INUMTYPE> pt_back(  boost::geometry::get<0>(ptb), boost::geometry::get<1>(ptb) );
/*
this should work !!! (but doesn't...)
		Point2d_<HOMOG2D_INUMTYPE> pt_front( outer.front() );
		Point2d_<HOMOG2D_INUMTYPE> pt_back(  outer.back() );
*/
		if( pt_front == pt_back ) // means it's closed
			isClosed = true;

		if( isClosed && std::is_same_v<PLT,typ::IsOpen> ) // cannot build an open polyline from a closed one
			HOMOG2D_THROW_ERROR_1( "unable to convert a closed boost::polygon into an OPolyline" );

		_plinevec.reserve( outer.size() - isClosed );
		for( auto it=outer.begin(); it!=outer.end()-isClosed; it++ )
		{
			const auto& bgpt = *it;
			_plinevec.emplace_back(
				Point2d_<FPT>( boost::geometry::get<0>(bgpt), boost::geometry::get<1>(bgpt) )
			);
		}
	}
#endif

///@}

public:
/// \name Attributes access
///@{

/// Returns the number of points
	size_t size() const { return _plinevec.size(); }

	constexpr bool isClosed() const
	{
		if constexpr( std::is_same_v<PLT,typ::IsClosed> )
			return true;
		else
			return false;
	}

	HOMOG2D_INUMTYPE length()    const;
	HOMOG2D_INUMTYPE area()      const;
	bool             isSimple()  const;

/// Returns Bounding Box of Polyline
	auto getBB() const
	{
		HOMOG2D_START;
#ifndef HOMOG2D_NOCHECKS
		if( size() < 2 )
			HOMOG2D_THROW_ERROR_1( "cannot compute bounding box of empty Polyline" );
#endif
		auto ppts = priv::getBB_Points( getPts() );
#ifndef HOMOG2D_NOCHECKS
		if( shareCommonCoord( ppts.first, ppts.second ) )
			HOMOG2D_THROW_ERROR_1( "unable, points share common coordinate" );
#endif
		return FRect_<HOMOG2D_INUMTYPE>( ppts );
	}

	LPBase<typ::IsPoint,HOMOG2D_INUMTYPE> centroid() const;

/// Returns the number of segments. If "closed",
/** the last segment (going from last to first point) is counted */
	size_t nbSegs() const
	{
		if( size() < 2 )
			return 0;
		if constexpr( std::is_same_v<PLT,typ::IsClosed> )
			return size();
		else
			return size() - 1;
	}

	Point2d_<FPT> getExtremePoint( CardDir ) const;
	Point2d_<FPT> getTmPoint() const;
	Point2d_<FPT> getBmPoint() const;
	Point2d_<FPT> getLmPoint() const;
	Point2d_<FPT> getRmPoint() const;

	bool isConvex() const;

///@}

private:
	HOMOG2D_INUMTYPE p_ComputeSignedArea() const;

/// Add single point. private, because only to be used from other member functions
/**
\warning This function was discarded from public API in dec. 2021, because
this will add the new point after the previous one \b only if the object has \b not
been normalized. This normalizing operation will happen if you do a comparison (== or !=)
*/
	template<typename FPT2>
	void p_addPoint( const Point2d_<FPT2>& pt )
	{
#ifndef HOMOG2D_NOCHECKS
		if( size() )
			if( pt == _plinevec.back() )
				HOMOG2D_THROW_ERROR_1(
					"cannot add a point identical to previous one: pt=" << pt << " size=" << size()
				);
#endif
		_attribs.setBad();
		_plIsNormalized=false;
		_plinevec.push_back( pt );
	}

public:

/// \name Data access
///@{

/// Returns the points (reference)
	std::vector<Point2d_<FPT>>& getPts()
	{
		_attribs.setBad();  // because we send the non-const reference
		_plIsNormalized=false;
		return _plinevec;
	}
/// Returns the points (const reference)
	const std::vector<Point2d_<FPT>>& getPts() const
	{
		return _plinevec;
	}

/// Returns (as a copy) the oriented segments of the polyline
	std::vector<OSegment_<FPT>> getOSegs() const
	{
		return priv::p_getSegs( *this, OSegment_<FPT>() );
	}

/// Returns (as a copy) the segments of the polyline
	std::vector<Segment_<FPT>> getSegs() const
	{
		return priv::p_getSegs( *this, Segment_<FPT>() );
	}

/// Returns one point of the polyline.
	Point2d_<FPT> getPoint( size_t idx ) const
	{
#ifndef HOMOG2D_NOCHECKS
		if( idx >= size() )
			HOMOG2D_THROW_ERROR_1( "requesting point " << idx
				<< ", only has "  << size()
			);
#endif
		return _plinevec[idx];
	}

private:
/// Private helper function
	template<typename ST>
	ST impl_getSegment( size_t idx, const ST& ) const
	{
	#ifndef HOMOG2D_NOCHECKS
		if( idx >= nbSegs() )
			HOMOG2D_THROW_ERROR_1( "requesting segment " << idx
				<< ", only has "  << nbSegs()
			);

		if( size() < 2 )
			HOMOG2D_THROW_ERROR_1( "empty, no segment " << idx );
	#endif
	if constexpr( std::is_same_v<PLT,typ::IsClosed> )
		return ST( _plinevec[idx], _plinevec[idx+1==nbSegs()?0:idx+1] );
	else
		return ST( _plinevec[idx], _plinevec[idx+1] );
	}

public:
/// Returns one oriented segment of the polyline.
	OSegment_<FPT> getOSegment( size_t idx ) const
	{
		return impl_getSegment( idx, OSegment_<FPT>() );
	}

/// Returns one segment of the polyline.
/**
Segment \c n is the one between point \c n and point \c n+1
*/
	Segment_<FPT> getSegment( size_t idx ) const
	{
		return impl_getSegment( idx, Segment_<FPT>() );
	}

	CPolyline_<FPT> convexHull() const;
///@}

public:
/// \name Modifiers (non-const member functions)
///@{

/// Clear all (does not change the "open/close" status).
	void clear()
	{
		_plinevec.clear();
		_plIsNormalized=false;
		_attribs.setBad();
	}

	template<typename FPT2>
	void rotate( Rotate, const Point2d_<FPT2>& );
	void rotate( Rotate );

/// Miminize the PolylineBase: remove all points that lie in the middle of two segments with same angle.
/**
For example, if we have the following points ("Open" polyline):
(0,0)-(1,0)-(2,0)<br>
This will be replaced by: (0,0)--(2,0)

\note We cannot use the Segment_::getAngle() function because it returns a
value in [0,PI/2], so we would'nt be able to detect a segment going
at 180° of the previous one.

\todo 20230217: implement these:
- https://en.wikipedia.org/wiki/Visvalingam%E2%80%93Whyatt_algorithm
- https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm
Also use the areCollinear() function
*/
	void
	minimize()
	{
		if( size()<3 )
			return;
		impl_minimizePL( detail::PlHelper<PLT>() );
	}

/// Translate Polyline using \c dx, \c dy
	template<typename TX,typename TY>
	void translate( TX dx, TY dy )
	{
		HOMOG2D_CHECK_IS_NUMBER( TX );
		HOMOG2D_CHECK_IS_NUMBER( TY );
		for( auto& pt: _plinevec )
			pt.translate( dx, dy );
	}

/// Translate Polyline, using a pair of numerical values
	template<typename T1, typename T2>
	void translate( const std::pair<T1,T2>& ppt )
	{
		translate( ppt.first, ppt.second );
	}

/// Move Polyline to new origin
	template<typename TX,typename TY>
	void moveTo( TX x, TY y )
	{
		HOMOG2D_CHECK_IS_NUMBER( TX );
		HOMOG2D_CHECK_IS_NUMBER( TY );
		moveTo( Point2d_<FPT>(x,y) );
	}

/// Move Polyline to new origin, given by \c new_org
/**
\warning The polygon origin is NOT its center but the point No 0!
*/
	template<typename T1>
	void moveTo( const Point2d_<T1>& new_org )
	{
		if( size() == 0 )
			HOMOG2D_THROW_ERROR_1( "Invalid call, Polyline is empty" );
		auto dx = new_org.getX() - getPoint(0).getX();
		auto dy = new_org.getY() - getPoint(0).getY();
		for( auto& pt: _plinevec )
			pt.translate( dx, dy );
	}

/// Set from vector/array/list of points (discards previous points)
/**
- nb of elements must be 0 or 2 or more
*/
	template<
		typename CONT,
		typename std::enable_if<
			trait::IsContainer<CONT>::value,
			CONT
		>::type* = nullptr
	>
	void set( const CONT& vec )
	{
#ifndef HOMOG2D_NOCHECKS
		if( vec.size() == 1 )
			HOMOG2D_THROW_ERROR_1( "Invalid: number of points must be 0, 2 or more" );
		if( vec.size() > 1 )
			p_checkInputData( vec );
#endif
		_attribs.setBad();
		_plIsNormalized=false;
		_plinevec.resize( vec.size() );
		auto it = std::begin( _plinevec );
		for( const auto& pt: vec )   // copying one by one will allow
			*it++ = pt;              //  type conversions (std::copy implies same FP type)
	}

	template<typename FPT1,typename FPT2,typename FPT3>
	void setParallelogram(
		const Point2d_<FPT1>& pt1,
		const Point2d_<FPT2>& pt2,
		const Point2d_<FPT3>& pt3
	);

/// Set from FRect
	template<typename FPT2>
	void set( const FRect_<FPT2>& rect )
	{
		static_assert( std::is_same_v<PLT,typ::IsClosed>, "Invalid: cannot set a OPolyline from a FRect" );

		CPolyline_<FPT> tmp(rect);
		std::swap( *this, tmp );
	}

/// Build RCP (Regular Convex Polygon), and return distance between consecutive points
	template<typename FPT2,typename T>
	std::pair<HOMOG2D_INUMTYPE,HOMOG2D_INUMTYPE>
	set( FPT2 rad, T n );

///@}

private:
/// Checks that no contiguous identical points are stored
	template<typename CONT>
	void p_checkInputData( const CONT& pts )
	{
		for( auto it=pts.begin(); it!=std::prev(pts.end()); it++ )
		{
			const auto& elem = *it;
			const auto& next = *std::next(it);
			if( elem == next )
				HOMOG2D_THROW_ERROR_1(
					"cannot add two consecutive identical points:\npt:"
					<< elem << " and pt:" << next
					<< " in set of size " << pts.size()
				);
		}
		if( pts.front() == pts.back() )
			HOMOG2D_THROW_ERROR_1(
				"cannot add first point equal to last point:\npt:"
				<< pts.front()
				<< " in set of size " << pts.size()
			);
	}

	void impl_minimizePL( const detail::PlHelper<typ::IsOpen>& );
	void impl_minimizePL( const detail::PlHelper<typ::IsClosed>& );
	void p_minimizePL( PolylineBase<PLT,FPT>&, size_t istart, size_t iend );

public:
/// \name Operators
///@{
	template<typename PLT2,typename FPT2>
	bool operator == ( const PolylineBase<PLT2,FPT2>& other ) const
	{
		if constexpr( std::is_same_v<PLT,PLT2> )
		{
			if( size() != other.size() )          // for quick exit
				return false;

			p_normalizePoly();
			other.p_normalizePoly();

			auto it = other._plinevec.begin();
			for( const auto& elem: _plinevec )
				if( *it++ != elem )
					return false;
			return true;
		}
		else
			return false;
	}

	template<typename PLT2,typename FPT2>
	bool operator != ( const PolylineBase<PLT2,FPT2>& other ) const
	{
		return !( *this == other );
	}
///@}

/// Polyline intersection with Line, Segment, FRect, Circle
	template<
		typename T,
		typename std::enable_if<
			trait::IsShape<T>::value,
			T
		>::type* = nullptr
	>
	detail::IntersectM<FPT> intersects( const T& other ) const
	{
		detail::IntersectM<FPT> out;
		for( const auto& pseg: getSegs() )
		{
			auto inters = pseg.intersects( other );
			if( inters() )
				out.add( inters.get() );
		}
		return out;
	}

/// Polyline isInside other primitive
/**
Sfinae should resolve for T=Circle,FRect,Ellipse but not for CPolyline
*/
	template<
		typename T,
		typename std::enable_if<
				(
					trait::HasArea<T>::value
					&&
					!trait::PolIsClosed<T>::value
				)
				,T
		>::type* = nullptr
	>
	bool
	isInside( const T& prim ) const
	{
		HOMOG2D_START;
		if( size() == 0 )
			return false;
		for( const auto& pt: getPts() )
			if( !pt.isInside( prim ) )
				return false;
		return true;
	}

/// Polyline isInside other CPolyline
/**
Sfinae should resolve ONLY for CPolyline
*/
	template<
		typename T,
		typename std::enable_if<
			trait::PolIsClosed<T>::value,T
		>::type* = nullptr
	>
	bool
	isInside( const T& cpol ) const
	{
		HOMOG2D_START;
		if( size() == 0 )
			return false;
		for( const auto& pt: getPts() )
			if( !pt.isInside( cpol ) )
				return false;
		if( intersects(cpol)() )
			return false;
		return true;
	}

	template<typename T1,typename T2>
	friend std::ostream&
	h2d::base::operator << ( std::ostream&, const h2d::base::PolylineBase<T1,T2>& );

#ifdef HOMOG2D_TEST_MODE
/// this is only needed for testing
	bool isNormalized() const { return _plIsNormalized; }
#endif

private:
	void p_normalizePoly() const;

public:
	template<typename T>
	PolylineBase<typ::IsClosed,FPT>
	getOffsetPoly( T value, OffsetPolyParams p=OffsetPolyParams{} ) const;

	std::vector<Line2d_<HOMOG2D_INUMTYPE>>
	getBisectorLines() const
	{
		 return h2d::getBisectorLines( *this );
	}

	void draw( img::Image<img::SvgImage>&, img::DrawParams dp=img::DrawParams() ) const;
#ifdef HOMOG2D_USE_OPENCV
	void draw( img::Image<cv::Mat>&,       img::DrawParams dp=img::DrawParams() ) const;
#endif

}; // class PolylineBase


//------------------------------------------------------------------
/// Return an "offsetted" closed polyline, requires simple polygon (CPolyline AND no crossings) as input
/**
On failure (for whatever reason), will return an empty CPolyline

- If dist>0, returns the polyline "outside" the source one
- If dist<0, returns the polyline "inside" the source one
*/
template<typename PLT,typename FPT>
template<typename T>
PolylineBase<typ::IsClosed,FPT>
PolylineBase<PLT,FPT>::getOffsetPoly( T dist, OffsetPolyParams params ) const
{
	HOMOG2D_CHECK_IS_NUMBER(T);
	auto dist0 = (HOMOG2D_INUMTYPE)dist; // casting

	bool valid = true;
	if( homog2d_abs(dist) < thr::nullDistance() )
	{
		HOMOG2D_LOG_WARNING( "Failure, distance value is null, returning empty CPolyline" );
		valid = false;
	}
	if( size()<3 )
	{
		HOMOG2D_LOG_WARNING( "Failure, computing offsetted Polyline requires at least 3 points, returning empty CPolyline" );
		valid = false;
	}
	if( !isSimple() )
	{
		HOMOG2D_LOG_WARNING( "Failure, Polyline is not a polygon, returning empty CPolyline" );
		valid = false;
	}
	if( !valid )
		return PolylineBase<typ::IsClosed,FPT>();

	p_normalizePoly();

	size_t current = 0;
//	bool paraLines = false;

	std::vector<Point2d_<FPT>> v_out;
	auto osegs = getOSegs();
	auto oseg1 = osegs.at(current);
	do
	{
		auto next = (current==size()-1 ? 0 : current+1);

		auto pt1 = getPoint(next);

		auto oseg2 = osegs.at(next);

		auto psegs1 = oseg1.getParallelSegs(homog2d_abs(dist0));
		auto psegs2 = oseg2.getParallelSegs(homog2d_abs(dist0));

		auto pseg1 = &psegs1.first;
		auto pseg2 = &psegs2.first;
		if( dist < 0 )
		{
			pseg1 = &psegs1.second;
			pseg2 = &psegs2.second;
		}

		auto li1 = pseg1->getLine();
		auto li2 = pseg2->getLine();
		if( !areParallel(li1,li2) )
		{
			auto pt_int = li1 * li2;
			if( !params._angleSplit || getAngle(oseg1,oseg2)>0 || dist < 0 )
			{
				v_out.push_back( pt_int );  // add point intersection of the two lines
			}
			else
			{
				auto oseg = OSegment_<HOMOG2D_INUMTYPE>( pt1, pt_int );
				auto dist_cut = std::min( oseg.length(), dist0 );

				auto pt_cut = oseg.getPointAt( dist_cut );
				auto oli = oseg.getLine().getOrthogLine( pt_cut );

				auto pt_cut1 = oli * li1;
				auto pt_cut2 = oli * li2;

				v_out.push_back( pt_cut1 );
				if( pt_cut1 != pt_cut2 )        // so we don't add two identical pts (in case they were computed as such)
					v_out.push_back( pt_cut2 );
			}
		}
//		else
//			std::cout << "parallel!\n";

		current++;
		oseg1 = oseg2;
	}
	while( current<size() ); //&& !paraLines );

	return PolylineBase<typ::IsClosed,FPT>( v_out );
}

//------------------------------------------------------------------
/// Build a parallelogram (4 points) from 3 points
template<typename PLT,typename FPT>
template<typename FPT1,typename FPT2,typename FPT3>
void
PolylineBase<PLT,FPT>::setParallelogram(
	const Point2d_<FPT1>& p1,
	const Point2d_<FPT2>& p2,
	const Point2d_<FPT3>& p3
)
{
	static_assert( std::is_same_v<PLT,typ::IsClosed>, "Invalid: cannot set a OPolyline as a parallelogram" );

	Point2d_<HOMOG2D_INUMTYPE> pt1(p1);
	Point2d_<HOMOG2D_INUMTYPE> pt2(p2);
	Point2d_<HOMOG2D_INUMTYPE> pt3(p3);

	std::vector<Point2d_<FPT>> vpts(4);
	vpts[0] = pt1;
	vpts[1] = pt2;
	vpts[2] = pt3;

	auto li_21 = pt1 * pt2;
	auto li_23 = pt3 * pt2;
	auto li_34 = li_21.getParallelLine( pt3 );
	auto li_14 = li_23.getParallelLine( pt1 );
	vpts[3] = li_34 * li_14;
	set( vpts );
}

//------------------------------------------------------------------
/// Build a Regular Convex Polygon of radius \c rad with \c n points, centered at (0,0)
/**
\return segment distance, inscribed circle radius
*/
template<typename PLT,typename FPT>
template<typename FPT2,typename T>
std::pair<HOMOG2D_INUMTYPE,HOMOG2D_INUMTYPE>
PolylineBase<PLT,FPT>::set(
	FPT2 rad,   ///< Radius
	T    ni     ///< Nb of points
)
{
	static_assert(
		std::is_integral<T>::value,
		"2nd argument type must be integral type"
	);
	static_assert(
		std::is_same_v<PLT,typ::IsClosed>,
		"Cannot build a RCP as open polyline"
	);

	if( ni < 3 )
		HOMOG2D_THROW_ERROR_1( "unable, nb of points must be > 2" );
	if( rad <= 0  )
		HOMOG2D_THROW_ERROR_1( "unable, radius must be > 0" );

	size_t n(ni);
	std::vector<Point2d_<HOMOG2D_INUMTYPE>> v_pts(n);
	auto it = std::begin( v_pts );
	auto angle0 = (HOMOG2D_INUMTYPE)2. * M_PI / n;

	Point2d_<HOMOG2D_INUMTYPE> pt0( rad, 0. ); // initial point
	HOMOG2D_INUMTYPE radius = 0.;

	for( size_t i=0; i<n; i++ )
	{
		auto angle = angle0 * i;
		auto x = homog2d_cos( angle );
		auto y = homog2d_sin( angle );

		if( i == 1 )    // compute radius of inscribed circle
		{
			auto pt1 = Point2d_<HOMOG2D_INUMTYPE>( x * rad, y * rad );
			Segment_<HOMOG2D_INUMTYPE> seg( pt0, pt1 );
			Line2d_<HOMOG2D_INUMTYPE> li( seg.getCenter() );     // line passing through (0,0) and middle point of segment
			auto inters_pt = seg.intersects( li ) ;              // intersection point
			assert( inters_pt() );
			radius = Point2d_<HOMOG2D_INUMTYPE>(0,0).distTo( inters_pt.get() );
		}

		*it = Point2d_<HOMOG2D_INUMTYPE>( x * rad, y * rad );
		it++;
	}
	*this = PolylineBase<PLT,FPT>( v_pts );

	return std::make_pair(
		getPoint(0).distTo( getPoint(1) ),
		radius
	);
}

} // namespace base


// FREE FUNCTIONS
// (need to be here because the 5 below are being called by some member functions
// of class PolylineBase, that is implemented further below

namespace priv {

//------------------------------------------------------------------
/// Return iterator on Bottom-most point of container holding points
/**
Used by
 - getBmPoint()
 - priv::chull::getPivotPoint()
*/
template<
	typename T,
	typename std::enable_if<
		(
			trait::IsContainer<T>::value &&
			std::is_same<typename T::value_type,Point2d_<typename T::value_type::FType>>::value
		), T
	>::type* = nullptr
>
auto
getBmPoint_helper( const T& t )
{
	using FPT = typename T::value_type::FType;
#ifndef HOMOG2D_NOCHECKS
	if( t.size() == 0 )
		HOMOG2D_THROW_ERROR_1( "invalid call, container is empty" );
#endif

	return std::min_element(
		std::begin(t),
		std::end(t),
		[]                  // lambda
		( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2 )
		{
			if( pt1.getY() < pt2.getY() )
				return true;
			if( pt1.getY() > pt2.getY() )
				return false;
			return( pt1.getX() < pt2.getX() );
		}
	);
}

} // namespace priv

//------------------------------------------------------------------
/// Return Bottom-most point of container holding points
template<
	typename T,
	typename std::enable_if<
		(
			trait::IsContainer<T>::value &&
			std::is_same<typename T::value_type,Point2d_<typename T::value_type::FType>>::value
		), T
	>::type* = nullptr
>
auto
getBmPoint( const T& t )
{
#ifndef HOMOG2D_NOCHECKS
	if( t.size() == 0 )
		HOMOG2D_THROW_ERROR_1( "invalid call, container is empty" );
#endif
	auto it = priv::getBmPoint_helper( t );
	return std::make_pair(
		*it,
		std::distance( std::begin(t), it )
	);
}

//------------------------------------------------------------------
/// Return Top-most point of container
template<
	typename T,
	typename std::enable_if<
		(
			trait::IsContainer<T>::value &&
			std::is_same<typename T::value_type,Point2d_<typename T::value_type::FType>>::value
		), T
	>::type* = nullptr
>
auto
getTmPoint( const T& t )
{
	using FPT = typename T::value_type::FType;

#ifndef HOMOG2D_NOCHECKS
	if( t.size() == 0 )
		HOMOG2D_THROW_ERROR_1( "invalid call, container is empty" );
#endif

	auto it = std::min_element(
		std::begin(t),
		std::end(t),
		[]                  // lambda
		( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2 )
		{
			if( pt1.getY() > pt2.getY() )
				return true;
			if( pt1.getY() < pt2.getY() )
				return false;
			return( pt1.getX() < pt2.getX() );
		}
	);
	return std::make_pair(
		*it,
		std::distance( std::begin(t), it )
	);
}

//------------------------------------------------------------------
/// Return Left-most point of container as a pair holding:
/**
- first: the point
- second: the distance (index) from begin position

\todo 20250222: CHANGE RETURN TYPE! => should return an iterator, not a pair
(so this allows usage of any non-random access container)
*/
template<
	typename T,
	typename std::enable_if<
		(
			trait::IsContainer<T>::value &&
			std::is_same<typename T::value_type,Point2d_<typename T::value_type::FType>>::value
		), T
	>::type* = nullptr
>
auto
getLmPoint( const T& t )
{
	using FPT = typename T::value_type::FType;

#ifndef HOMOG2D_NOCHECKS
	if( t.size() == 0 )
		HOMOG2D_THROW_ERROR_1( "invalid call, container is empty" );
#endif

	auto it = std::min_element(
		std::begin(t),
		std::end(t),
		[]                  // lambda
		( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2 )
		{
			if( pt1.getX() < pt2.getX() )
				return true;
			if( pt1.getX() > pt2.getX() )
				return false;
			return( pt1.getY() < pt2.getY() );
		}
	);

	return std::make_pair(
		*it,
		std::distance( std::begin(t), it )
	);
}

//------------------------------------------------------------------
/// Return Right-most point of container holding points
template<
	typename T,
	typename std::enable_if<
		(
			trait::IsContainer<T>::value &&
			std::is_same<typename T::value_type,Point2d_<typename T::value_type::FType>>::value
		), T
	>::type* = nullptr
>
auto
getRmPoint( const T& t )
{
	using FPT = typename T::value_type::FType;

#ifndef HOMOG2D_NOCHECKS
	if( t.size() == 0 )
		HOMOG2D_THROW_ERROR_1( "invalid call, container is empty" );
#endif

	auto it = std::min_element(
		std::begin(t),
		std::end(t),
		[]                  // lambda
		( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2 )
		{
			if( pt1.getX() > pt2.getX() )
				return true;
			if( pt1.getX() < pt2.getX() )
				return false;
			return( pt1.getY() < pt2.getY() );
		}
	);

	return std::make_pair(
		*it,
		std::distance( std::begin(t), it )
	);
}

//------------------------------------------------------------------
/// Return Bottom-most point of Polyline (free function)
template<typename PLT,typename FPT>
Point2d_<FPT>
getBmPoint( const base::PolylineBase<PLT,FPT>& poly )
{
	return getBmPoint( poly.getPts() ).first;
}
/// Return Top-most point of Polyline (free function)
template<typename PLT,typename FPT>
Point2d_<FPT>
getTmPoint( const base::PolylineBase<PLT,FPT>& poly )
{
	return getTmPoint( poly.getPts() ).first;
}
/// Return Left-most point of Polyline (free function)
template<typename PLT,typename FPT>
Point2d_<FPT>
getLmPoint( const base::PolylineBase<PLT,FPT>& poly )
{
	return getLmPoint( poly.getPts() ).first;
}
/// Return Right-most point of Polyline (free function)
template<typename PLT,typename FPT>
Point2d_<FPT>
getRmPoint( const base::PolylineBase<PLT,FPT>& poly )
{
	return getRmPoint( poly.getPts() ).first;
}

//------------------------------------------------------------------
/// Get Top-most / Bottom-most / Left-most / Right-most point, depending on \c dir
/// (free function)
/**
Type \c T can be either a Polyline (open or closed), or a container holding points, or a FRect_
*/
template<typename T>
auto
getExtremePoint( CardDir dir, const T& t )
{
	switch( dir )
	{
		case CardDir::Top:    return getTmPoint(t); break;
		case CardDir::Bottom: return getBmPoint(t); break;
		case CardDir::Left:   return getLmPoint(t); break;
		case CardDir::Right:  return getRmPoint(t); break;
		default: assert(0);
	}
}

namespace base {

//------------------------------------------------------------------
/// Get Top-most / Bottom-most / Left-most / Right-most point
template<typename PLT,typename FPT>
Point2d_<FPT>
PolylineBase<PLT,FPT>::getExtremePoint( CardDir dir ) const
{
	return getExtremePoint( dir, *this );
}

/// Return Bottom-most point of Polyline
template<typename PLT,typename FPT>
Point2d_<FPT>
PolylineBase<PLT,FPT>::getBmPoint() const
{
	return h2d::getBmPoint( *this );
}

/// Return Top-most point of Polyline
template<typename PLT,typename FPT>
Point2d_<FPT>
PolylineBase<PLT,FPT>::getTmPoint() const
{
	return h2d::getTmPoint( *this );
}

/// Return Left-most point of Polyline
template<typename PLT,typename FPT>
Point2d_<FPT>
PolylineBase<PLT,FPT>::getLmPoint() const
{
	return h2d::getLmPoint( *this );
}

/// Return Right-most point of Polyline
template<typename PLT,typename FPT>
Point2d_<FPT>
PolylineBase<PLT,FPT>::getRmPoint() const
{
	return h2d::getRmPoint( *this );
}

//------------------------------------------------------------------
#ifdef HOMOG2D_PRELIMINAR
/// TMP
struct TMP_DebugSquare
{
	Line2d_<double> li0;
	Line2d_<double> line_ortho;
	Line2d_<double> li_L, li_R;

	Segment_<double> s1;
	Segment_<double> sL,sR;

	double dist;
	PointPair_<double>  ppts;
	Point2d_<double> pt_resL, pt_resR;
};

template<typename CONT>
auto
buildSquare( const CONT& pts )
{
	TMP_DebugSquare dbg;

	if( pts.size() != 4 )
		HOMOG2D_THROW_ERROR_1( "Error, size must be = 4, current size=" << pts.size() );

	if( areCollinear( pts[0], pts[1], pts[2] ) )
		HOMOG2D_THROW_ERROR_1( "Unable, points 012 are colinear" );
	if( areCollinear( pts[1], pts[2], pts[3] ) )
		HOMOG2D_THROW_ERROR_1( "Unable, points 123 are colinear" );
	if( areCollinear( pts[1], pts[0], pts[3] ) )
		HOMOG2D_THROW_ERROR_1( "Unable, points 013 are colinear" );
	if( areCollinear( pts[0], pts[2], pts[3] ) )
		HOMOG2D_THROW_ERROR_1( "Unable, points 023 are colinear" );

	auto pt_L = getLmPoint( pts );
	auto pt_R = getRmPoint( pts );
	auto pt_T = getTmPoint( pts );
	auto pt_B = getBmPoint( pts );
	Segment_<HOMOG2D_INUMTYPE> s1( pt_L.first, pt_R.first );
dbg.s1 = s1;
	auto dist = s1.length();
dbg.dist = dist;

	auto line_ortho = s1.getLine().getOrthogLine( pt_T.first );
dbg.line_ortho = line_ortho;

	auto ppts = line_ortho.getPoints( pt_T.first, dist );
dbg.ppts = ppts;
	auto d1 = ppts.first.distTo( pt_L.first );
	auto d2 = ppts.second.distTo( pt_L.first );

	auto goodpt = ppts.first;
	if( d2 < d1 )
		goodpt = ppts.second;

	auto li0 = goodpt * pt_B.first;
dbg.li0 = li0;

	auto li_L = li0.getOrthogLine( pt_L.first );
	auto li_R = li0.getOrthogLine( pt_R.first );
dbg.li_L = li_L;
dbg.li_R = li_R;

	auto pt_resL = li_L * li0;
	auto pt_resR = li_R * li0;
dbg.pt_resL = pt_resL;
dbg.pt_resR = pt_resR;
	auto s_Top = li_L.getOrthogSegment( pt_T.first );
	auto li_Top = s_Top.getLine();

	auto pt_res3 = li_L * li_Top;
	auto pt_res4 = li_R * li_Top;

	std::vector<Point2d_<double>> vout;
	vout.push_back( pt_resL );
	vout.push_back( pt_res3 );
	vout.push_back( pt_res4 );
	vout.push_back( pt_resR );

	return std::make_pair(
		PolylineBase<typ::IsClosed,typename CONT::value_type::FType>( vout ),
		dbg
	);
}

template<typename T1,typename T2,typename T3,typename T4>
PolylineBase<typ::IsClosed,T1>
buildSquare(
	const Point2d_<T1>& p1,
	const Point2d_<T2>& p2,
	const Point2d_<T3>& p3,
	const Point2d_<T4>& p4
)
{
	std::vector<Point2d_<HOMOG2D_INUMTYPE>> v(4);
	v[0] = p1;
	v[1] = p2;
	v[2] = p3;
	v[3] = p4;
	return buildSquare( v );
}
#endif // HOMOG2D_PRELIMINAR
//------------------------------------------------------------------
/// Rotates the polyline by either 90°, 180°, 270° (-90°) at point \c refpt
/**
For an arbitrary angle \c alpha (rad.), you can write:
```
auto poly2 = Homogr(alpha) * poly;
```
\sa PolylineBase<PLT,FPT>::rotate( Rotate )
*/
template<typename PLT,typename FPT>
template<typename FPT2>
void
PolylineBase<PLT,FPT>::rotate( Rotate rot, const Point2d_<FPT2>& refpt )
{
	translate( -refpt.getX(), -refpt.getY() );
	this->rotate( rot );
	translate( refpt.getX(), refpt.getY() );
}

//------------------------------------------------------------------
/// Rotates the polyline by either 90°, 180°, 270° (-90°) at point (0,0)
/**
For an arbitrary angle \c alpha (rad.), you can write:
```
auto poly2 = Homogr(alpha) * poly;
```
\sa PolylineBase<PLT,FPT>::rotate( Rotate, const Point2d_<FPT2>& )
*/
template<typename PLT,typename FPT>
void
PolylineBase<PLT,FPT>::rotate( Rotate rot )
{
	switch( rot )
	{
		case Rotate::CCW:
			for( auto& pt: getPts() )
				pt.set( -pt.getY(), +pt.getX() );
		break;
		case Rotate::CW:
			for( auto& pt: getPts() )
				pt.set( +pt.getY(), -pt.getX() );
		break;
		case Rotate::Full:
			for( auto& pt: getPts() )
				pt.set( -pt.getX(), -pt.getY() );
		break;
		case Rotate::VMirror:
			for( auto& pt: getPts() )
				pt.set( -pt.getX(), pt.getY() );
		break;
		case Rotate::HMirror:
			for( auto& pt: getPts() )
				pt.set( pt.getX(), -pt.getY() );
		break;

		default: assert(0);
	}
}

//------------------------------------------------------------------
/// Private member function, called by PolylineBase::impl_minimizePL()
template<typename PLT,typename FPT>
void
PolylineBase<PLT,FPT>::p_minimizePL( PolylineBase<PLT,FPT>& pl, size_t istart, size_t iend )
{
	auto nbpts = pl.size();
// step 1: check each point to see if it is the middle point of two segments with same angle
	std::vector<size_t> ptset;
	for( size_t i=istart; i<iend; i++ )
	{
		const auto& p0 = pl._plinevec.at(i);
		const auto& pnext = getPoint( i==nbpts-1 ? 0 : i+1 );
		const auto& pprev = getPoint( i==0 ? nbpts-1 : i-1 );

		auto vx1 = pnext.getX() - p0.getX();
		auto vy1 = pnext.getY() - p0.getY();
		auto vx2 = p0.getX() - pprev.getX();
		auto vy2 = p0.getY() - pprev.getY();
		auto a1 = std::atan2( vx1, vy1 );
		auto a2 = std::atan2( vx2, vy2 );

		if( homog2d_abs(a1-a2) < thr::nullAngleValue() )
			ptset.push_back( i );
	}

	if( ptset.size() == 0 ) // if no same angle segment (=no "middle point")
		return;             // then no change

// step 2: build new Polyline without those points
	PolylineBase<PLT,FPT> out;
	size_t vec_idx = 0;
	for( size_t i=0; i<nbpts; i++ )
	{
//		HOMOG2D_LOG("ptset.size()=" << ptset.size() << " vec_idx=" << vec_idx );

		if( vec_idx<ptset.size() ) // if there is more points to remove
		{
			if( ptset.at(vec_idx) != i )               // if regular point, add it
				out.p_addPoint( pl._plinevec.at(i) );  //  to the output set
			else                                       // else, we found a "middle point"
				vec_idx++;                             // and switch to next one
		}
		else                                        // no more points to remove
			out.p_addPoint( pl._plinevec.at(i) );   // so just add the point to the output set
	}
	std::swap( out, pl ); // maybe we can "move" instead?
}

template<typename PLT,typename FPT>
void
PolylineBase<PLT,FPT>::impl_minimizePL( const detail::PlHelper<typename typ::IsOpen>& )
{
	assert( size() > 2 );
	p_minimizePL( *this, 1, size()-1 );
}

template<typename PLT,typename FPT>
void
PolylineBase<PLT,FPT>::impl_minimizePL( const detail::PlHelper<typename typ::IsClosed>& )
{
	p_minimizePL( *this, 0, size() );
}


//------------------------------------------------------------------
/// Returns true if object is a polygon (closed, and no segment crossing)
template<typename PLT,typename FPT>
bool
PolylineBase<PLT,FPT>::isSimple() const
{
	if( size()<3 )       // needs at least 3 points to be a polygon
		return false;
	if constexpr( std::is_same_v<PLT,typ::IsOpen> ) // If open, then not a polygon
		return false;
	else                  // If closed, we need to check for crossings
	{
		if( _attribs._isSimplePolyg.isBad() )
		{
			auto nbs = nbSegs();
			size_t i=0;
			bool notDone = true;
			bool hasIntersections = false;
			do
			{
				auto seg1 = getSegment(i);
				auto lastone = i==0?nbs-1:nbs;
				for( auto j=i+2; j<lastone; j++ )
					if( getSegment(j).intersects(seg1)() )
					{
						notDone = false;
						hasIntersections = true;
						break;
					}
				i++;
			}
			while( i<nbs && notDone );
			_attribs._isSimplePolyg.set( !hasIntersections );
		}
		return _attribs._isSimplePolyg.value();
	}
}
//------------------------------------------------------------------
/// Returns true if polygon is convex
/**
This implies that:
 - the Polyline is a Polygon
 - cross product of consecutive points have same sign
 (see https://stackoverflow.com/a/1881201/193789 )

\todo 20221120: as points are homogeneous, the cross product can probably be computed
in a different way (quicker? simpler?).
Need to check this.

\todo 20250127: use crossProduct free function with base::SegVec args
*/
template<typename PLT,typename FPT>
bool
PolylineBase<PLT,FPT>::isConvex() const
{
	if( !isSimple() )
		return false;

	int8_t sign = 0;
	const auto& vpts = getPts();
	if( vpts.size() == 3 )   // 3 pts => always convex
		return true;

	for( size_t i=0; i<vpts.size(); i++ )
	{
		const auto& pt0 = vpts[ i==0?vpts.size()-1:i-1 ];
		const auto& pt1 = vpts[ i ];
		const auto& pt2 = vpts[ i==vpts.size()-1?0:i+1 ];
		auto dx1 = pt1.getX() - pt0.getX();
		auto dy1 = pt1.getY() - pt0.getY();

		auto dx2 = pt2.getX() - pt1.getX();
		auto dy2 = pt2.getY() - pt1.getY();

		auto crossproduct = dx1*dy2 - dy1*dx2;
		if( sign == 0 )                          // initial sign value
			sign = (crossproduct>0 ? +1 : -1);
		else
			if (sign != (crossproduct>0 ? +1 : -1) )
				return false;
	}
	return true;
}

//------------------------------------------------------------------
/// Returns length of Polyline
template<typename PLT,typename FPT>
HOMOG2D_INUMTYPE
PolylineBase<PLT,FPT>::length() const
{
	if( _attribs._length.isBad() )
	{
		HOMOG2D_INUMTYPE sum = 0.;
		for( const auto& seg: getSegs() )
			sum += static_cast<HOMOG2D_INUMTYPE>( seg.length() );
		_attribs._length.set( sum );
	}
	return _attribs._length.value();
}

//------------------------------------------------------------------
/// Returns area of polygon (computed only if necessary)
template<typename PLT,typename FPT>
HOMOG2D_INUMTYPE
PolylineBase<PLT,FPT>::area() const
{
	if( !isSimple() )  // implies that is both closed and has no intersections
		return 0.;

	if( _attribs._area.isBad() )
		_attribs._area.set( homog2d_abs( p_ComputeSignedArea() ) );
	return _attribs._area.value();
}

//------------------------------------------------------------------
/// Compute and returns signed area (used in area() and in centroid() )
template<typename PLT,typename FPT>
HOMOG2D_INUMTYPE
PolylineBase<PLT,FPT>::p_ComputeSignedArea() const
{
	HOMOG2D_INUMTYPE area = 0.;
	for( size_t i=0; i<size(); i++ )
	{
		auto j = (i == size()-1 ? 0 : i+1);
		auto pt1 = _plinevec[i];
		auto pt2 = _plinevec[j];
		area += static_cast<HOMOG2D_INUMTYPE>(pt1.getX()) * pt2.getY();
		area -= static_cast<HOMOG2D_INUMTYPE>(pt1.getY()) * pt2.getX();
	}
	return area / 2.;
}

//------------------------------------------------------------------
/// Compute centroid of polygon
/**
ref: https://en.wikipedia.org/wiki/Centroid#Of_a_polygon

\warning: centroid CAN be outside of polygon!
*/
template<typename PLT,typename FPT>
Point2d_<HOMOG2D_INUMTYPE>
base::PolylineBase<PLT,FPT>::centroid() const
{
	if( !isSimple() )
		HOMOG2D_THROW_ERROR_1( "unable, Polyline object is not simple" );

	if( _attribs._centroid.isBad() )
	{
		HOMOG2D_INUMTYPE cx = 0.;
		HOMOG2D_INUMTYPE cy = 0.;
		for( size_t i=0; i<size(); i++ )
		{
			auto j = (i == size()-1 ? 0 : i+1);
			const auto& pt1 = _plinevec[i];
			const auto& pt2 = _plinevec[j];
			auto x1 = pt1.getX();
			auto x2 = pt2.getX();
			auto y1 = pt1.getY();
			auto y2 = pt2.getY();

			auto prod = x1*y2 - x2*y1;
			cx += (x1+x2) * prod;
			cy += (y1+y2) * prod;
		}
		auto signedArea = p_ComputeSignedArea();
		cx /= (6. * signedArea);
		cy /= (6. * signedArea);

		auto c = Point2d_<HOMOG2D_INUMTYPE>( cx, cy );
		_attribs._centroid.set( c );
	}
	return _attribs._centroid.value();
}

//------------------------------------------------------------------
/// Normalization of CPolyline_
/**
Two tasks:
- rotating so that the smallest one is first
- for closed polylines: constant orientation

\note: is \c const, although it moves the points, but all the attributes are the same
(done by declaring the vector of points as \c mutable).

Reference for the orientation:
- http://www.faqs.org/faqs/graphics/algorithms-faq/
Subject 2.07: How do I find the orientation of a simple polygon?
- https://en.wikipedia.org/wiki/Curve_orientation#Orientation_of_a_simple_polygon
*/
template<typename PLT,typename FPT>
void
base::PolylineBase<PLT,FPT>::p_normalizePoly() const
{
	if( size() == 0 )
		return;

	if( !_plIsNormalized )
	{
		if constexpr ( std::is_same_v<PLT,typ::IsClosed> )
		{
			auto minpos = std::min_element( _plinevec.begin(), _plinevec.end() );
			std::rotate( _plinevec.begin(), minpos, _plinevec.end() );

			auto ptB = _plinevec[0];
			auto ptA = _plinevec[1];
			auto ptC = _plinevec[size()-1];

			HOMOG2D_INUMTYPE xA = ptA.getX();
			HOMOG2D_INUMTYPE xB = ptB.getX();
			HOMOG2D_INUMTYPE xC = ptC.getX();
			HOMOG2D_INUMTYPE yA = ptA.getY();
			HOMOG2D_INUMTYPE yB = ptB.getY();
			HOMOG2D_INUMTYPE yC = ptC.getY();

			auto det = (xB-xA)*(yC-yA) - (xC-xA)*(yB-yA);
			if( det < 0)
			{
				std::reverse( _plinevec.begin(), _plinevec.end() );
				minpos = std::min_element( _plinevec.begin(), _plinevec.end() );
				std::rotate( _plinevec.begin(), minpos, _plinevec.end() );
			}
		}
		else
		{
			if( _plinevec.back() < _plinevec.front() )
				std::reverse( _plinevec.begin(), _plinevec.end() );
		}
		_plIsNormalized=true;
	}
}

} // namespace base

//------------------------------------------------------------------
/// Rotates the rectangle by either 90°, 180°, 270° (-90°) at point \c refpt
/**
For an arbitrary angle \c alpha (rad.), you can write:
```
auto r2 = Homogr(alpha) * rect;
```
\sa FRect_<FPT>::rotate( Rotate )
\sa h2d::rotate()
*/
template<typename FPT>
template<typename FPT2>
void
FRect_<FPT>::rotate( Rotate rot, const Point2d_<FPT2>& refpt )
{
	translate( -refpt.getX(), -refpt.getY() );
	this->rotate( rot );
	translate( refpt.getX(), refpt.getY() );
}

//------------------------------------------------------------------
/// Rotates the rectangle by either 90°, 180°, 270° (-90°) at point (0,0)
/**
For an arbitrary angle \c alpha (rad.), you can write:
```
auto r2 = Homogr(alpha) * rect;
```
\sa FRect_<FPT>::rotate( Rotate, const Point2d_<FPT2>& )
\sa h2d::rotate()

\note This function converts the FRect to a CPolyline_, rotates it, and builds the output rectangle
by getting the bounding box.
This is because we cannot directly rotate the points, because the two points of a FRect_ are switched so that
the smallest point is always in _ptR1.
*/
template<typename FPT>
void
FRect_<FPT>::rotate( Rotate rot )
{
	CPolyline_<HOMOG2D_INUMTYPE> pol( *this );
	pol.rotate( rot );
	*this = pol.getBB();
}

//------------------------------------------------------------------
/// Returns Rectangle of the intersection area of two rectangles
/**
\return An object of type detail::RectArea, that can be checked for success using the ()
operator, and if success, will hold the resulting FRect_

Algorithm:

4 situations need to be considered, depending on the number of intersection points:

- A: 2 intersection points on same segment => we have 2 points inside.
\verbatim
  +------+                   +------+
  |      |                   |      |
  |   +--+-----+          +--+----+ |
  |   |  |     |          |  |    | |
  |   |  |     |    or    |  |    | |  or ...
  |   +--+-----+          +--+----+ |
  |      |                   |      |
  +------+                   +------+
\endverbatim

- B: 2 intersection points on different segments => for each rectangle, 1 point is inside the other.
\verbatim
  +------+
  |      |
  |      |
  |   +--+-----+
  |   |  |     |   or ...
  +---+--+     |
      |        |
      +--------+
\endverbatim

- C: 3 points
\verbatim
  +------+-----+
  |      |     |
  |      |     |
  +------+-----+
  |      |
  +------+
\endverbatim

- D: 4 intersection points: the intersection rectangle is made of these 4 points
\verbatim
     +------+
     |      |
  +--+------+-----+
  |  |      |     |
  |  |      |     |
  +--+------+-----+
     |      |
     +------+
\endverbatim
*/
template<typename FPT>
template<typename FPT2>
detail::RectArea<FPT>
FRect_<FPT>::intersectArea( const FRect_<FPT2>& other ) const
{
	if( *this == other )                      // if same rectangles, then
		return detail::RectArea<FPT>(other);  // the intersection area is the rectangle

	auto inter = this->intersects( other );

	if( !inter() )                        // rectangles do not intersect
	{
		if( this->isInside( other ) )
			return detail::RectArea<FPT>(*this);
		if( other.isInside( *this ) )
			return detail::RectArea<FPT>(other);
		return detail::RectArea<FPT>();
	}

	if( inter.size() < 2 )                // only one intersection point
		return detail::RectArea<FPT>();   // => no intersection area!

	if( inter.size() == 4 )  // 4 intersection points => case "D"
		return detail::RectArea<FPT>(FRect_<FPT>( inter.get().at(0), inter.get().at(3) ) );

	if( inter.size() == 3 )  // 3 intersection points => case "C"
	{
		auto v = inter.get();

		auto xmin = std::min( v[0].getX(), std::min( v[1].getX(),v[2].getX() ) );
		auto ymin = std::min( v[0].getY(), std::min( v[1].getY(),v[2].getY() ) );
		auto xmax = std::max( v[0].getX(), std::max( v[1].getX(),v[2].getX() ) );
		auto ymax = std::max( v[0].getY(), std::max( v[1].getY(),v[2].getY() ) );
#ifndef HOMOG2D_DEBUGMODE
		assert( xmax-xmin > thr::nullOrthogDistance() );
		assert( ymax-ymin > thr::nullOrthogDistance() );
#else
		HOMOG2D_DEBUG_ASSERT(
			( ( xmax-xmin > thr::nullOrthogDistance() )
			&&
			( ymax-ymin > thr::nullOrthogDistance() )) ,
			std::scientific
			<< "this=" << *this << " other=" << other
			<< "\nxmax=" << xmax << " xmin=" << xmin
			<< "\nymax=" << ymax << " ymin=" << ymin
			<< "\nnod=" << thr::nullOrthogDistance()
		);
#endif
		return detail::RectArea<FPT>( FRect_<FPT>( xmin, ymin, xmax,ymax ) );
	}
/*------------------------
 If we get to this point, we have 2 intersection points
------------------------*/
#ifndef HOMOG2D_DEBUGMODE
	assert( inter.size() == 2 );
#else
/*	HOMOG2D_DEBUG_ASSERT(
		inter.size() == 2,
		"inter.size()=" << inter.size() << "\n-this=" << *this << "\n-other=" << other
		<<"\n-inter:" << inter
	);*/
#endif
	const auto& r1 = *this;
	const auto& r2 = other;
//	HOMOG2D_LOG( "r1="<<r1 << " r2="<<r2 );
	auto v1 = r1.p_pointsInside( r2 );
	auto v2 = r2.p_pointsInside( r1 );
	auto c1 = v1.size();
	auto c2 = v2.size();
//	HOMOG2D_LOG( "c1=" << c1 << " c2=" << c2 );

	if( c1==0 && c2==0 ) // unable, rectangles share a segment
		return detail::RectArea<FPT>();

	assert(
		( c1==1 && c2==1 )
		||
		( c1==0 && c2==2 )
		||
		( c2==0 && c1==2 )
	);
	if( c1==1 || c2==1 ) // only 1 point inside => the rectangle is defined by the intersection points
		return detail::RectArea<FPT>( FRect_<FPT>( inter.get().at(0), inter.get().at(1) ) );

// here: 2 points inside, then build rectangle using the 4 points:
// the 2 inside, and the two intersection points
	assert( c1 == 2 || c2 == 2 );

	if( c1 == 2 )
		return detail::RectArea<FPT>(
			FRect_<FPT>(
				inter.get().at(0),
				inter.get().at(1),
				v1.at(0),
				v1.at(1)
			)
		);
	return detail::RectArea<FPT>(
		FRect_<FPT>(
			inter.get().at(0),
			inter.get().at(1),
			v2.at(0),
			v2.at(1)
		)
	);
}

namespace priv {
/// Common stuff for FRect_ union, see FRect_::unionArea()
namespace runion {
//------------------------------------------------------------------
/// Holds x or y coordinate value of rectangle during computation of intersection area
/**
- \c T will be the floating-point type
- used in FRect_<FPT>::unionArea()
*/
template<typename T>
struct Index
{
	T       _value;
	uint8_t _rect_idx=0;   // 0 means none, will be 1 or 2

	Index() = default;
	Index( T v, uint8_t r )
		: _value(v), _rect_idx(r)
	{}
	friend bool operator < ( const Index& i1, const Index& i2 )
	{

		if( i1._value == i2._value )
			return i1._rect_idx < i2._rect_idx;
		return i1._value < i2._value;
	}
	friend std::ostream& operator << ( std::ostream& f, const Index& idx )
	{
		f << idx._value << " ";
		return f;
	}
};

/// A wrapper around a bool, that gets set if that point defined
/// by indexes \c ix and \c iy is a corner in the table
/**
used in FRect_<FPT>::unionArea()
*/
struct Cell
{
	bool _isCorner = false;
	Cell() = default;
#ifdef HOMOG2D_DEBUGMODE
	Cell( bool b ) : _isCorner(b)
	{}
#endif
	template<typename T>
	Cell( const Index<T>& ix, const Index<T>& iy )
	{
		if( ix._rect_idx == iy._rect_idx )
			_isCorner = true;
	}
};


using Table  = std::array<std::array<Cell,4>,4>;
using PCoord = std::pair<uint8_t,uint8_t>;

enum class Direction: uint8_t { N, E, S, W };

/* NEEDED TO DEBUG
const char* getString( Direction dir )
{
	switch( dir )
	{
		case Direction::W: return "W"; break;
		case Direction::S: return "S"; break;
		case Direction::E: return "E"; break;
		case Direction::N: return "N"; break;
	}
	return 0; // to avoid a warning
}*/

enum class Turn: uint8_t { Left, Right };

inline
Direction
turn( Direction dir, Turn turn )
{
	switch( dir )
	{
		case Direction::W:
			return turn == Turn::Left ? Direction::S : Direction::N;
		case Direction::S:
			return turn == Turn::Left ? Direction::E : Direction::W;
		case Direction::E:
			return turn == Turn::Left ? Direction::N : Direction::S;
		case Direction::N:
			return turn == Turn::Left ? Direction::W : Direction::E;
	}
	return Direction::N; // to avoid a warning
}

inline
void
moveToNextCell( uint8_t& row, uint8_t& col, const Direction& dir )
{
	switch( dir )
	{
		case Direction::N: row--; break;
		case Direction::S: row++; break;
		case Direction::E: col++; break;
		case Direction::W: col--; break;
	}
}

/// Helper function for FRect_<FPT>::unionArea()
/**
- Start from 0,0, direction East
*/
inline
std::vector<PCoord>
parseTable( Table& table )
{
	bool firstTime = true;
	bool done = false;
	uint8_t row = 0;
	uint8_t col = 0;
	Direction dir = Direction::E;
	std::vector<PCoord> out;
	do
	{
		if( table[row][col]._isCorner )
		{
			auto new_pair = std::make_pair(row,col);
			if( out.size() > 0 )
				if( new_pair == out.front() && out.size() > 2 )
					done = true;
			if( !done )
				out.push_back( new_pair );
			if( firstTime )
				firstTime = false;
			else
				dir = turn( dir, Turn::Right );
		}
		else
		{
			if( ( row==2 || row==1 ) && ( col==2 || col==1 ) )
			{
				out.push_back( std::make_pair(row,col) );
				dir = turn( dir, Turn::Left );
			}
		}
		moveToNextCell( row, col, dir );
	}
	while( !done );
	return out;
}

#ifdef HOMOG2D_DEBUGMODE
inline
void
printTable( const Table& t, std::string msg )
{
	std::cout << "Table: " << msg << "\n  | ";
	for( uint8_t r=0;r<4; r++ )
		std::cout << (int)r << " ";
	std::cout << "\n--|---------|\n";
	for( uint8_t r=0;r<4; r++ )
	{
		std::cout << (int)r << " | ";
		for( uint8_t c=0;c<4; c++ )
			std::cout << (t[r][c]._isCorner?'F':'.') << " ";
		std::cout << "|\n";
	}
}
#endif

//------------------------------------------------------------------
/// Helper function, used in FRect_<FPT>::unionArea()
template<typename FPT>
inline
CPolyline_<FPT>
convertToCoord(
	const std::vector<PCoord>&      v_coord, ///< vector of coordinate indexes
	const std::array<Index<FPT>,4>& v_x,      ///< holds x-coordinates
	const std::array<Index<FPT>,4>& v_y       ///< holds y-coordinates
)
{
	std::vector<Point2d_<FPT>> v_pts;
	for( const auto& elem: v_coord )
	{
		auto id_x = elem.first;
		auto id_y = elem.second;
		assert( id_x<4 && id_y<4 );

		auto pt = Point2d_<FPT>( v_x[id_x]._value, v_y[id_y]._value );
		if( v_pts.empty() )
			v_pts.push_back( pt );
		else                             // add to vector only  if not same as previous
		{                                // and not same as first
			if( v_pts.back() != pt && v_pts.front() != pt )
				v_pts.push_back( pt );
		}
	}
//	printVector( v_pts, "polyline" );
	return CPolyline_<FPT>( v_pts );//, IsClosed::Yes );
}

} // namespace priv
} // namespace runion
//------------------------------------------------------------------
/// Computes the CPolyline of the union of two rectangles
/**
Algorithm:
 -# build vectors of x and y coordinates (4 elements)
 -# build table x-y (4x4), with corners tagged
 -# parse the table by turning right at each corner, and left if position is not one the outside row/col
 -# convert back indexes to real coordinates, to build the final CPolyline object

Two examples:
\verbatim
9      +----+              +-------+
       |    |              |       |
8  +---+----+---+      +---+---+   |
   |   |    |   |      |   |   |   |
7  +---+----+---+      +---+---+   |
       |    |              |       |
6      +----+              +-------+
   1   2    3   4      1   2   3   4
\endverbatim
Step 1 will build (for both situations above):
 - vx: 1,2,3,4
 - vy: 6,7,8,9

 Step 2 will build a table showing where the corners are:
\verbatim
  | 0 1 2 3             | 0 1 2 3
--|---------|         --|---------|
0 | . F F . |          0| . F . F
1 | F F F F |          1| F F . .
2 | F F F F |          2| F F . .
3 | . F F . |          3| . F . F
\endverbatim

Step 3: parse that table and turn on each corner:
- left if "inner" corner (row and col = 1 or = 2)
- right if "outer" corner (row and col = 0 or = 3)

Final step: convert indexes to real coordinates

Special note: if the rectangles have an identical coordinate, as in this example:
\verbatim
9      +----+
       |    |
8  +---+----+
   |   |    |
7  +---+----+
       |    |
6      +----+
   1   2    3
\endverbatim
Then the vectors are:
- vx: 1,2,3,3
- vy: 6,7,8,9

(notice the duped coordinate)

This will produce a Polyline_ with 2 extra points:<br>
(1,7)-(1,8)-(2,8)-(2,9)-(3,9)-(3,8)-(3,7)-(3,6)-(2,6)-(2,7)
<br>instead of:<br>
(1,7)-(1,8)-(2,8)-(2,9)-(3,9)-(3,6)-(2,6)-(2,7)
<br>We solve this by proceeding an extra Polyline minimization,
see PolylineBase::minimize()
*/
template<typename FPT>
template<typename FPT2>
CPolyline_<FPT>
FRect_<FPT>::unionArea( const FRect_<FPT2>& other ) const
{
	using namespace priv::runion;

	if( *this == other )                  // if same rectangles, then
		return CPolyline_<FPT>( other );   // returns one of them

	if( !this->intersects(other)() )  // if no intersection,
	{
		if( this->isInside( other ) )
			return CPolyline_<FPT>(other);
		if( other.isInside( *this ) )
			return CPolyline_<FPT>(*this);
		return CPolyline_<FPT>();      // return empty polygon
	}

/* step 0: make sure the rect with highest x is first.
 This is needed to avoid this kind of situation in table:

  F F . .
  . . F F
  . . F F
  F F . .

  That would happen if we have an identical x in the two rect
  (thus, they DO intersect), because when sorting, the rect with smallest index
  (1 or 2) is placed first in the vector of coordinates */
	const auto* pr1 = this;
	const auto* pr2 = &other;
	if( pr1->getPts().first.getX() < pr2->getPts().first.getX() )
		std::swap( pr1, pr2 );
	const auto& r1 = *pr1;
	const auto& r2 = *pr2;

// step 1: build vectors of coordinates and sort them
	std::array<Index<FPT>,4> vx, vy;
	int i=0;
	vx[i++] = Index<FPT>( r1.getPts().first.getX(),  1 );
	vx[i++] = Index<FPT>( r1.getPts().second.getX(), 1 );
	vx[i++] = Index<FPT>( r2.getPts().first.getX(),  2 );
	vx[i++] = Index<FPT>( r2.getPts().second.getX(), 2 );

	i=0;
	vy[i++] = Index<FPT>( r1.getPts().first.getY(),  1 );
	vy[i++] = Index<FPT>( r1.getPts().second.getY(), 1 );
	vy[i++] = Index<FPT>( r2.getPts().first.getY(),  2 );
	vy[i++] = Index<FPT>( r2.getPts().second.getY(), 2 );

	std::sort( vx.begin(), vx.end() );
	std::sort( vy.begin(), vy.end() );
//	priv::printArray( vx, "vx" ); priv::printArray( vy, "vy");

// step 2: fill table\n";
	Table table;
	for( uint8_t r=0;r<4; r++ )
		for( uint8_t c=0;c<4; c++ )
			table[r][c] = Cell( vx[r], vy[c] );
//	printTable( table, "after step 2" );

// step 3: parse table
	auto vpts = parseTable( table );
//	priv::printVectorPairs( vpts );

// step 4: convert back vector of coordinates indexes into vector of coordinates
	auto res1 = convertToCoord( vpts, vx, vy );
	res1.minimize(); // remove unecessary points
	return res1;
}

//------------------------------------------------------------------
namespace detail {

//------------------------------------------------------------------
/// See getPtLabel( const Point2d_<FPT>& pt, const Circle_<FPT2>& circle )
enum class PtTag: uint8_t
{
	Inside, Outside, OnEdge
};

/// Returns a label characterizing point \c pt, related to \c circle: inside, outside, or on edge of circle
template<typename FPT,typename FPT2>
PtTag
getPtLabel( const Point2d_<FPT>& pt, const Circle_<FPT2>& circle )
{
	if( pt.isInside( circle ) )
		return PtTag::Inside;
	if(
		homog2d_abs( pt.distTo( circle.center() ) - circle.radius() )
			< thr::nullDistance()
	)
		return PtTag::OnEdge;
	return PtTag::Outside;
}
#if 0
/// Debug, can be removed after
void
printTag( std::string txt, PtTag tag )
{
	std::cout << "point " << txt << ": ";
	switch( tag )
	{
		case PtTag::Inside: std::cout << "Inside\n"; break;
		case PtTag::Outside: std::cout << "Outside\n"; break;
		case PtTag::OnEdge: std::cout << "OnEdge\n"; break;
	}
}
#endif
} // namespace detail

namespace base {
//------------------------------------------------------------------
/// Segment/Segment intersection
/**
Algorithm:<br>
- first compute the intersection point
- then check if the intersection point lies in between the range of both segments, both on x and on y
*/
template<typename SV,typename FPT>
template<typename SV2,typename FPT2>
detail::Intersect<detail::Inters_1,FPT>
SegVec<SV,FPT>::intersects( const SegVec<SV2,FPT2>& s2 ) const
{
//	HOMOG2D_START;
	if( *this == s2 )              // same segment => no intersection
		return detail::Intersect<detail::Inters_1,FPT>();

	Line2d_<HOMOG2D_INUMTYPE> l1 = getLine();
	Line2d_<HOMOG2D_INUMTYPE> l2 = s2.getLine();
	if( l1.isParallelTo( l2 ) )                            // if parallel,
		return detail::Intersect<detail::Inters_1,FPT>();  // then, no intersection

	const auto& ptA1 = getPts().first;
	const auto& ptA2 = getPts().second;
	const auto& ptB1 = s2.getPts().first;
	const auto& ptB2 = s2.getPts().second;

	auto ptInter = l1 * l2;   // intersection point

	auto dA1 = ptA1.distTo( ptInter );
	auto dA2 = ptA2.distTo( ptInter );
	if( homog2d_abs(dA1+dA2 - length()) < thr::nullDistance() )
	{
		auto dB1 = ptB1.distTo( ptInter );
		auto dB2 = ptB2.distTo( ptInter );
		if( homog2d_abs(dB1+dB2 - s2.length()) < thr::nullDistance() )
			return detail::Intersect<detail::Inters_1,FPT>( ptInter );
	}
	return detail::Intersect<detail::Inters_1,FPT>(); // no intersection
}

//------------------------------------------------------------------
/// Segment/Line intersection
/**
Algorithm:<br>
We check if the intersection point lies in between the range of the segment, both on x and on y
*/
template<typename SV,typename FPT>
template<typename FPT2>
detail::Intersect<detail::Inters_1,FPT>
SegVec<SV,FPT>::intersects( const Line2d_<FPT2>& li1 ) const
{
//	HOMOG2D_START;
//	HOMOG2D_LOG( "seg=" << *this << " line=" << li1 );
	detail::Intersect<detail::Inters_1,FPT> out;
	auto li2 = getLine();

	if( li1.isParallelTo( li2 ) ) // if parallel, no intersection
		return out;

	out._ptIntersect = li1 * li2;   // intersection point

	const auto& pi  = out._ptIntersect;
	const auto& pt1 = getPts().first;
	const auto& pt2 = getPts().second;

	auto d1 = pt1.distTo( pi );
	auto d2 = pt2.distTo( pi );
	if( homog2d_abs(d1+d2-length()) < thr::nullDistance() )
 		out._doesIntersect = true;

	return out;
}

//------------------------------------------------------------------
/// Segment/Circle intersection
/**
For each point of the segment, we need to consider 3 different situations
 - point is inside (PI)
 - point is outside (PO)
 - point is on the edge (PE)

That makes 6 different situations to handle:

 - S1: PI-PI => no intersection
 - S2: PI-PO => 1 intersection
 - S3: PI-PE => 1 intersection
 - S4: PO-PO => depends on the support line:
  - S4A: if line does NOT intersects circle, no intersection pts
  - S4B: if line does intersects circle, and intersections point in segment area => 2 intersection pts
  - S4C: if line does intersects circle, and intersections point NOT in segment area => no intersection pts
 - S5: PO-PE => 1 intersection
 - S6: PE-PE => 2 intersections
*/
template<typename SV,typename FPT>
template<typename FPT2>
detail::IntersectM<FPT>
SegVec<SV,FPT>::intersects( const Circle_<FPT2>& circle ) const
{
//	HOMOG2D_START;
	using detail::PtTag;

	auto tag_ptS1 = detail::getPtLabel( _ptS1, circle ); // get status of segment points related to circle (inside/outside/on-edge)
	auto tag_ptS2 = detail::getPtLabel( _ptS2, circle );

	if( tag_ptS1 == PtTag::Inside )
		if( tag_ptS2 == PtTag::Inside )
			return detail::IntersectM<FPT>();

	auto int_lc = getLine().intersects( circle );
	if( !int_lc() )
		return detail::IntersectM<FPT>();

	auto p_pts = int_lc.get();      // get the line intersection points
	const auto& p1 = p_pts.first;
	const auto& p2 = p_pts.second;

	if(                                                            // one inside, the other outside
		( tag_ptS1 == PtTag::Inside  && tag_ptS2 == PtTag::Outside )
		||
		( tag_ptS1 == PtTag::Outside && tag_ptS2 == PtTag::Inside )
	)
	{
		detail::IntersectM<FPT> out;

		auto d1 = _ptS1.distTo( p1 );
		auto d2 = _ptS2.distTo( p1 );
		if( homog2d_abs( d1+d2-length() ) < thr::nullDistance() )
			out.add( p1 );                          // points is inside
		else
			out.add( p2 );
		return out;
	}

	detail::IntersectM<FPT> out;
	if( tag_ptS1 == PtTag::Outside && tag_ptS2 == PtTag::Outside ) // both outside
	{
		auto d1 = _ptS1.distTo( p1 );
		auto d2 = _ptS2.distTo( p1 );
		if( homog2d_abs( d1+d2-length() ) >= thr::nullDistance() )   // if sum of the two distances is higher than length
			return detail::IntersectM<FPT>();

		out.add( p1 );
		out.add( p2 );
		return out;
	}

	if( tag_ptS1 == PtTag::OnEdge )
		out.add( _ptS1 );
	if( tag_ptS2 == PtTag::OnEdge )
		out.add( _ptS2 );

	return out;
}

} // namespace base

/////////////////////////////////////////////////////////////////////////////
// SECTION  - STREAMING OPERATORS
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
namespace base {

/// Stream operator, free function, call member function pseudo operator impl_op_stream()
template<typename LP,typename FPT>
std::ostream&
operator << ( std::ostream& f, const h2d::base::LPBase<LP,FPT>& pl )
{
	if constexpr( std::is_same_v<LP,typ::IsLine> )
		f << '[' << pl._v[0] << ',' << pl._v[1] << ',' << pl._v[2] << "]";
	else
	{
		if( pl.isInf() )
			f << '[' << pl._v[0] << ',' << pl._v[1] << ',' << pl._v[2] << "]";
		else
			f
		//	<< std::scientific << std::setprecision(25)
				<< '[' << pl.getX() << ',' << pl.getY() << "]";
	}

	return f;
}

} // namespace base

/// Stream operator for a container of points/lines, free function
template<
	typename T,
	typename std::enable_if<
		trait::IsContainer<T>::value,
		T
	>::type* = nullptr
>
std::ostream&
operator << ( std::ostream& f, const T& vec )
{
	f << "#="<< vec.size() << '\n';
	for( const auto& elem: vec )
		f << elem << '\n';
	return f;
}

/// Stream operator for a pair of points/lines, free function
template<typename LP1,typename LP2,typename FPT>
std::ostream&
operator << ( std::ostream& f, const std::pair<base::LPBase<LP1,FPT>,base::LPBase<LP2,FPT>>& pr )
{
	f << "std::pair (" << getString(pr.first.type()) << "-" << getString(pr.second.type())
		<< "):\n -first="  << pr.first
		<< "\n -second=" << pr.second
		<< ' ';
	return f;
}

template<typename T>
std::ostream&
operator << ( std::ostream& f, const FRect_<T>& r )
{
	f << "pt1: " << r._ptR1 << " pt2: " << r._ptR2;
	return f;
}

template<typename T>
std::ostream&
operator << ( std::ostream& f, const Circle_<T>& r )
{
	f << "center: " << r._center << ", radius=" << r._radius;
	return f;
}

namespace base {

template<typename SV,typename T>
std::ostream&
operator << ( std::ostream& f, const h2d::base::SegVec<SV,T>& seg )
{
	f << seg._ptS1;
	if constexpr( std::is_same_v<SV,typ::IsOSeg> )
		f << "=>";
	else
		f << "-";
	f << seg._ptS2;
	return f;
}

template<typename PLT,typename FPT>
std::ostream&
operator << ( std::ostream& f, const h2d::base::PolylineBase<PLT,FPT>& pl )
{
	f << (pl.isClosed() ? 'C' : 'O' ) << "Polyline: ";
	if( !pl.size() )
		f << "empty";
	else
	{
		for( const auto& pt: pl._plinevec )
			f << pt << "-";
	}
	return f;
}
} // namespace base

template<typename T>
std::ostream&
operator << ( std::ostream& f, const Ellipse_<T>& ell )
{
	auto par = ell.template p_getParams<HOMOG2D_INUMTYPE>();
	f << par;
	return f;
}

//------------------------------------------------------------------
/// Returns the 2 parallel lines at distance \c dist from \c li (free function)
template<typename FPT,typename T>
std::pair<Line2d_<FPT>,Line2d_<FPT>>
getParallelLines( const Line2d_<FPT>& li, T dist )
{
	return li.getParallelLines( dist );
}

/////////////////////////////////////////////////////////////////////////////
// SECTION  - MEMBER FUNCTION IMPLEMENTATION: CLASS Ellipse_
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
/// Returns standard parameters from matrix coeffs
template<typename FPT>
template<typename T>
detail::EllParams<T>
Ellipse_<FPT>::p_getParams() const
{
#ifdef HOMOG2D_OPTIMIZE_SPEED
	if( _epHasChanged )
	{
		_par = p_computeParams<T>();
		_epHasChanged = false;
	}
	return _par;
#else
	return p_computeParams<T>();
#endif // HOMOG2D_OPTIMIZE_SPEED
}
//------------------------------------------------------------------
/// Compute and returns standard parameters from matrix coeffs
/**
\note In the checking below, we **cannot** print the matrix using the
<< operator, because it call this function, thus it would enter
an infinite loop (an eventually SO).
*/
template<typename FPT>
template<typename T>
detail::EllParams<T>
Ellipse_<FPT>::p_computeParams() const
{
	const auto& m = detail::Matrix_<FPT>::_mdata;
	HOMOG2D_INUMTYPE A = m[0][0];
	HOMOG2D_INUMTYPE C = m[1][1];
	HOMOG2D_INUMTYPE F = m[2][2];
	HOMOG2D_INUMTYPE B = 2. * m[0][1];
	HOMOG2D_INUMTYPE D = 2. * m[0][2];
	HOMOG2D_INUMTYPE E = 2. * m[1][2];

	detail::EllParams<T> par;  // theta already set to zero

	auto denom = B*B - (HOMOG2D_INUMTYPE)4. * A * C;

#ifndef HOMOG2D_NOCHECKS
	if( homog2d_abs(denom) < thr::nullDenom() )
		HOMOG2D_THROW_ERROR_1(
			"unable to compute parameters, denom=" << std::scientific << std::setprecision(15) << denom
		);
#endif

	par.x0 = ( (HOMOG2D_INUMTYPE)2.*C*D - B*E ) / denom;
	par.y0 = ( (HOMOG2D_INUMTYPE)2.*A*E - B*D ) / denom;
	auto common_ab = (HOMOG2D_INUMTYPE)2. * ( A*E*E + C*D*D - B*D*E + denom*F );
	auto AmC = A-C;
	auto AmC2 = AmC*AmC;

	auto sqr = homog2d_sqrt(AmC2+B*B);
	par.a = -homog2d_sqrt( common_ab * ( A+C+sqr ) )/ denom;
	par.b = -homog2d_sqrt( common_ab * ( A+C-sqr ) )/ denom;


	par.a2 = par.a * par.a;
	par.b2 = par.b * par.b;
	if( homog2d_abs(B) < thr::nullDenom() )
	{
		if( A > C )
			par.theta = 90.;
	}
	else
	{
		auto t = (C - A - sqr) / B;
		par.theta = homog2d_atan( t );
	}
	par.sint = homog2d_sin( par.theta );
	par.cost = homog2d_cos( par.theta );
	return par;
}

//------------------------------------------------------------------
/// Returns true if ellipse is a circle
/**
Using the matrix representation, if A = C and B = 0, then the ellipse is a circle

You can provide the 0 threshold as and argument
*/
template<typename FPT>
bool
Ellipse_<FPT>::isCircle( HOMOG2D_INUMTYPE thres ) const
{
	const auto& m = detail::Matrix_<FPT>::_mdata;
	HOMOG2D_INUMTYPE A  = m[0][0];
	HOMOG2D_INUMTYPE C  = m[1][1];
	HOMOG2D_INUMTYPE B2 = m[0][1];
	if( homog2d_abs(A-C) < thres )
		if( homog2d_abs(B2)*2. < thres )
			return true;
	return false;
}

//------------------------------------------------------------------
/// Returns center of ellipse
/// \sa center( const T& )
template<typename FPT>
Point2d_<FPT>
Ellipse_<FPT>::getCenter() const
{
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	return Point2d_<FPT>( par.x0, par.y0 );
}

//------------------------------------------------------------------
template<typename FPT>
std::pair<HOMOG2D_INUMTYPE,HOMOG2D_INUMTYPE>
Ellipse_<FPT>::getMajMin() const
{
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	return std::make_pair( par.a, par.b );
}

//------------------------------------------------------------------
/// Returns angle of ellipse
/// \sa angle( const Ellipse_& )
template<typename FPT>
HOMOG2D_INUMTYPE
Ellipse_<FPT>::angle() const
{
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	return par.theta;
}

//------------------------------------------------------------------
/// Returns (approximate) perimeter of ellipse using the Ramanujan second formulae
/**
See https://en.wikipedia.org/wiki/Ellipse#Circumference
*/
template<typename FPT>
HOMOG2D_INUMTYPE
Ellipse_<FPT>::length() const
{
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	auto ab_sum  = par.a + par.b;
	auto ab_diff = par.a - par.b;
	auto h = ab_diff * ab_diff / (ab_sum * ab_sum);
	auto denom = (HOMOG2D_INUMTYPE)10. + homog2d_sqrt(4. - 3. * h);
	return (par.a + par.b) * M_PI * ( 1. + 3. * h / denom );
}

//------------------------------------------------------------------
/// Returns pair of axis lines of ellipse
template<typename FPT>
std::pair<Line2d_<FPT>,Line2d_<FPT>>
Ellipse_<FPT>::getAxisLines() const
{
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	auto dy = static_cast<HOMOG2D_INUMTYPE>(par.sint) * par.a;
	auto dx = static_cast<HOMOG2D_INUMTYPE>(par.cost) * par.a;
	Point2d_<HOMOG2D_INUMTYPE> ptA(
		par.x0 + dx,
		par.y0 + dy
	);
	auto pt0 = Point2d_<HOMOG2D_INUMTYPE>( par.x0, par.y0 );
	auto li_H = ptA * pt0;
	HOMOG2D_LOG( std::scientific << "ptA=" << ptA << " pt0=" << pt0 << " li_H=" << li_H );

	auto li_V = li_H.getOrthogLine( pt0 );
	return std::make_pair( li_H, li_V );
}

//------------------------------------------------------------------
/// Returns axis-aligned bounding box of ellipse
/**
- see https://math.stackexchange.com/questions/91132/how-to-get-the-limits-of-rotated-ellipse
*/
template<typename FPT>
auto
Ellipse_<FPT>::getBB() const
{
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	auto vx = par.a2 * par.cost * par.cost	+ par.b2 * par.sint * par.sint;
	auto vy = par.a2 * par.sint * par.sint	+ par.b2 * par.cost * par.cost;
	auto vx_sq = homog2d_sqrt( vx );
	auto vy_sq = homog2d_sqrt( vy );
	return FRect_<HOMOG2D_INUMTYPE>(
		Point2d_<HOMOG2D_INUMTYPE>( par.x0 - vx_sq, par.y0 - vy_sq ),
		Point2d_<HOMOG2D_INUMTYPE>( par.x0 + vx_sq, par.y0 + vy_sq )
	);
}

//------------------------------------------------------------------
/// Returns oriented bounding box of ellipse as a closed Polyline
/**
Algorithm:
 - build line \c liH going through major axis, by using center point and
 point on semi-major axis, intersecting ellipse
 - get opposite point \c ptB, lying on line and at distance \c a
 - get the two parallel lines to \c liH, at a distance \c b
 - get the two orthogonal lines at \c ptA and \c ptB

\todo 20240330: unclear, the text above does not match what is done below (or does it?).
Clarify that, and build a gif showing how this is done.
*/
template<typename FPT>
CPolyline_<FPT>
Ellipse_<FPT>::getOBB() const
{
// step 1: build ptA using angle
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	auto dy = par.sint * par.a;
	auto dx = par.cost * par.a;
	Point2d_<FPT> ptA(
		par.x0 + dx,
		par.y0 + dy
	);
	auto pt0 = Point2d_<HOMOG2D_INUMTYPE>( par.x0, par.y0 );

// step 2: build main-axis line, going through center and ptA
	auto li_H = ptA * pt0;

// step 3: get ptB, using line and distance
	auto ppts = li_H.getPoints( pt0, par.a );
	auto ptB = ppts.first;
	if( ptB == ptA )
		ptB = ppts.second;

	auto para = getParallelLines( li_H, par.b );
	auto li_V1 = li_H.getOrthogLine( ptA );
	auto li_V2 = li_H.getOrthogLine( ptB );

	CPolyline_<FPT> out;
#ifndef	HOMOG2D_DEBUGMODE
	out.p_addPoint( para.first  * li_V1 );
	out.p_addPoint( para.second * li_V1 );
	out.p_addPoint( para.second * li_V2 );
	out.p_addPoint( para.first  * li_V2 );
#else
	auto p1 = para.first  * li_V1;
	auto p2 = para.second * li_V1;
	auto p3 = para.second * li_V2;
	auto p4 = para.first  * li_V2;
	HOMOG2D_DEBUG_ASSERT(
		( p2!=p1 && p3!=p2 && p4!=p3 ),
		"p1=" << p1 << " p2=" << p2 << " p3=" << p3 << " p4=" << p4
		<< "\n para.1=" << para.first
		<< "\n li_V1=" << li_V1
		<< "\n ptA=" << ptA
		<< "\n ptB=" << ptB
		<< "\n " << ppts
	);
	out.p_addPoint( p1 );
	out.p_addPoint( p2 );
	out.p_addPoint( p3 );
	out.p_addPoint( p4 );
#endif
	return out;
}

//------------------------------------------------------------------
/// Returns true if point is inside ellipse
/**
taken from https://stackoverflow.com/a/16814494/193789
*/
template<typename FPT>
template<typename FPT2>
bool
Ellipse_<FPT>::pointIsInside( const Point2d_<FPT2>& pt ) const
{
	HOMOG2D_INUMTYPE x = pt.getX();
	HOMOG2D_INUMTYPE y = pt.getY();

	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	const auto& x0 = par.x0;
	const auto& y0 = par.y0;

	auto v1 = par.cost * (x-x0) + par.sint * (y-y0);
	HOMOG2D_INUMTYPE sum = v1*v1 / par.a2;

	auto v2 = par.sint * (x-x0) - par.cost * (y-y0);
	sum += v2*v2 / par.b2;
	if( sum < 1. )
		return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// SECTION  - MEMBER FUNCTION IMPLEMENTATION: CLASS LPBase
/////////////////////////////////////////////////////////////////////////////

namespace base {
//------------------------------------------------------------------
/// Normalize to unit length, and make sure \c a is always >0
/**
\todo Checkout: in what situation will we be unable to normalize?
Is the test below relevant? Clarify.
*/
template<typename LP,typename FPT>
void
LPBase<LP,FPT>::impl_normalize( const detail::BaseHelper<typename typ::IsLine>& ) const
{
// not sure which one is the best...
#if 0
	auto sq = homog2d_sqrt( _v[0]*_v[0] + _v[1]*_v[1] );
#else
	#ifdef HOMOG2D_USE_TTMATH
		auto sq = ttmath::Sqrt( _v[0]*_v[0] + _v[1]*_v[1] );
	#else
		auto sq = std::hypot( _v[0], _v[1] );
	#endif
#endif

#ifndef HOMOG2D_NOCHECKS
	if( sq <= std::numeric_limits<double>::epsilon() )
		HOMOG2D_THROW_ERROR_1( "unable to normalize line, sq=" << sq << " values: a=" << _v[0] << " b=" << _v[1] << " c=" << _v[2] );
#endif
	for( int i=0; i<3; i++ )
		const_cast<LPBase<LP,FPT>*>(this)->_v[i] /= sq; // needed to remove constness

	if( _v[0] < 0. ) // a always >0
		for( int i=0; i<3; i++ )
			const_cast<LPBase<LP,FPT>*>(this)->_v[i] = -_v[i];

	if( homog2d_abs(_v[0]) < thr::nullDenom() ) // then, change sign so that b>0
		if( _v[1] < 0 )
		{
			const_cast<LPBase<LP,FPT>*>(this)->_v[1] = - _v[1];
			const_cast<LPBase<LP,FPT>*>(this)->_v[2] = - _v[2];
		}
}

//------------------------------------------------------------------
/// Point normalization. Just make sure a>0
template<typename LP,typename FPT>
void
LPBase<LP,FPT>::impl_normalize( const detail::BaseHelper<typename typ::IsPoint>& ) const
{
	if( _v[0] < 0. )
	{
		const_cast<LPBase<LP,FPT>*>(this)->_v[0] = -_v[0];
		const_cast<LPBase<LP,FPT>*>(this)->_v[1] = -_v[1];
		const_cast<LPBase<LP,FPT>*>(this)->_v[2] = -_v[2];
	}
#ifndef HOMOG2D_NOCHECKS
	if(
		homog2d_abs(_v[2]) < thr::nullDenom()
		&&
		( _v[0] < thr::nullOrthogDistance() && homog2d_abs(_v[1]) < thr::nullOrthogDistance() )
	)
		HOMOG2D_THROW_ERROR_1( "invalid point values" );
#endif
}

//------------------------------------------------------------------
template<typename LP,typename FPT>
template<typename FPT2>
HOMOG2D_INUMTYPE
LPBase<LP,FPT>::getCoord( GivenCoord gc, FPT2 other ) const
{
	static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot call on a point" );

	const auto a = static_cast<HOMOG2D_INUMTYPE>( _v[0] );
	const auto b = static_cast<HOMOG2D_INUMTYPE>( _v[1] );
	auto denom = ( gc == GivenCoord::X ? b : a );
#ifndef HOMOG2D_NOCHECKS
	if( homog2d_abs(denom) < thr::nullDenom() )
		HOMOG2D_THROW_ERROR_1( "null denominator encountered" );
#endif
	if( gc == GivenCoord::X )
		return ( -a * other - _v[2] ) / b;
	else
		return ( -b * other - _v[2] ) / a;
}

//------------------------------------------------------------------
/// Returns pair of points on line at distance \c dist from point on line
/// at coord \c coord.
template<typename LP,typename FPT>
template<typename FPT2,typename FPT3>
PointPair_<FPT>
LPBase<LP,FPT>::getPoints( const Point2d_<FPT2>& pt, FPT3 dist ) const
{
	static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot call on a point" );
	HOMOG2D_CHECK_IS_NUMBER(FPT2);
	HOMOG2D_CHECK_IS_NUMBER(FPT3);

#ifndef HOMOG2D_NOCHECKS
	if( this->distTo( pt ) > thr::nullDistance() )
	{
		std::cerr << "homog2d: distance=" << std::scientific << this->distTo( pt )
			<< " > null distance (" << thr::nullDistance() << ")\n";
		HOMOG2D_THROW_ERROR_2( "getPoints", "point is not on line" );
	}
#endif

	return priv::getPoints_B2( pt, dist, *this );
}


//------------------------------------------------------------------
/// Returns an orthogonal line to the one it is called on, at a point on the line defined by one of its coordinates.
template<typename LP,typename FPT>
template<typename FPT2>
Line2d_<FPT>
LPBase<LP,FPT>::getOrthogLine( GivenCoord gc, FPT2 val ) const
{
	static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot call getOrthogLine() on a point" );

	auto other_val = getCoord( gc, val );
	Point2d_<HOMOG2D_INUMTYPE> pt( other_val, val ) ;
	if( gc == GivenCoord::X )
		pt.set( val, other_val );

	return priv::getOrthogonalLine_B2( pt, *this );
}

/// Returns an orthogonal line to the one it is called on, at point \c pt
template<typename LP,typename FPT>
template<typename FPT2>
Line2d_<FPT>
LPBase<LP,FPT>::getOrthogLine( const Point2d_<FPT2>& pt ) const
{
	static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot call getOrthogLine() on a point" );

/*
#ifndef HOMOG2D_NOCHECKS
	if( this->distTo( pt ) > thr::nullDistance() )
	{
		std::cerr << "homog2d: distance=" << std::scientific << this->distTo( pt )
			<< "> null distance (" << thr::nullDistance() << ")\n";
		HOMOG2D_THROW_ERROR_1( "point is not on line" );
	}
#endif
*/
	return priv::getOrthogonalLine_B2( pt, *this );
}

//------------------------------------------------------------------
template<typename LP,typename FPT>
template<typename FPT2,typename T>
Line2d_<FPT>
LPBase<LP,FPT>::getRotatedLine( const Point2d_<FPT2>& pt, T theta ) const
{
	HOMOG2D_CHECK_IS_NUMBER(T);
	static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot call getRotatedLine() on a point" );

#ifndef HOMOG2D_NOCHECKS
	if( this->distTo( pt ) > thr::nullDistance() )
	{
		std::cerr << "homog2d: distance=" << std::scientific << this->distTo( pt )
			<< "> null distance (" << thr::nullDistance() << ")\n";
		HOMOG2D_THROW_ERROR_2( "getLineAngle", "point is not on line" );
	}
#endif
	Homogr_<HOMOG2D_INUMTYPE> H;
	H.addTranslation( -pt.getX(), -pt.getY() ).addRotation(theta).addTranslation( pt.getX(), pt.getY() );
	return H * *this;
}

/// Returns the shortest (oriented) segment that joins a point and a line
/**
Upon return, the first point will hold the intersection point (projection of point on line),
and the second will hold the given point.
*/
template<typename LP,typename FPT>
template<typename FPT2>
OSegment_<FPT>
LPBase<LP,FPT>::getOrthogSegment( const Point2d_<FPT2>& pt ) const
{
	static_assert( !std::is_same_v<LP,typ::IsPoint>, "Invalid call, cannot compute orthogonal segment between two points" );

	Line2d_<HOMOG2D_INUMTYPE> src = *this;  // copy to highest precision
	auto dist = src.distTo(pt);
#ifndef HOMOG2D_NOCHECKS
	if( dist < thr::nullDistance() )   // sanity check
		HOMOG2D_THROW_ERROR_1( "unable to compute segment, distance too small=" << dist );
#endif
	auto pair_lines = getParallelLines( dist );

// determine on which of the two parallel lines does the point lie?
	Line2d_<HOMOG2D_INUMTYPE>* pline = &pair_lines.first;
	if( pt.distTo(pair_lines.second) < thr::nullDistance() )
		pline = &pair_lines.second;

	auto oline = pline->getOrthogLine( pt );
	auto p2 = *this * oline;
	return OSegment_<FPT>( p2, pt );
}

//------------------------------------------------------------------
/// Returns an parallel line, implementation of getParallelLine().
template<typename LP,typename FPT>
template<typename FPT2>
Line2d_<FPT>
LPBase<LP,FPT>::getParallelLine( const Point2d_<FPT2>& pt ) const
{
	static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot call getParallelLine() on a point" );

	Line2d_<FPT> out = *this;
	out._v[2] = static_cast<HOMOG2D_INUMTYPE>(-_v[0]) * pt.getX() - _v[1] * pt.getY();
	out.p_normalizePL();
	return out;
}

//------------------------------------------------------------------

/// Implementation for lines
template<typename LP,typename FPT>
template<typename T>
std::pair<Line2d_<FPT>,Line2d_<FPT>>
LPBase<LP,FPT>::getParallelLines( T dist ) const
{
	static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot call getParallelLines() on a point" );

	Line2d_<FPT> l1 = *this;
	Line2d_<FPT> l2 = *this;
	l1._v[2] = static_cast<HOMOG2D_INUMTYPE>(this->_v[2]) + dist;
	l2._v[2] = static_cast<HOMOG2D_INUMTYPE>(this->_v[2]) - dist;
	return std::make_pair( l1, l2 );
}

//------------------------------------------------------------------
/// Comparison operator, used for lines
/**
Definition: two lines will be equal:
- if they are parallel
AND
- if their offset (3 third value) is less than nullDistance()
*/
template<typename LP,typename FPT>
bool
LPBase<LP,FPT>::impl_op_equal( const LPBase<LP,FPT>& other, const detail::BaseHelper<typename typ::IsLine>& ) const
{
	if( !this->isParallelTo( other ) )
		return false;

	if( std::fabs( _v[2] - other._v[2] ) > thr::nullDistance() )
		return false;

	return true;
}

/// Comparison operator, used for points
template<typename LP,typename FPT>
bool
LPBase<LP,FPT>::impl_op_equal( const LPBase<LP,FPT>& other, const detail::BaseHelper<typename typ::IsPoint>& ) const
{
	return ( this->distTo( other ) < thr::nullDistance() );
}

//------------------------------------------------------------------
/// Sorting operator, for points (illegal for lines)
template<typename LP,typename FPT>
bool
LPBase<LP,FPT>::operator < ( const LPBase<LP,FPT>& other ) const
{
	static_assert( std::is_same_v<LP,typ::IsPoint>, "Invalid < operator: you cannot sort lines" );

	if( getX() < other.getX() )
		return true;
	if( getX() > other.getX() )
		return false;
	if( getY() < other.getY() )
		return true;
	return false;
}

} // namespace base

//------------------------------------------------------------------
/// Inner implementation details
namespace detail {

/// Cross product of points * points or line * line
/**
- https://en.wikipedia.org/wiki/Cross_product#Coordinate_notation
*/
template<typename Out,typename In,typename FPT1,typename FPT2>
base::LPBase<Out,FPT1>
crossProduct(
	const base::LPBase<In,FPT1>& r1,
	const base::LPBase<In,FPT2>& r2
)
{
	auto r1_a = static_cast<HOMOG2D_INUMTYPE>(r1._v[0]);
	auto r1_b = static_cast<HOMOG2D_INUMTYPE>(r1._v[1]);
	auto r1_c = static_cast<HOMOG2D_INUMTYPE>(r1._v[2]);
	auto r2_a = static_cast<HOMOG2D_INUMTYPE>(r2._v[0]);
	auto r2_b = static_cast<HOMOG2D_INUMTYPE>(r2._v[1]);
	auto r2_c = static_cast<HOMOG2D_INUMTYPE>(r2._v[2]);

	base::LPBase<Out,FPT1> res;
	res._v[0] = static_cast<FPT1>( r1_b * r2_c  - r1_c * r2_b );
	res._v[1] = static_cast<FPT1>( r1_c * r2_a  - r1_a * r2_c );
	res._v[2] = static_cast<FPT1>( r1_a * r2_b  - r1_b * r2_a );

	return res;
}

/// Cross product of two oriented segments, return a scalar
template<typename FPT1,typename FPT2>
HOMOG2D_INUMTYPE
crossProductV(
	const base::SegVec<typ::IsOSeg,FPT1>& v1,
	const base::SegVec<typ::IsOSeg,FPT2>& v2
)
{
	auto ppts1 = v1.getPts();
	auto ppts2 = v2.getPts();

	auto dx1 =
		static_cast<HOMOG2D_INUMTYPE>( ppts1.second.getX()  )
		- static_cast<HOMOG2D_INUMTYPE>( ppts1.first.getX() );
	auto dy1 =
		static_cast<HOMOG2D_INUMTYPE>(   ppts1.second.getY() )
		- static_cast<HOMOG2D_INUMTYPE>( ppts1.first.getY()  );
	auto dx2 =
		static_cast<HOMOG2D_INUMTYPE>(   ppts2.second.getX() )
		- static_cast<HOMOG2D_INUMTYPE>( ppts2.first.getX()  );
	auto dy2 =
		static_cast<HOMOG2D_INUMTYPE>(   ppts2.second.getY() )
		- static_cast<HOMOG2D_INUMTYPE>( ppts2.first.getY()  );

	return dx1 * dy2 - dy1 * dx2;
}

} // namespace detail


//------------------------------------------------------------------
///////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////

namespace base {

/// Generic init from two numeric args
template<typename LP, typename FPT>
template<typename T1,typename T2>
void
LPBase<LP,FPT>::p_init_2( const T1& v1, const T2& v2 )
{
	if constexpr( std::is_same_v<LP,typ::IsPoint> )
	{
		_v[0] = v1;
		_v[1] = v2;
		_v[2] = 1.;
	}
	else
	{
		Point2d_<FPT> pt1;                // 0,0
		Point2d_<FPT> pt2(v1,v2);
		pt2.p_normalizePL();
		*this = detail::crossProduct<typ::IsLine>( pt1, pt2 );
		p_normalizePL();
	}
}

/// Overload for point to point distance
template<typename LP, typename FPT>
template<typename FPT2>
HOMOG2D_INUMTYPE
LPBase<LP,FPT>::impl_distToPoint( const Point2d_<FPT2>& pt, const detail::BaseHelper<typename typ::IsPoint>& ) const
{
#ifndef HOMOG2D_USE_TTMATH
	return std::hypot(
		static_cast<HOMOG2D_INUMTYPE>( getX() ) - static_cast<HOMOG2D_INUMTYPE>( pt.getX() ),
		static_cast<HOMOG2D_INUMTYPE>( getY() ) - static_cast<HOMOG2D_INUMTYPE>( pt.getY() )
	);
#else
	auto v1 = static_cast<HOMOG2D_INUMTYPE>( getX() ) - static_cast<HOMOG2D_INUMTYPE>( pt.getX() );
	auto v2 = static_cast<HOMOG2D_INUMTYPE>( getY() ) - static_cast<HOMOG2D_INUMTYPE>( pt.getY() );
	return ttmath::Sqrt( v1*v1 + v2*v2 );
#endif
}
//------------------------------------------------------------------
/// Returns distance between the line and point \b pt. overload for line to point distance.
/**
http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
<pre>
        | a.x0 + b.y0 + c |
  d = -----------------------
         sqrt( a*a + b*b )
</pre>
\todo Do we really require computation of hypot ? (because the line is supposed to be normalized, i.e. h=1 ?)
*/
template<typename LP, typename FPT>
template<typename FPT2>
HOMOG2D_INUMTYPE
LPBase<LP,FPT>::impl_distToPoint( const Point2d_<FPT2>& pt, const detail::BaseHelper<typename typ::IsLine>& ) const
{
	return std::fabs(
		static_cast<HOMOG2D_INUMTYPE>( _v[0] ) * pt.getX()
		+ static_cast<HOMOG2D_INUMTYPE>( _v[1] ) * pt.getY()
		+ static_cast<HOMOG2D_INUMTYPE>( _v[2] )
	)
#ifndef HOMOG2D_USE_TTMATH
	/ std::hypot( _v[0], _v[1] );
#else
;
#endif
}

/// overload for line to point distance
template<typename LP, typename FPT>
template<typename FPT2>
HOMOG2D_INUMTYPE
LPBase<LP,FPT>::distTo( const Line2d_<FPT2>& li ) const
{
	static_assert( !std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot compute distance between two lines" );
	return li.distTo( *this );
}

template<typename LP, typename FPT>
template<typename FPT2>
HOMOG2D_INUMTYPE
LPBase<LP,FPT>::distTo( const Segment_<FPT2>& seg ) const
{
	static_assert( !std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot compute distance between a line and a segment" );
	return seg.distTo( *this );
}

} // namespace base

//------------------------------------------------------------------
/// Free function, returns angle between two segments/lines
/// \sa Segment_::getAngle()
/// \sa Line2d_::getAngle()
template<typename T1,typename T2>
HOMOG2D_INUMTYPE
getAngle( const T1& t1, const T2& t2 )
{
	static_assert(
		(
			   std::is_same_v<typename T1::SType,typ::IsLine>
			|| std::is_same_v<typename T1::SType,typ::IsSegment>
			|| std::is_same_v<typename T1::SType,typ::IsOSeg>
		) &&
		(
			   std::is_same_v<typename T2::SType,typ::IsLine>
			|| std::is_same_v<typename T2::SType,typ::IsSegment>
			|| std::is_same_v<typename T2::SType,typ::IsOSeg>
		),
		"Both types must be either a Line or a Segment"
	);
	return t1.getAngle( t2 );
}

//------------------------------------------------------------------
namespace base {

template<typename LP, typename FPT>
template<typename FPT2>
bool
LPBase<LP,FPT>::isParallelTo( const Line2d_<FPT2>& li ) const
{
	static_assert( std::is_same_v<LP,typ::IsLine>, "Invalid: you cannot use IsParallel() with a point" );

	if( this->getAngle(li) < thr::nullAngleValue() )
		return true;
	return false;
}

//------------------------------------------------------------------
/// Returns the angle (in rad.) between the line and the other one.
/**
- The returned value will be in the range <code> [0, M_PI/2] </code>

If the lines \f$ (a_0,a_1,a_2) \f$ and \f$ (b_0,b_1,b_2) \f$ are correctly normalized (should be, but...)
then the angle between them is \f$ \alpha = acos( a_0*b_0 + a_1*b_1) \f$. <br>
However, in "some situations", even if the lines have been previously normalized (which is the case here)
we can encounter numerical issues, so here we "reinforce the normalization and compute:
\f[
\alpha = acos \left(
	\frac{a_0*b_0 + a_1*b_1} { \sqrt{ a_0^2 + a_1^2 } * \sqrt{ b_0^2 + b_1^2 } }
\right)
\f]
In some situations, the value inside the parenthesis "may" be equal to \f$ 1+\epsilon \f$
(typically, something like "1.0000000000123").
This is out of bounds for the \f$ acos() \f$ function, that will then return "nan"
(thus induce some failure further on). <br>
To avoid this, a checking is done, and any value higher than 1 will be truncated.
This is logged on \c std::cerr so that the user may take that into consideration.

\todo more investigation needed ! : what are the exact situation that will lead to this event?
*/
template<typename LP, typename FPT>
template<typename LP2, typename FPT2>
HOMOG2D_INUMTYPE
LPBase<LP,FPT>::getAngle( const LPBase<LP2,FPT2>& li ) const
{
	static_assert( !std::is_same_v<LP,typ::IsPoint>, "cannot get angle of a point" );

	HOMOG2D_INUMTYPE l1_a = _v[0];
	HOMOG2D_INUMTYPE l1_b = _v[1];
	HOMOG2D_INUMTYPE l2_a = li._v[0];
	HOMOG2D_INUMTYPE l2_b = li._v[1];
	HOMOG2D_INUMTYPE res = l1_a * l2_a + l1_b * l2_b;

	res /= homog2d_sqrt( (l1_a*l1_a + l1_b*l1_b) * (l2_a*l2_a + l2_b*l2_b) );
	auto fres = homog2d_abs(res);

	if( fres > 1.0 )
	{
		HOMOG2D_LOG_WARNING(
			"homog2d Warning: angle computation overflow detected, value "
			<< std::scientific << std::setprecision(20)
			<< fres << ", truncated to 1.0\n input lines:\n  l1: "
			<< *this << "\n  l2: " << li
		);
		fres = 1.0;
	}
	return homog2d_acos( fres );
}

//------------------------------------------------------------------
/// Returns true if point is inside (or on the edge) of a flat rectangle
template<typename LP, typename FPT>
template<typename FPT2>
bool
LPBase<LP,FPT>::isInside( const FRect_<FPT2>& rect ) const
{
	if constexpr( std::is_same_v<LP,typ::IsLine> )
		return false;
	else
	{
		auto pair_pts = rect.getPts();
		const auto& p00 = pair_pts.first;
		const auto& p11 = pair_pts.second;
		return detail::ptIsInside( *this, p00, p11 );
	}
}


//------------------------------------------------------------------
#if 0
// deprecated. kept here just in case it would be useful in the future (who knows?)
namespace sub {

/// Returns the index of the segment (among the ones in \c bbox)
/// that is the farthest from point \c pt
template<typename FPT1,typename FPT2>
int
getFarthestSegment( const Point2d_<FPT1>& pt, const FRect_<FPT2>& bbox )
{
	auto segs = bbox.getSegs();
	using localPair = std::pair<HOMOG2D_INUMTYPE,int>;
	std::array<localPair,4> dist_array;
	for( int i=0; i<4; i++ )
	{
		dist_array[i].first  = pt.distTo( segs[i].getLine() );
		dist_array[i].second = i;
	}
	std::sort(
		dist_array.begin(),
		dist_array.end(),
		[]                     // lambda
		( const localPair& i, const localPair& j )
		{
			return i.first < j.first;
		}
	);

	return dist_array[3].second;
}

} // namespace sub
#endif
//------------------------------------------------------------------
/// Returns true if point is inside closed Polyline
/**
See https://en.wikipedia.org/wiki/Point_in_polygon

Implementing this algorithm in a naive way may encounter some issue:
if the main segment used to compute the number of intersections is colinear with
one of the points of the polyline, we might run into a numerical issue.

See page \ref md_docs_homog2d_algorithms for details.
*/
template<typename LP, typename FPT>
template<typename T,typename PTYPE>
bool
LPBase<LP,FPT>::impl_isInsidePoly( const base::PolylineBase<PTYPE,T>& poly, const detail::BaseHelper<typename typ::IsPoint>& ) const
{
	if( !poly.isSimple() )
		return false;

// step 1: if point is outside bounding box, return false
	auto bbox = poly.getBB();
	if( !this->isInside(bbox) )
		return false;

// step 2: check if point is lying on one of the segments. If so, return false;
	for( auto seg: poly.getSegs() )
		if( seg.getLine().distTo( *this ) < thr::nullDistance() )
				return false;

	auto segarr = bbox.getExtended().getSegs();
	std::vector<Segment_<HOMOG2D_INUMTYPE>> seg_bb( segarr.begin(), segarr.end() );

	int iter = 0;
	do
	{
		bool tooClose = false;
		for( size_t i=0; i<seg_bb.size(); i++ )        // iterate on all segments of the extended bounding box
		{
			Segment_<HOMOG2D_INUMTYPE> seg_ref( *this, seg_bb[i].getCenter() );

			auto poly_pts = poly.getPts();
			for( size_t j=0; j<poly_pts.size() && !tooClose; j++  )   // make sure that every point of the polyline is not on the reference segment
			{
				const auto& poly_pt = poly_pts[j];
				if( poly_pt.distTo(seg_ref) < thr::nullDistance() )
					tooClose = true;
			}
			if( !tooClose )     // if no polyline points
			{                   // was found on the segment supporting line
				size_t c = 0;
				for( auto seg: poly.getSegs() )  // iterate on the polyline segments
				{
					if( seg.intersects(seg_ref)() )
						c++;
				}
				return static_cast<bool>( c%2 );
			}
		}
		iter++;
		if( tooClose )  // if unable to find a suitable reference segment,
		{               //  we double the number of segments, by splitting each of them
			if( iter < HOMOG2D_MAXITER_PIP ) // (only if not max iterations!)
			{
				std::vector<Segment_<HOMOG2D_INUMTYPE>> seg_bb2( seg_bb.size() * 2 );
				for( size_t j=0; j<seg_bb.size(); j++ )
				{
					auto pseg      = seg_bb[j].split();
					seg_bb2[j*2]   = pseg.first;
					seg_bb2[j*2+1] = pseg.second;
				}
				std::swap( seg_bb, seg_bb2 );
			}
		}
	}
	while( iter < HOMOG2D_MAXITER_PIP );
	HOMOG2D_THROW_ERROR_1( "unable to determine if point is inside after " + std::to_string(HOMOG2D_MAXITER_PIP) + " iterations" );
	return false; // to avoid a warning
}

template<typename LP, typename FPT>
template<typename T,typename PTYPE>
constexpr bool
LPBase<LP,FPT>::impl_isInsidePoly( const base::PolylineBase<PTYPE,T>&, const detail::BaseHelper<typename typ::IsLine>& ) const
{
	return false;
}

//------------------------------------------------------------------
/// Point is inside Polyline
template<typename LP, typename FPT>
template<typename FPT2>
bool
LPBase<LP,FPT>::isInside( const Ellipse_<FPT2>& ell ) const
{
	if constexpr( std::is_same_v<LP,typ::IsPoint> )
		return ell.pointIsInside( *this );
	else
		return false;
}


//------------------------------------------------------------------
/// Intersection of line and circle: implementation for points
template<typename LP, typename FPT>
template<typename T>
constexpr detail::Intersect<detail::Inters_2,FPT>
LPBase<LP,FPT>::impl_intersectsCircle( const Point2d_<FPT>&, T, const detail::BaseHelper<typename typ::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use intersects(Circle) with a point" );
}

/// Line/Circle intersection: implementation
/// For computation details, checkout http://skramm.lautre.net/files/misc/intersect_circle_line.pdf
/// \todo 20230124: change return type: intersection of a circle with a line can be a single point
template<typename LP, typename FPT>
template<typename T>
detail::Intersect<detail::Inters_2,FPT>
LPBase<LP,FPT>::impl_intersectsCircle(
	const Point2d_<FPT>& pt,       ///< circle origin
	T                    radius,   ///< radius
	const detail::BaseHelper<typename typ::IsLine>&  ///< dummy arg, needed so that this overload is only called for lines
) const
{
	detail::Intersect<detail::Inters_2,FPT> out;
	HOMOG2D_CHECK_IS_NUMBER(T);
	const HOMOG2D_INUMTYPE a = static_cast<HOMOG2D_INUMTYPE>(_v[0]); // just to lighten a bit...
	const HOMOG2D_INUMTYPE b = static_cast<HOMOG2D_INUMTYPE>(_v[1]);
	const HOMOG2D_INUMTYPE c = static_cast<HOMOG2D_INUMTYPE>(_v[2]);

// step 1: translate to origin
	auto cp = pt.getX() * a + pt.getY() * b + c;

// step 2: compute distance	between center (origin) and middle point
	auto a2b2 = a * a + b * b;
	auto d0 = homog2d_abs(cp) / homog2d_sqrt( a2b2 );
	if( radius < d0 )                            // if less than radius,
		return out;                         // no intersection

	auto d2 = radius*radius - d0*d0;

// step 3: compute coordinates of middle point B
	auto xb = - a * cp / a2b2;
	auto yb = - b * cp / a2b2;

// step 4: compute coordinates of intersection points, with center at (0,0)
	auto m  = homog2d_sqrt( d2 / a2b2 );
	auto x1 = xb + m*b;
	auto y1 = yb - m*a;

	auto x2 = xb - m*b;
	auto y2 = yb + m*a;

// last step: translate back
	out._ptIntersect_1.set( x1 + pt.getX(), y1 + pt.getY() );
	out._ptIntersect_2.set( x2 + pt.getX(), y2 + pt.getY() );
	out._doesIntersect = true;

	priv::fix_order( out._ptIntersect_1, out._ptIntersect_2 );
	return out;
}
//------------------------------------------------------------------
/// Overload used when attempting to use that on a point
template<typename LP, typename FPT>
template<typename FPT2>
constexpr detail::IntersectM<FPT>
LPBase<LP,FPT>::impl_intersectsFRect( const FRect_<FPT2>&, const detail::BaseHelper<typename typ::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call intersects(FRect) on a point" );
}

/// Line/FRect intersection
/**
Returns 0, 1, or 2 intersections
(1 when the line touches the edge of rectangle)

\todo 20230118: check if there is a need to fix the order of points in returned object (this was done previously)
*/
template<typename LP, typename FPT>
template<typename FPT2>
detail::IntersectM<FPT>
LPBase<LP,FPT>::impl_intersectsFRect( const FRect_<FPT2>& rect, const detail::BaseHelper<typename typ::IsLine>& ) const
{
	std::vector<Point2d_<FPT>> pti;
	for( const auto& seg: rect.getSegs() ) // get segment of rectangle
	{
		auto ppts_seg = seg.getPts();
		auto inters = seg.intersects( *this );
		if( inters() )
		{
			bool storePoint(true);
			auto pt = inters.get();
			if( pt == ppts_seg.first || pt == ppts_seg.second )  // if intersection point is one of the segment pts
				if( pti.size() == 1 ) {                          // AND if there is already one
					if( pti[0] == pt )                           // AND that one is already stored
						storePoint = false;                      // THEN don't store it
				}
			if( storePoint )
				pti.push_back( pt );

			if( pti.size() == 2 )  // already got 2, done
				break;
		}
	}
	if( pti.empty() )
		return detail::IntersectM<FPT>();
	detail::IntersectM<FPT> out;
	out.add( pti );
	return out;
}

} // namespace base

/////////////////////////////////////////////////////////////////////////////
// SECTION  - PRODUCT OPERATORS DEFINITIONS (HELPER FUNCTIONS)
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
/// Apply homography to a vector/array/list (type T) of points or lines.
template<typename W,typename FPT>
template<typename T>
void
Hmatrix_<W,FPT>::applyTo( T& vin ) const
{
#if 0
	std::transform(
		std::begin( vin ),
		std::end( vin ),
		std::begin( vin ),
		[](){}
	);
#else
	for( auto& elem: vin )
		elem = *this * elem;
#endif
}

namespace detail {

/// Implementation of product 3x3 by 3x3

template<typename FPT1,typename FPT2,typename FPT3>
void
product(
	Matrix_<FPT1>&       out,
	const Matrix_<FPT2>& h1,
	const Matrix_<FPT3>& h2
)
{
	out.p_fillZero();
	for( int i=0; i<3; i++ )
		for( int j=0; j<3; j++ )
			for( int k=0; k<3; k++ )
				out.value(i,j) +=
					static_cast<HOMOG2D_INUMTYPE>( h1.value(i,k) ) * h2.value(k,j);
}

/// Implementation of product 3x3 by 3x1
/**
- T1 and T2: typ::IsLine or typ::IsPoint (same but also different)
*/
template<typename T1,typename T2,typename FPT1,typename FPT2>
void
product(
	base::LPBase<T1,FPT1>&        out,
	const Matrix_<FPT2>&          h,
	const base::LPBase<T2,FPT1>&  in
)
{
	for( int i=0; i<3; i++ )
	{
		auto sum  = static_cast<HOMOG2D_INUMTYPE>(h._mdata[i][0]) * in._v[0];
		sum      += h._mdata[i][1] * in._v[1];
		sum      += h._mdata[i][2] * in._v[2];
		out._v[i] = sum;
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// SECTION  - PRODUCT OPERATORS DEFINITIONS
/////////////////////////////////////////////////////////////////////////////

/// Free function template, product of two lines, returns a point
template<typename FPT,typename FPT2>
Point2d_<FPT>
operator * ( const Line2d_<FPT>& lhs, const Line2d_<FPT2>& rhs )
{
#ifndef HOMOG2D_NOCHECKS
	if( lhs.isParallelTo(rhs) )
		HOMOG2D_THROW_ERROR_1( "lines are parallel, unable to compute product:\nlhs="
			<< lhs << " rhs=" << rhs
		);
#endif

	return detail::crossProduct<typ::IsPoint,typ::IsLine,FPT>(lhs, rhs);
}

/// Free function template, product of two segments/vector, returns a point
template<typename SV1,typename SV2,typename FPT1,typename FPT2>
Point2d_<FPT1>
operator * ( const base::SegVec<SV1,FPT1>& lhs, const base::SegVec<SV2,FPT2>& rhs )
{
	return lhs.getLine() * rhs.getLine();
}

#ifdef HOMOG2D_FUTURE_STUFF
/// Apply Epipolar matrix to a point or line, this will return the opposite type.
/// Free function, templated by point or line
template<typename T,typename U,typename V>
base::LPBase<typename detail::HelperPL<T>::OtherType,V>
operator * ( const Hmatrix_<typ::IsEpipmat,U>& h, const base::LPBase<T,V>& in )
{
	LPBase<typename detail::HelperPL<T>::OtherType,V> out;
	detail::product( out, h._data, in );
	return out;
}
#endif

/// Free function, apply homography to a point.
template<typename T,typename U>
Point2d_<T>
operator * ( const Homogr_<U>& h, const Point2d_<T>& in )
{
	Point2d_<T> out;
	detail::product( out, h, in );
	return out;
}

/// Free function, apply homography to a line.
template<typename T,typename U>
Line2d_<T>
operator * ( const Homogr_<U>& h, const Line2d_<T>& in )
{
	if( h._hmt == nullptr )             // if H^-T	not allocated yet, do it
	{
		h._hmt = std::unique_ptr<detail::Matrix_<U>>( new detail::Matrix_<U>() );
		h._hasChanged = true;
	}

	if( h._hasChanged )                  // if homography has changed, recompute inverse and transposed
	{
		auto hi = h;
		auto mat_inv = hi.inverse().transpose();
		*h._hmt = mat_inv;
		h._hasChanged = false;
	}

	Line2d_<T> out;
	detail::product( out, *h._hmt, in );
	out.p_normalizePL();
	return out;
}

/// Apply homography to a Segment
template<typename SV,typename FPT1,typename FPT2>
Segment_<FPT1>
operator * ( const Homogr_<FPT1>& h, const base::SegVec<SV,FPT2>& seg )
{
	const auto& pts = seg.getPts();
	Point2d_<FPT1> pt1 = h * pts.first;
	Point2d_<FPT1> pt2 = h * pts.second;
	return base::SegVec<SV,FPT2>( pt1, pt2 );
}

/// Apply homography to a Polyline
template<typename FPT1,typename FPT2,typename PLT>
base::PolylineBase<PLT,FPT1>
operator * ( const Homogr_<FPT2>& h, const base::PolylineBase<PLT,FPT1>& pl )
{
	base::PolylineBase<PLT,FPT1> out;
	const auto& pts = pl.getPts();
	for( const auto& pt: pts )
		out.p_addPoint( h * pt );
	return out;
}

/// Apply homography to a flat rectangle produces a closed polyline
template<typename FPT1,typename FPT2>
CPolyline_<FPT1>
operator * ( const h2d::Homogr_<FPT2>& h, const h2d::FRect_<FPT1>& rin )
{
	h2d::CPolyline_<FPT1> out;
	for( const auto& pt: rin.get4Pts() )
		out.p_addPoint( h * pt );
	return out;
}

/// Apply homography to a Ellipse, produces an Ellipse
/**
\f[
Q' = H^{-T} \cdot Q \cdot H^{-1}
\f]
*/
template<typename FPT1,typename FPT2>
Ellipse_<FPT1>
operator * ( const Homogr_<FPT2>& h, const Ellipse_<FPT1>& ell_in )
{
	auto hm = static_cast<detail::Matrix_<HOMOG2D_INUMTYPE>>(h);
	hm.inverse();
	auto hmt = hm;
	hmt.transpose();

	const auto& ell_in2 = static_cast<detail::Matrix_<FPT1>>(ell_in);
	auto prod = hmt * ell_in2 * hm;

	Ellipse_<FPT1> out( prod );
	return out;
}

/// Apply homography to a Circle, produces an Ellipse
/** Converts the circle to an ellipse, then calls the corresponding code */
template<typename FPT1,typename FPT2>
Ellipse_<FPT1>
operator * ( const Homogr_<FPT2>& h, const Circle_<FPT1>& cir )
{
	Ellipse_<FPT1> ell_in( cir );
	return h * ell_in;
}

//------------------------------------------------------------------
namespace priv {

/// Allocation for \c std::array container
/**
\sa operator * ( const Hmatrix_&, const Cont& );
*/
template<
	typename Cont,
	typename std::enable_if<
		trait::IsArray<Cont>::value,
		Cont
	>::type* = nullptr
>
Cont
alloc( std::size_t /* unused here */ )
{
	return Cont();
}

/// Allocation for \c std::vector and \c std::list container
template<
	typename Cont,
	typename std::enable_if<
		!trait::IsArray<Cont>::value,
		Cont
	>::type* = nullptr
>
Cont
alloc( std::size_t nb )
{
	return Cont(nb);
}

} // namespace priv

//------------------------------------------------------------------
/// Used to proceed multiple products, whatever the container (\c std::list, \c std::vector, or \c std::array).
/// Returned container is of same type as given input
template<typename FPT,typename Cont>
typename std::enable_if<trait::IsContainer<Cont>::value,Cont>::type
operator * (
	const Hmatrix_<typ::IsHomogr,FPT>& h,    ///< Matrix
	const Cont&                        vin   ///< Input container
)
{
	Cont vout = priv::alloc<Cont>( vin.size() );
	auto it = std::begin( vout );
	for( const auto& elem: vin )
		*it++ = h * elem;
	return vout;
}

/////////////////////////////////////////////////////////////////////////////
// SECTION  - FREE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

template<typename FPT>
FPT getX( const Point2d_<FPT>& pt) { return pt.getX(); }

template<typename FPT>
FPT getY( const Point2d_<FPT>& pt) { return pt.getY(); }

//------------------------------------------------------------------
/// Free function, returns -1 or +1 depending on the side of \c pt, related to \c li
template<typename FPT1,typename FPT2>
int
side( const Point2d_<FPT1>& pt, const Line2d_<FPT2>& li )
{
	const auto& arr = li.get();
	HOMOG2D_INUMTYPE a = arr[0];
	HOMOG2D_INUMTYPE b = arr[1];
	HOMOG2D_INUMTYPE c = arr[2];

	auto dist = a * pt.getX() + b * pt.getY() + c;
	if( homog2d_abs(dist) < thr::nullDistance() )
		return 0;
	return (std::signbit( dist ) ? -1 : +1);
}

//------------------------------------------------------------------
/// Free function, distance between points
/// \sa Point2d_::distTo()
template<typename FPT1,typename FPT2>
HOMOG2D_INUMTYPE
dist( const Point2d_<FPT1>& pt1, const Point2d_<FPT2>& pt2 )
{
	return pt1.distTo( pt2 );
}

//------------------------------------------------------------------
/// Intersection area over Union area (free function)
template<typename FPT1,typename FPT2>
HOMOG2D_INUMTYPE
IoU( const FRect_<FPT1>& r1, const FRect_<FPT2>& r2 )
{
	auto ia = intersectArea( r1, r2 );
	if( ia() )
		return ia.get().area() / unionArea( r1, r2 ).area();
	return 0.;
}

//------------------------------------------------------------------
/// Free function, see FRect_::unionArea()
template<typename FPT1,typename FPT2>
CPolyline_<FPT1>
unionArea( const FRect_<FPT1>& r1, const FRect_<FPT2>& r2 )
{
	return r1.unionArea(r2);
}

//------------------------------------------------------------------
/// Free function, see FRect_::intersectArea()
template<typename FPT1,typename FPT2>
detail::RectArea<FPT1>
intersectArea(  const FRect_<FPT1>& r1, const FRect_<FPT2>& r2 )
{
	return r1.intersectArea(r2);
}

//------------------------------------------------------------------
/// Returns circle passing through 4 points of flat rectangle (free function)
/// \sa FRect_::getBoundingCircle()
template<typename FPT>
Circle_<FPT>
getBoundingCircle( const FRect_<FPT>& rect )
{
	return rect.getBoundingCircle();
}

//------------------------------------------------------------------
/// Return circle inscribed in rectangle
/// \sa hFRect_::getInscribedCircle()
template<typename FPT>
Circle_<FPT>
getInscribedCircle( const FRect_<FPT>& rect )
{
	return rect.getInscribedCircle();
}

//------------------------------------------------------------------
/// Returns true if is a polygon (free function)
///  \sa PolylineBase::isSimple()
template<typename PLT,typename FPT>
bool
isSimple( const base::PolylineBase<PLT,FPT>& pl )
{
	return pl.isSimple();
}

//------------------------------------------------------------------
/// Returns true if polygon is convex (free function)
/// \sa PolylineBase::isConvex()
template<typename PLT,typename FPT>
bool
isConvex( const base::PolylineBase<PLT,FPT>& poly )
{
	return poly.isConvex();
}

//------------------------------------------------------------------
/// Returns the number of segments (free function)
/// \sa PolylineBase::nbSegs()
template<typename PLT,typename FPT>
size_t
nbSegs( const base::PolylineBase<PLT,FPT>& pl )
{
	return pl.nbSegs();
}

//------------------------------------------------------------------
/// Returns the segments of the polyline (free function)
/// \sa PolylineBase::getSegs()
template<typename PLT,typename FPT>
std::vector<Segment_<FPT>>
getSegs( const base::PolylineBase<PLT,FPT>& pl )
{
	return pl.getSegs();
}

//------------------------------------------------------------------
/// Rotates the primitive (only available for Polyline and FRect) around (0,0)
template<typename T>
void
rotate( T& prim, Rotate rot )
{
	prim.rotate( rot );
}

//------------------------------------------------------------------
/// Rotates the primitive (only available for Polyline and FRect) around \c refpt
template<typename T,typename FPT>
void
rotate( T& prim, Rotate rot, const Point2d_<FPT>& refpt )
{
	prim.rotate( rot, refpt );
}

//------------------------------------------------------------------
/// Returns Bounding Box of Ellipse_ (free function)
/// \sa Ellipse_::getBB()
template<typename FPT>
CPolyline_<FPT>
getOBB( const Ellipse_<FPT>& ell )
{
	return ell.getOBB();
}

//------------------------------------------------------------------
/// Returns bisector lines of a Polyline
/**
Size of vector will be:
- equal to size() of polyline if closed
- equal to size()-2 of polyline if open

\warning:
- if open: the FIRST line (index 0) will correspond to SECOND point of the polyline
*/
template<typename PLT,typename FPT>
std::vector<Line2d_<HOMOG2D_INUMTYPE>>
getBisectorLines( const base::PolylineBase<PLT,FPT>& pl )
{
	if( pl.size() < 3 )
	{
		HOMOG2D_THROW_ERROR_1( "unable, minimum size is 3, currently=" << pl.size() );
	}
	const auto& pts  = pl.getPts();
	std::vector<Line2d_<HOMOG2D_INUMTYPE>> out;

	if constexpr ( std::is_same_v<PLT,typ::IsOpen> )
	{
		out.reserve( pl.size()-2 );
		OSegment_<HOMOG2D_INUMTYPE> seg1( pts[1], pts[0] );
		for( size_t i=0; i<pl.size()-2; i++ )
		{
			OSegment_<HOMOG2D_INUMTYPE> seg2( pts[i+1], pts[i+2] );
			auto angle = seg1.getAngle( seg2 );
			out.emplace_back( seg1.getLine().getRotatedLine( pts[i+1], angle/2. ) );
			seg1 = -seg2;
		}
	}
	else
	{
		out.reserve( pl.size() );
		OSegment_<HOMOG2D_INUMTYPE> seg1( pts[0], pts[pl.size()-1] );
		for( size_t i=0; i<pl.size(); i++ )
		{
			auto next = ( i != pl.size()-1 ? i+1 : 0 );
			OSegment_<HOMOG2D_INUMTYPE> seg2( pts[i], pts[next] );
			auto angle = seg1.getAngle( seg2 );
			out.emplace_back( seg1.getLine().getRotatedLine( pts[i], angle/2. ) );
			seg1 = -seg2;
		}
	}
	return out;
}


//------------------------------------------------------------------
/// Holds free functions returning a pair of points
namespace ppair {

/// Returns pair of points defining a BB of a primitive
/// Overload 1/4, private free function
/**
Arg is a neither a Point2d, a Segment, a Line2d or a polyline
*/
template<
	typename T,
	typename std::enable_if<
		(                                                          // type is
			   !std::is_same<T,Line2d_<typename    T::FType>>::value // not a line,
			&& !std::is_same<T,Point2d_<typename   T::FType>>::value // not a point,
			&& !std::is_same<T,Segment_<typename   T::FType>>::value // not a segment
			&& !std::is_same<T,OSegment_<typename  T::FType>>::value // not a vector
			&& !std::is_same<T,CPolyline_<typename T::FType>>::value // not a CPolyline
			&& !std::is_same<T,OPolyline_<typename T::FType>>::value // not a OPolyline
		)
		,T
	>::type* = nullptr
>
auto
getPointPair( const T& elem )
{
	HOMOG2D_START;
	return elem.getBB().getPts();
}

/// Returns pair of points defining a BB of a Polyline
/// Overload 2/4, private free function
template<
	typename T,
	typename std::enable_if<
		(
			std::is_same<T,CPolyline_<typename T::FType>>::value
			|| std::is_same<T,OPolyline_<typename T::FType>>::value
		)
		,T
	>::type* = nullptr
>
auto
getPointPair( const T& poly )
{
	HOMOG2D_START;

	if( poly.size() == 0 )
		HOMOG2D_THROW_ERROR_1( "cannot compute point pair of empty Polyline" );
	if( poly.size() == 2 )
	{
		const auto& pts = poly.getPts();
		return std::make_pair( pts[0], pts[1] );
	}
	return priv::getBB_Points( poly.getPts() );
}

/// Returns pair of points defining a BB of a Point2d
/// Overload 3/4, private free function
/**
This seems useless at first glance, but it is used to get the common bounding box of two objects
when one of them (or both) is a point.
*/
template<
	typename T,
	typename std::enable_if<
		std::is_same<T,Point2d_<typename T::FType>>::value
		,T
	>::type* = nullptr
>
auto
getPointPair( const T& elem )
{
	HOMOG2D_START;
	return std::make_pair( Point2d_<HOMOG2D_INUMTYPE>(elem), Point2d_<HOMOG2D_INUMTYPE>(elem) );
}

/// Returns pair of points defining a BB of Segment
/// Overload 4/4, private free function
template<
	typename T,
	typename std::enable_if<
		( std::is_same_v<T,Segment_<typename T::FType>>
		|| std::is_same_v<T,OSegment_<typename T::FType>> )
		,T
	>::type* = nullptr
>
auto
getPointPair( const T& elem )
{
	HOMOG2D_START;
	return elem.getPts();
}

/// Needed because of variant
template<typename FPT>
PointPair_<FPT>
getPointPair( const Line2d_<FPT>& )
{
	HOMOG2D_START;
	HOMOG2D_THROW_ERROR_1( "Unable to get pair of points for a Line2d" );
}

} // namespace ppair


/////////////////////////////////////////////////////////////////////////////
// SECTION - FUNCTORS USED IN FOLLOWING FREE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

#ifdef HOMOG2D_ENABLE_VRTP
namespace fct {
//------------------------------------------------------------------
/// A functor to get pair of points englobing the element
/**
\sa CommonType_
\sa area()
*/
struct PtPairFunct
{
	template<typename T>
	PointPair_<typename T::FType>
	operator ()(const T& a)
	{
		return ppair::getPointPair(a);
	}
};

//------------------------------------------------------------------
/// A functor to get the Bounding Box
/// \sa getBB()
struct BBFunct
{
	template<typename T>
//	FRect_<typename T::FType> operator ()(const T& a)
	FRect_<HOMOG2D_INUMTYPE> operator ()(const T& a)
	{
		return a.getBB();
	}
};

} // namespace fct

#endif // HOMOG2D_ENABLE_VRTP

/////////////////////////////////////////////////////////////////////////////
// SECTION  - FREE FUNCTIONS HANDLING VARIANT TYPE
/////////////////////////////////////////////////////////////////////////////

/**
\addtogroup varff Free functions handling variant type


defgroup

These function can be used on a "regular" geometric object or on a `CommonType` object,
holding a std::variant and allowing run-time polymorphism,

See md_docs_homog2d_manual.html#section_rtp
*/

//------------------------------------------------------------------
/// Free function. Returns the type of object or variant
/**
Can be printed with `getString()`
\sa CommonType_
\ingroup varff
*/
template<typename T>
Type
type( const T& elem )
{
#ifdef HOMOG2D_ENABLE_VRTP
	if constexpr( trait::IsVariant<T>::value )
		return std::visit( fct::TypeFunct{}, elem );
	else
#endif
		return elem.type();
}

//------------------------------------------------------------------
/// Returns the underlying data type of object or variant
/**
Can be printed with `getString()`
\ingroup varff
*/
template<typename T>
Dtype
dtype( const T& elem )
{
#ifdef HOMOG2D_ENABLE_VRTP
	if constexpr( trait::IsVariant<T>::value )
		return std::visit( fct::DTypeFunct{}, elem );
	else
#endif
		return elem.dtype();
}

//------------------------------------------------------------------
/// Returns length of element or variant (free function)
/// \ingroup varff
template<typename T>
HOMOG2D_INUMTYPE
length( const T& elem )
{
#ifdef HOMOG2D_ENABLE_VRTP
	if constexpr( trait::IsVariant<T>::value )
		return std::visit( fct::LengthFunct{}, elem );
	else
#endif
		return elem.length();
}

//------------------------------------------------------------------
/// Returns area of element or variant (free function)
/// \ingroup varff
template<typename T>
HOMOG2D_INUMTYPE
area( const T& elem )
{
#ifdef HOMOG2D_ENABLE_VRTP
	if constexpr( trait::IsVariant<T>::value )
		return std::visit( fct::AreaFunct{}, elem );
	else
#endif
		return elem.area();
}

//------------------------------------------------------------------
/// Returns size of element or variant (free function)
/// \ingroup varff
template<typename T>
HOMOG2D_INUMTYPE
size( const T& elem )
{
#ifdef HOMOG2D_ENABLE_VRTP
	if constexpr( trait::IsVariant<T>::value )
		return std::visit( fct::SizeFunct{}, elem );
	else
#endif
		return elem.size();
}


#ifdef HOMOG2D_ENABLE_VRTP
namespace priv {

/// Get Bounding Box for a container holding variant objects
/**
\note Here, we DO NOT preallocate the vector, because if the container holds lines, then no point pair will be added for these elements.
If we would have preallocate with 2 x size of input vector, then some default-constructed points could sneak in, and introduce an error.

\todo 20240513: At present, this is only implemented for std::vector. Let it handle std::list and std::array.
*/
template<typename FPT>
auto
getBB_CommonType( const std::vector<CommonType_<FPT>>& v_var )
{
	HOMOG2D_START;
	HOMOG2D_DEBUG_ASSERT( v_var.size(), "cannot compute bounding box of empty set of variant" );

	std::vector<Point2d_<FPT>> vpts;
	vpts.reserve( v_var.size()*2 );
	PointPair_<FPT> ppair;
	for( const auto& elem: v_var )
	{
		try
		{
			ppair = std::visit( fct::PtPairFunct{}, elem );
		}
		catch( const std::exception& err )
		{
			HOMOG2D_LOG_WARNING( "unable to compute point pair\n -msg=" << err.what() );
		}
		vpts.push_back( ppair.first );
		vpts.push_back( ppair.second );
	}
	return FRect_<FPT>( getBB_Points( vpts ) );
}

} // namespace priv

#endif // HOMOG2D_ENABLE_VRTP

//------------------------------------------------------------------
/// Return Bounding Box of primitive or container holding primitives (free function)
/**
tests: [BB-cont]
\ingroup varff
*/
template<typename T>
FRect_<HOMOG2D_INUMTYPE>
getBB( const T& t )
{
	HOMOG2D_START;

	if constexpr( !trait::IsContainer<T>::value ) // if not a container,
		return t.getBB();                         // then call the member function
	else
	{
		if( t.empty() )
			HOMOG2D_THROW_ERROR_1( "unable, can't compute BB of empty container" );

		if constexpr( trait::IsPoint<typename T::value_type>::value )
		{
			if( t.size() < 2 )
				HOMOG2D_THROW_ERROR_1( "unable, need at least two points" );
			return FRect_<typename T::value_type::FType>( priv::getBB_Points( t ) );
		}
		else
		{
			if constexpr( trait::HasBB<typename T::value_type>::value )
			{
				using FPT = typename T::value_type::FType;
				std::vector<FRect_<FPT>> v_bb( t.size() );

				auto it = v_bb.begin();
				for( const auto& elem: t ) // compute bounding box of each element
					*it++ = elem.getBB();

				return priv::getBB_FRect( v_bb ); // compute BB of all the BB
			}
			else
			{
				if constexpr( trait::IsSegment<typename T::value_type>::value )
				{
					if( t.empty()  )
						HOMOG2D_THROW_ERROR_1( "unable, need at least one segment" );
					return priv::getBB_Segments( t );
				}
				else
				{
#ifdef HOMOG2D_ENABLE_VRTP
					if constexpr( !trait::IsVariant<typename T::value_type>::value )
					{
						HOMOG2D_THROW_ERROR_1( "Unable, cannot compute BoundingBox of a container holding Line2d objects" );
					}
					else
						return priv::getBB_CommonType( t );
#else
					HOMOG2D_THROW_ERROR_1( "Unable, cannot compute BoundingBox of a container holding Line2d objects" );
#endif
				}
			}
		}
	}
}

#ifdef HOMOG2D_ENABLE_VRTP

/// Apply homography to primitive
/**
\warning The floating-point type of the returned object (variant) will be the one of the homography \c h,
\b NOT the one of the input element.
\ingroup varff
*/
template<typename T, typename FPT>
CommonType_<FPT>
transform( const Homogr_<FPT>& h, const T& elem )
{
	if constexpr( trait::IsVariant<T>::value )
		return std::visit( fct::TransformFunct<FPT>(h), elem );
	else
		return h * elem;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// SECTION - FREE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

/// Returns a pair of points holding min and max coordinates of the two input pair of points
/**
Tests: search for [minmax]
*/

template<typename T1,typename T2,typename T3,typename T4>
auto
getMinMax( const PointPair2_<T1,T2>& pp1, const PointPair2_<T3,T4>& pp2 )
{
	HOMOG2D_START;

	std::array<Point2d_<HOMOG2D_INUMTYPE>,4> arr;
	arr[0] = pp1.first;
	arr[1] = pp2.first;
	arr[2] = pp1.second;
	arr[3] = pp2.second;

	return priv::getBB_Points( arr );
}

//------------------------------------------------------------------
/// Returns bounding box of two pairs of points
/**
\warning May throw in case of identical coordinates
(for example with [0,0]-[1,0] and [10,0]-[20,0]
*/
template<typename T1,typename T2,typename T3,typename T4>
auto
getBB( const PointPair2_<T1,T2>& pp1, const PointPair2_<T3,T4>& pp2 )
{
	HOMOG2D_START;
	return FRect_<T1>( getMinMax( pp1, pp2 ) );
}

//------------------------------------------------------------------
/// Overload 1/3. This one is called if NONE of the args are a Line2d
template<
	typename T1,
	typename T2,
	typename std::enable_if<(
		   !std::is_same_v<T1,Line2d_<typename T1::FType>>
		&& !std::is_same_v<T2,Line2d_<typename T2::FType>>
		),T1
	>::type* = nullptr
>
auto
getBB( const T1& elem1, const T2& elem2 )
{
	HOMOG2D_START;

	bool isPoly_1 = false;
	if( elem1.type() == Type::OPolyline || elem1.type() == Type::CPolyline )
		isPoly_1 = true;

	bool isPoly_2 = false;
	if( elem2.type() == Type::OPolyline || elem2.type() == Type::CPolyline )
		isPoly_1 = true;

	auto siz_1 = elem1.size();
	auto siz_2 = elem2.size();

	if( isPoly_1 && isPoly_2 )
		if( siz_1==0 && siz_2==0 )
			HOMOG2D_THROW_ERROR_1( "unable to compute bounding box of two empty polyline objects" );

	FRect_<typename T1::FType> out;
// we need to do that in two steps: first, copy return value into `out`, then return
// because the underlying floating point might not be the same for the two arguments.
	if( isPoly_2 && siz_1==0 )
	{
		auto pp = ppair::getPointPair( elem2 );
		if( shareCommonCoord(pp) )
		{
			HOMOG2D_THROW_ERROR_1( "Unable to build common Bounding Box of empty polyline and elem2:" << elem2 );
		}
		else
			return FRect_<typename T1::FType>( pp );
	}
	if( isPoly_1 && siz_2==0 )
	{
		auto pp = ppair::getPointPair( elem1 );
		if( shareCommonCoord(pp) )
		{
			HOMOG2D_THROW_ERROR_1( "Unable to build common Bounding Box of empty polyline and elem1:" << elem1 );
		}
		else
			return FRect_<typename T1::FType>( pp );
	}

	try
	{
		out = getBB(
			ppair::getPointPair( elem1 ),
			ppair::getPointPair( elem2 )
		);
	}
	catch( const std::exception& err )
	{
		HOMOG2D_THROW_ERROR_1( "unable to compute bounding box:\n -arg1="
			<< elem1 << "\n -arg2=" << elem2 << "\n -err=" << err.what() );
	}
	return out;
}

//------------------------------------------------------------------
/// Overload 3/3. Called if one of the args is a Line2d (=> no build!)
template<
	typename T1,
	typename T2,
	typename std::enable_if<
		(std::is_same<T1,Line2d_<typename T1::FType>>::value || std::is_same<T2,Line2d_<typename T2::FType>>::value)
		,T1
	>::type* = nullptr
>
auto
getBB( const T1&, const T2& )
{
	HOMOG2D_START;
	static_assert( detail::AlwaysFalse<T1>::value, "fallback: undefined function" );
	return FRect_<T1>(); // to avoid a compile warning
}

//------------------------------------------------------------------
// forward declaration
namespace priv {
template<typename PLT1,typename FPT1,typename PLT2,typename FPT2>
class ClosestPoints;
}

// forward declaration
template<typename PLT1,typename FPT1,typename PLT2,typename FPT2>
priv::ClosestPoints<PLT1,FPT1,PLT2,FPT2>
getClosestPoints(
	const base::PolylineBase<PLT1,FPT1>& poly1,
	const base::PolylineBase<PLT2,FPT2>& poly2
);

namespace priv {

/// Used in getClosestPoints()
template<typename PLT1,typename FPT1,typename PLT2,typename FPT2>
class ClosestPoints
{
	friend priv::ClosestPoints<PLT1,FPT1,PLT2,FPT2>
	h2d::getClosestPoints<>( const base::PolylineBase<PLT1,FPT1>&,const base::PolylineBase<PLT2,FPT2>& );

private:
	size_t _pt1_min = 0;
	size_t _pt2_min = 0;
	HOMOG2D_INUMTYPE _minDist;
	Point2d_<HOMOG2D_INUMTYPE> _pt1;
	Point2d_<HOMOG2D_INUMTYPE> _pt2;
	const base::PolylineBase<PLT1,FPT1>& _poly1;
	const base::PolylineBase<PLT2,FPT2>& _poly2;

	ClosestPoints( const base::PolylineBase<PLT1,FPT1>& poly1, const base::PolylineBase<PLT2,FPT2>& poly2 )
		: _poly1(poly1), _poly2(poly2)
	{
		_minDist = dist( poly1.getPoint(0), poly2.getPoint(0) );
	}
	void store( HOMOG2D_INUMTYPE d, size_t idx1, size_t idx2 )
	{
		_minDist = d;
		_pt1_min = idx1;
		_pt2_min = idx2;
	}

public:
	PointPair_<HOMOG2D_INUMTYPE>
	getPoints() const
	{
		return std::make_pair(
			_poly1.getPoint(_pt1_min),
			_poly2.getPoint(_pt2_min)
		);
	}
	HOMOG2D_INUMTYPE getMinDist() const
	{
		return _minDist;
	}
	std::pair<size_t,size_t>
	getIndexes() const
	{
		return std::make_pair( _pt1_min, _pt2_min );
	}
	void print() const
	{
		std::cout << "ClosestPoints:\n _pt1_min=" << _pt1_min << " _pt2_min=" << _pt2_min
			<< "\n_minDist=" << _minDist
			<< "\npt1=" << _pt1 << " pt2=" << _pt2
		<< std::endl;
	}
};

} // namespace priv

//------------------------------------------------------------------
/// Computes the closest points between two polylines (types can be different)
template<typename PLT1,typename FPT1,typename PLT2,typename FPT2>
priv::ClosestPoints<PLT1,FPT1,PLT2,FPT2>
getClosestPoints(
	const base::PolylineBase<PLT1,FPT1>& poly1,
	const base::PolylineBase<PLT2,FPT2>& poly2
)
{
#ifndef HOMOG2D_NOCHECKS
	if( poly1.size() == 0 )
		HOMOG2D_THROW_ERROR_1( "arg 1 is empty" );
	if( poly2.size() == 0 )
		HOMOG2D_THROW_ERROR_1( "arg 2 is empty" );
#endif
	priv::ClosestPoints<PLT1,FPT1,PLT2,FPT2> out( poly1, poly2 );
	for( size_t i=0; i<poly1.size(); i++ )
	{
		const auto& pt1 = poly1.getPoint(i);
		for( size_t j=0; j<poly2.size(); j++ )
		{
			const auto& pt2 = poly2.getPoint(j);
			auto currentDist = dist( pt1, pt2 );
			if( currentDist < out._minDist )
				out.store( currentDist, i, j );
		}
	}
	return out;
}

//------------------------------------------------------------------
/// Returns the points of Segment as a std::pair (free function)
/// \sa Segment_::getPts()
template<typename FPT>
auto
getPts( const Segment_<FPT>& seg )
{
	return seg.getPts();
}

/// Returns Segment supporting line (free function)
/// \sa Segment_::getLine()
template<typename FPT>
Line2d_<FPT>
getLine( const Segment_<FPT>& seg )
{
	return seg.getLine();
}

/// Returns Extended Segment (free function)
/// \sa Segment_::geExtended()
template<typename FPT>
Segment_<FPT>
getExtended( const Segment_<FPT>& seg )
{
	return seg.getExtended();
}

/// Returns a pair of segments, corresponding to the input segment split by middle point (free function)
/// \sa Segment_::split()
template<typename SV,typename FPT>
std::pair<base::SegVec<SV,FPT>,base::SegVec<SV,FPT>>
split( const base::SegVec<SV,FPT>& seg )
{
	return seg.split();
}

/// Returns a pair of segments, corresponding to the input segment split by point at distance \c dist (free function)
/// \note: can only be used on oriented segments
/// \sa Segment_::split( T )
template<typename FPT, typename T>
std::pair<OSegment_<FPT>,OSegment_<FPT>>
split( const OSegment_<FPT>& seg, T dist )
{
	HOMOG2D_CHECK_IS_NUMBER(T);
	return seg.split( dist );
}


/// Returns the 4 orthogonal segments to the segment
/// \sa Segment_::getOrthogSegs()
template<typename FPT>
std::array<Segment_<FPT>,4>
getOrthogSegs( const Segment_<FPT>& seg )
{
	return seg.getOrthogSegs();
}

/// Returns the 4 orthogonal points to the segment
/// \sa Segment_::getOrthogPts()
template<typename FPT>
std::array<Point2d_<FPT>,4>
getOrthogPts( const Segment_<FPT>& seg )
{
	return seg.getOrthogPts();
}

//------------------------------------------------------------------
/// Free function, returns segment between two circle centers (or ellipse)
template<typename T1,typename T2>
Segment_<typename T1::FType>
getSegment( const T1& c1, const T2& c2 )
{
	return Segment_<typename T1::FType>( c1.getCenter(), c2.getCenter() );
}

/// Free function, returns line between two circle centers
template<typename FPT1,typename FPT2,typename FPT3>
Line2d_<FPT1>
getLine( const Circle_<FPT2>& c1, const Circle_<FPT3>& c2 )
{
	return Line2d_<FPT1>( c1.center(), c2.center() );
}

/// Free function, returns middle point of segment
/// \sa Segment_::getCenter()
template<typename FPT>
Point2d_<FPT>
getCenter( const Segment_<FPT>& seg )
{
	return seg.getCenter();
}

/// Free function, returns bisector line of segment
/// \sa Segment_::getBisector()
template<typename FPT>
Line2d_<FPT>
getBisector( const Segment_<FPT>& seg )
{
	return seg.getBisector();
}

/// Free function, returns a pair of parallel segments at a distance \c dist
/// \sa Segment_::getBisector()
template<typename FPT,typename T>
std::pair<Segment_<FPT>,Segment_<FPT>>
getParallelSegs( const Segment_<FPT>& seg, T dist )
{
	return seg.getParallelSegs(dist);
}

/// Free function, returns middle points of set of segments/circles
/**
\sa Segment_::getCenter()
\sa Circle_::getCenter()
- input: set of segments
- output: set of points (same container)
*/
template<
	typename T,
	typename std::enable_if<
		trait::IsContainer<T>::value,
		T
	>::type* = nullptr
>
auto
getCenters( const T& vsegs )
{
	std::vector<Point2d_<typename T::value_type::FType>> vout( vsegs.size() );

	auto it = std::begin( vout );
	for( const auto& seg: vsegs )
		*it++ = getCenter( seg );
	return vout;
}

/// Free function, returns a set of lines from a set of segments
/**
\sa Segment_::getLine()
*/
template<
	typename T,
	typename std::enable_if<
		trait::IsContainer<T>::value,
		T
	>::type* = nullptr
>
auto
getLines( const T& vsegs )
{
	std::vector<Line2d_<typename T::value_type::FType>> vout( vsegs.size() );

	auto it = std::begin( vout );
	for( const auto& seg: vsegs )
		*it++ = seg.getLine();
	return vout;
}

/// Free function, returns segments of the rectangle
/// \sa FRect_::getSegs()
template<typename FPT>
std::array<Segment_<FPT>,4>
getSegs( const FRect_<FPT>& seg )
{
	return seg.getSegs();
}

/// Free function, returns the pair of segments tangential to the two circles
/** Sources:
  - https://math.stackexchange.com/questions/719758/
  - https://math.stackexchange.com/a/211854/133647
  - https://en.wikipedia.org/wiki/Tangent_lines_to_circles
  - https://gieseanw.wordpress.com/2012/09/12/finding-external-tangent-points-for-two-circles/
*/
template<typename FPT1,typename FPT2>
std::pair<Segment_<FPT1>,Segment_<FPT1>>
getTanSegs( const Circle_<FPT1>& c1, const Circle_<FPT2>& c2 )
{
#ifndef HOMOG2D_NOCHECKS
	if( c1 == c2 )
		HOMOG2D_THROW_ERROR_1( "c1 and c2 identical" );
#endif

// if same radius, return the two segments parallel to the one joining the centers
	if( homog2d_abs( c1.radius() - c2.radius() ) < thr::nullDistance() )
	{
		Segment_<HOMOG2D_INUMTYPE> seg_center( c1.center(), c2.center() );
		return seg_center.getParallelSegs( c1.radius() );
	}

// check which one is the smallest
	Circle_<HOMOG2D_INUMTYPE> cA = c1;
	Circle_<HOMOG2D_INUMTYPE> cB = c2;
	if( c1.radius() < c2.radius() )
		std::swap( cA, cB );

	auto h = dist( cA.center(), cB.center() );

	auto theta = homog2d_asin( ( cA.radius() - cB.radius() ) / h ) ;
	auto hcost = h * homog2d_cos(theta);

// get rotated lines at center of CB
	auto l0 = cA.center() * cB.center();
	auto l1 = l0.getRotatedLine( cB.center(),  theta );
	auto l2 = l0.getRotatedLine( cB.center(), -theta );

// build segments by getting the opposite point

	auto ppts1 = l1.getPoints( cB.center(), hcost );
	auto p1 = ppts1.first;
	if( ppts1.second.distTo( cA.center()) < p1.distTo( cA.center()) )
		p1 = ppts1.second;

	auto ppts2 = l2.getPoints( cB.center(), hcost );
	auto p2 = ppts2.first;
	if( ppts2.second.distTo( cA.center()) < p2.distTo( cA.center()) )
		p2 = ppts2.second;
//	std::cout <<"p1=" << p1 << " p2=" << p2 << '\n';

	auto seg1 = Segment_<HOMOG2D_INUMTYPE>( p1, cB.center() );
	auto seg2 = Segment_<HOMOG2D_INUMTYPE>( p2, cB.center() );

	auto psegs1 = seg1.getParallelSegs( cB.radius() );
	if( psegs1.first.distTo(cA.center()) <  psegs1.second.distTo(cA.center()) )
		std::swap( psegs1.first, psegs1.second );

	auto psegs2 = seg2.getParallelSegs( cB.radius() );
	if( psegs2.second.distTo(cA.center()) <  psegs2.first.distTo(cA.center()) )
		std::swap( psegs2.first, psegs2.second );

	return std::make_pair(
		psegs1.first,
		psegs2.second
	);
}

/// Returns the 4 points of the rectangle (free function)
/// \sa FRect_::get4Pts()
template<typename FPT>
std::array<Point2d_<FPT>,4>
get4Pts( const FRect_<FPT>& rect )
{
	return rect.get4Pts();
}

/// Returns the 2 major points of the rectangle (free function)
/// \sa FRect_::getPts()
template<typename FPT>
auto
getPts( const FRect_<FPT>& rect )
{
	return rect.getPts();
}

/// Free function
template<typename FPT>
HOMOG2D_INUMTYPE
height( const FRect_<FPT>& rect )
{
	return rect.height();
}

/// Free function
template<typename FPT>
HOMOG2D_INUMTYPE
width( const FRect_<FPT>& rect )
{
	return rect.width();
}

/// Free function
template<typename FPT>
FRect_<FPT>
getExtended( const FRect_<FPT>& rect )
{
	return rect.getExtended();
}

/// Free function
template<typename FPT>
std::pair<Segment_<FPT>,Segment_<FPT>>
getDiagonals( const FRect_<FPT>& rect )
{
	return rect.getDiagonals();
}

/// Returns centroid of Polyline (free function)
/// \sa PolylineBase::centroid()
template<typename PLT,typename FPT>
base::LPBase<typ::IsPoint,FPT>
centroid( const base::PolylineBase<PLT,FPT>& pl )
{
	return pl.centroid();
}

/// Returns reference on radius of circle (free function), non-const version
/// \sa Circle_::radius()
template<typename FPT>
FPT&
radius( Circle_<FPT>& cir )
{
	return cir.radius();
}

/// Returns reference on radius of circle (free function), const version
/// \sa Circle_::radius()
template<typename FPT>
const FPT&
radius( const Circle_<FPT>& cir )
{
	return cir.radius();
}

/// Returns reference on center of circle (free function), non-const version
/// \sa Circle_::center()
template<typename FPT>
Point2d_<FPT>&
center( Circle_<FPT>& cir )
{
	return cir.center();
}

/// Returns reference on center of circle (free function), const version
/// \sa Circle_::center()
template<typename FPT>
const Point2d_<FPT>&
center( const Circle_<FPT>& cir )
{
	return cir.center();
}

/// Get data size expressed as number of bits for, respectively, mantissa and exponent
/// \sa detail::Common::dsize()
template<typename T>
std::pair<int,int>
dsize( const T& t )
{
	return t.dsize();
}

//------------------------------------------------------------------
/// Returns the distance between 2 parallel lines (free function)
/**
- ref: https://en.wikipedia.org/wiki/Distance_between_two_parallel_lines

We first check that the two lines are indeed parallel.
This should ensure that a1=a2 and b1=b2.
But due to numeric issues they could by slightly different. In order to gain accuracy,
we use the geometric mean of these.
*/
template<typename FPT>
HOMOG2D_INUMTYPE
getParallelDistance( const Line2d_<FPT>& li1, const Line2d_<FPT>& li2 )
{
#ifndef HOMOG2D_NOCHECKS
	if( !li1.isParallelTo(li2) )
		HOMOG2D_THROW_ERROR_1( "lines are not parallel" );
#endif
	const auto ar1 = li1.get();
	const auto ar2 = li2.get();
	const HOMOG2D_INUMTYPE a1 = ar1[0];
	const HOMOG2D_INUMTYPE b1 = ar1[1];
	const HOMOG2D_INUMTYPE c1 = ar1[2];
	const HOMOG2D_INUMTYPE a2 = ar2[0];
	const HOMOG2D_INUMTYPE b2 = ar2[1];
	const HOMOG2D_INUMTYPE c2 = ar2[2];

	HOMOG2D_INUMTYPE a = homog2d_sqrt( a1*a2 );
	HOMOG2D_INUMTYPE b = homog2d_sqrt( b1*b2 );
	return homog2d_abs( c1 - c2 ) / homog2d_sqrt( a*a + b*b );
}

/// Return angle of ellipse (free function)
/// \sa Ellipse_::angle()
template<typename FPT>
HOMOG2D_INUMTYPE
angle( const Ellipse_<FPT>& ell )
{
	return ell.angle();
}

/// Return center of Segment_, FRect_, or Ellipse_ (free function)
/// \sa Ellipse_::getCenter()
/// \sa FRect_::getCenter()
/// \sa Segment_::getCenter()
template<typename T>
Point2d_<typename T::FType>
getCenter(const T& other )
{
	return other.getCenter();
}

/// Translate primitive \c prim (free function)
/**
Calls the member function. Type checking is done there.
*/
template<typename T,typename FP1,typename FP2>
void
translate( T& prim, FP1 dx, FP2 dy )
{
	prim.translate( dx, dy );
}

/// Translate primitive \c prim with values in a \c std::pair (free function)
/**
Calls the member function. Type checking is done there.
*/
template<typename T,typename FP1,typename FP2>
void
translate( T& prim, const std::pair<FP1,FP2>& ppt )
{
	prim.translate( ppt.first, ppt.second );
}

/// Move primitive to other location (free function)
/**
Calls the member function. Type checking is done there.
*/
template<typename T,typename FP>
void
moveTo( T& prim, const Point2d_<FP>& pt )
{
	prim.moveTo( pt );
}

/// Move primitive to other location (free function)
/**
Calls the member function. Type checking is done there.
*/
	template<typename T,typename FP1,typename FP2>
	void moveTo( T& prim, FP1 x, FP2 y )
	{
		prim.moveTo( x, y );
	}

/// Returns true if ellipse is a circle
/// \sa Ellipse_::isCircle()
template<typename FPT>
bool
isCircle( const Ellipse_<FPT>& ell, HOMOG2D_INUMTYPE thres=1.E-10 )
{
	return ell.isCircle( thres );
}

/// Returns ellipse axis lines
/// \sa Ellipse_::getAxisLines()
template<typename FPT>
std::pair<Line2d_<FPT>,Line2d_<FPT>>
getAxisLines( const Ellipse_<FPT>& ell )
{
	return ell.getAxisLines();
}


namespace priv {

/// used in findPoint()
struct F_MIN{};
struct F_MAX{};

/// Private. Common function for searching nearest of farthest point
/*
\sa findNearestPoint()
\sa findFarthestPoint
*/
template<typename FPT, typename CONT, typename S_WHAT>
size_t
findPoint(
	const Point2d_<FPT>& qpt,  ///< query point
	const CONT&          cont, ///< container
	const S_WHAT&              ///< F_MIN or F_MAX
//	const CONT2*         mask  ///< optional vector or list or array of bool
)
{
	if( cont.size() < 2 )
		HOMOG2D_THROW_ERROR_1( "container holds " << cont.size() \
			<< " points, minimum is 2" );

	decltype( priv::sqDist( qpt, qpt ) ) resDist;
	size_t startIdx = 1;
	size_t resIdx = 0;

// if point is the first one of the container, then
// initialize with the second one
	if( cont[0] == qpt )
	{
		startIdx++;
		resIdx++;
		resDist = priv::sqDist( qpt, cont[1] );
	}
	else // initialize with first point
	{
		resDist = priv::sqDist( qpt, cont[0] );
	}

	for( size_t i=startIdx; i<cont.size(); i++ )
	{
		if( qpt != cont[i] )
		{
			auto currentDist = priv::sqDist( qpt, cont[i] );
			if constexpr( std::is_same_v<S_WHAT, F_MIN> )
			{
				if( currentDist < resDist )
				{
					resIdx  = i;
					resDist = currentDist;
				}
			}
			else
			{
				if( currentDist > resDist )
				{
					resIdx  = i;
					resDist = currentDist;
				}
			}
		}
	}
	return resIdx;
}

} //namespace priv

//------------------------------------------------------------------
/// Returns index of point in container \c cont that is the nearest to \c pt
/// \todo add some sfinae and/or checking on type T
/// \todo perform some speed analysis, and check if usage of squared distance is really better
/// \todo add other distance computations (Manhattan?)
/// \todo Would it be better to return an iterator?
template<typename FPT, typename T>
size_t
findNearestPoint( const Point2d_<FPT>& pt, const T& cont )
{
	return priv::findPoint( pt, cont, priv::F_MIN() );
}
//------------------------------------------------------------------
/// Returns index of point in container \c cont that is the farthest to \c pt
template<typename FPT, typename T>
size_t
findFarthestPoint( const Point2d_<FPT>& pt, const T& cont )
{
	return priv::findPoint( pt, cont, priv::F_MAX() );
}
//------------------------------------------------------------------
/// Returns indexes of points in container \c cont that are nearest/farthest
/**
- return pair: first is nearest, second is farthest
*/
template<typename FPT, typename T>
auto
findNearestFarthestPoint( const Point2d_<FPT>& pt, const T& cont )
{
	if( cont.size() < 2 )
		HOMOG2D_THROW_ERROR_1( "container holds " << cont.size() \
			<< " points, minimum is 2" );

	auto maxDist = priv::sqDist( pt, cont[0] );
	auto minDist = maxDist;
	size_t idxMin = 0;
	size_t idxMax = 0;
	size_t startIdx = 1;
	if( pt == cont[0] )
	{
		idxMin++;
		idxMax++;
		startIdx++;
		maxDist = priv::sqDist( pt, cont[1] );
		minDist = maxDist;
	}
	for( size_t i=startIdx; i<cont.size(); i++ )
	{
		if( pt != cont[i] )
		{
			auto currentDist = priv::sqDist( pt, cont[i] );
			if( currentDist > maxDist )
			{
				idxMax  = i;
				maxDist = currentDist;
			}
			if( currentDist < minDist )
			{
				idxMin  = i;
				minDist = currentDist;
			}
		}
	}
	return std::make_pair(idxMin, idxMax);
}

/// Returns set of points that are inside primitive \c prim
/**
Output container will be of same type as input container (vector, list or array

\todo 20250123: maybe replace the bunch of static_asserts with something around trait::HasArea ?
*/
template<typename CONT, typename PRIM>
CONT
getPtsInside(
	const CONT& input_set,
	const PRIM& prim        ///< geometrical primitive (FRect, Circle, ...?)
)
{
	static_assert( !std::is_same_v<typename PRIM::SType, typ::T_OPol>,    "Cannot find points inside a Open Polyline" );
	static_assert( !std::is_same_v<typename PRIM::SType, typ::T_Line>,    "Cannot find points inside a Line" );
	static_assert( !std::is_same_v<typename PRIM::SType, typ::T_Point>,   "Cannot find points inside a Point" );
	static_assert( !std::is_same_v<typename PRIM::SType, typ::T_Segment>, "Cannot find points inside a Segment" );
	static_assert( !trait::IsArray<CONT>::value, "Cannot use std::array as container" );

	CONT out;
	out.reserve( input_set.size() );

	for( auto pt: input_set )
		if( pt.isInside( prim ) )
			out.push_back( pt );
	return out;
}

/////////////////////////////////////////////////////////////////////////////
// SECTION  - GENERIC DRAWING FREE FUNCTIONS (BACK-END INDEPENDENT)
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
/// Free function, draws any of the primitives by calling the relevant member function
/**
\todo 20240504: see note on the IsDrawable trait class
*/
template<
	typename U,
	typename Prim,
	typename std::enable_if<
		trait::IsDrawable<Prim>::value,
		Prim
	>::type* = nullptr
>
void
draw( img::Image<U>& img, const Prim& prim, const img::DrawParams& dp=img::DrawParams() )
{
	prim.draw( img, dp );
}

/// Free function, draws text \c str at position \c pt
template<typename U,typename FPT>
void
drawText( img::Image<U>& im, std::string str, Point2d_<FPT> pt, img::DrawParams dp=img::DrawParams() )
{
	im.drawText( str, pt, dp );
}

namespace priv {

/// Draw indexes for points
template<typename IMG,typename FPT>
void
impl_drawIndexes( img::Image<IMG>& img, size_t c, const img::DrawParams& dp, const Point2d_<FPT>& pt )
{
	if( dp._dpValues._showIndex )
		drawText( img, std::to_string(c), pt, dp );
}

/// Draw indexes for segment
template<typename IMG,typename FPT>
void
impl_drawIndexes( img::Image<IMG>& img, size_t c, const img::DrawParams& dp, const Segment_<FPT>& seg )
{
	if( dp._dpValues._showIndex )
		drawText( img, std::to_string(c), seg.getCenter(), dp );
}

/// Default signature, will be instanciated if no other fits (and does nothing)
template<typename IMG,typename DUMMY>
void
impl_drawIndexes( img::Image<IMG>&, size_t, const img::DrawParams&, const DUMMY& )
{}

} // namespace priv

/// Free function, draws a set of primitives
/**
- Type \c T can be \c std::array<type> or \c std::vector<type>, with \c type being anything drawable.
- The types inside the container can be either plain \c h2d types (\c FRect, \c Segment, ...) of variant types,
using the \c CommonType class (requires HOMOG2D_ENABLE_VRTP symbol).

\ingroup varff
*/
template<
	typename U,
	typename T,
	typename std::enable_if<
		trait::IsContainer<T>::value,
		T
	>::type* = nullptr
>
void
draw( img::Image<U>& img, const T& cont, const img::DrawParams& dp=img::DrawParams() )
{
#ifdef HOMOG2D_ENABLE_VRTP
	if constexpr( trait::IsVariant<typename T::value_type>::value )
	{
		fct::DrawFunct vde( img, dp ); // functor
		for( auto& e: cont )
			std::visit( vde, e );
	}
	else
#endif
	{
		size_t c=0;
		for( const auto& elem: cont )
		{
			elem.draw( img, dp );
			priv::impl_drawIndexes( img, c++, dp, elem );
		}
	}
}

/// This version holds a \c std::function as 3th parameter. It can be used to pass a function
/// that will return a different img::DrawParams for a given index of the container.
/**
At present, cannot be used with container holding variants...
*/
template<
	typename U,
	typename T,
	typename std::enable_if<
		trait::IsContainer<T>::value,
		T
	>::type* = nullptr
>
void
draw( img::Image<U>& img, const T& cont, std::function<img::DrawParams(int)>& func )
{
	size_t c=0;
	for( const auto& elem: cont )
	{
		auto dp = func(c);
		elem.draw( img, dp );
		priv::impl_drawIndexes( img, c++, dp, elem );
	}
}

//------------------------------------------------------------------
/// Free function, draws a pair of objects
/**
Type \c T1 and \c T2 can be anything drawable
*/
template<typename T1,typename T2,typename U>
void draw(
	img::Image<U>&          img,
	const std::pair<T1,T2>& pa,
	const img::DrawParams&  dp=img::DrawParams() )
{
	pa.first.draw(  img, dp );
	pa.second.draw( img, dp );
}

namespace detail {

/// Private helper function, used by LPBase<IsPoint>::draw(). Draw point on image.
template<typename T>
void
drawPt(
	img::Image<T>&               img,
	img::PtStyle                 ps,
	std::vector<Point2d_<float>> vpt,
	const img::DrawParams&       dp,
	bool                         drawDiag=false
)
{
	auto delta  = dp._dpValues._ptDelta;
	auto delta2 = std::round( 0.7 * delta);
	switch( ps )
	{
		case img::PtStyle::Times:
		case img::PtStyle::Squ:
			vpt[0].translate( -delta2, +delta2 );
			vpt[1].translate( +delta2, -delta2 );
			vpt[2].translate( +delta2, +delta2 );
			vpt[3].translate( -delta2, -delta2 );
		break;

		case img::PtStyle::Plus:
		case img::PtStyle::Diam:
			vpt[0].translate( -delta, 0.      );
			vpt[1].translate( +delta, 0.      );
			vpt[2].translate( 0.,      -delta );
			vpt[3].translate( 0.,      +delta );
		break;
		default: assert(0);
	}
	auto dp2(dp);          // we need to do this, because this is called by the
	dp2.showPoints(false); //  segment drawing function. If not, infinite recursion
	if( !drawDiag )
	{
		if( ps == img::PtStyle::Squ )
		{
			std::vector<Segment_<float>> vseg{
				{vpt[0], vpt[1]},
				{vpt[2], vpt[1]},
				{vpt[2], vpt[3]},
				{vpt[0], vpt[3]}
			};
			for( const auto& s:vseg )
				s.draw( img, dp2 );
		}
		else
		{
			Segment_<float> s1( vpt[0], vpt[1] );
			Segment_<float> s2( vpt[2], vpt[3] );
			s1.draw( img, dp2 );
			s2.draw( img, dp2 );
		}
	}
	else // draw 4 diagonal lines
	{
		Segment_<float>( vpt[0], vpt[2] ).draw( img, dp2 );
		Segment_<float>( vpt[2], vpt[1] ).draw( img, dp2 );
		Segment_<float>( vpt[1], vpt[3] ).draw( img, dp2 );
		Segment_<float>( vpt[0], vpt[3] ).draw( img, dp2 );
	}
}

} // namespace detail

//------------------------------------------------------------------
namespace priv {
/// Holds convex hull code
namespace chull {

//------------------------------------------------------------------
/// Used int the convex hull algorithm
template<typename FPT>
size_t
getPivotPoint( const std::vector<Point2d_<FPT>>& in )
{
	auto pmin = h2d::priv::getBmPoint_helper( in );
	return static_cast<size_t>( pmin - in.begin() );
}

//------------------------------------------------------------------
/// Sorts points by angle between the lines with horizontal axis
template<typename FPT>
std::vector<size_t>
sortPoints( const std::vector<Point2d_<FPT>>& in, size_t piv_idx )
{
	assert( in.size()>3 );

// step 1: create new vector holding the indexes of the points, including the pivot point (will be in first position)
	std::vector<size_t> out( in.size() );
	std::iota( out.begin(), out.end(), 0 ); // fill vector: [0,1,2,...]
	std::swap( out[piv_idx], out[0] );
	auto pt0 = in[piv_idx];

// step 2: sort points by angle of lines between the current point and pivot point
	std::sort(
		out.begin()+1,
		out.end(),
		[&]                  // lambda
		( size_t i1, size_t i2 )
		{
			auto pt1 = in[i1];
			auto pt2 = in[i2];
			auto dx1 = pt1.getX() - pt0.getX();
			auto dy1 = pt1.getY() - pt0.getY();
			auto dx2 = pt2.getX() - pt0.getX();
			auto dy2 = pt2.getY() - pt0.getY();
			return ((dx1 * dy2 - dx2 * dy1) > 0);
		}
	);
	return out;
}

//------------------------------------------------------------------
/// To find orientation of ordered triplet of points (p, q, r).
/** The function returns following values
- 0 --> p, q and r are colinear
- 1 --> Clockwise
- 2 --> Counterclockwise

\todo 20240326: this is subject to numerical instability, as it is based on differences.

\todo 20230212: replace const value HOMOG2D_THR_ZERO_DETER with related static function
*/
template<typename T>
int orientation( Point2d_<T> p, Point2d_<T> q, Point2d_<T> r )
{
	HOMOG2D_INUMTYPE px = p.getX();
	HOMOG2D_INUMTYPE py = p.getY();
	HOMOG2D_INUMTYPE qx = q.getX();
	HOMOG2D_INUMTYPE qy = q.getY();
	HOMOG2D_INUMTYPE rx = r.getX();
	HOMOG2D_INUMTYPE ry = r.getY();

	auto val = (qy - py) * (rx - qx) - (qx - px) * (ry - qy);

	if( homog2d_abs(val) < HOMOG2D_THR_ZERO_DETER )
		return 0;  // collinear
    return (val > 0 ? 1 : -1 ); // clock or counterclock wise
}
//------------------------------------------------------------------
/// Inherits std::stack<> and adds a member function to fetch the underlying std::vector.
/// Used in h2d::convexHull()
struct Mystack : std::stack<size_t,std::vector<size_t>>
{
	const std::vector<size_t>& getVect() const
	{
		return this->c;
	}
};
//------------------------------------------------------------------
} // namespace chull
} // namespace priv

//------------------------------------------------------------------
/// Compute Convex Hull of a Polyline (free function)
/**
- type \c T: can be either OPolyline, CPolyline, or std::vector<Point2d>
- Graham scan algorithm: https://en.wikipedia.org/wiki/Graham_scan
*/
template<typename CT,typename FPT>
CPolyline_<FPT>
convexHull( const base::PolylineBase<CT,FPT>& input )
{
	return convexHull( input.getPts() );
}

namespace base {
//------------------------------------------------------------------
/// Computes and returns the convex hull of a set of points (free function)
/**
- Graham scan algorithm: https://en.wikipedia.org/wiki/Graham_scan

\todo 20230728: make this function accept also std::array and std::list (using Sfinae alogn with trait::IsContainer)
*/
template<typename FPT>
CPolyline_<FPT>
convexHull( const std::vector<Point2d_<FPT>>& input )
{
	if( input.size() < 4 )  // if 3 pts or less, then the hull is equal to input set
		return CPolyline_<FPT>( input );

// step 1: find pivot (point with smallest Y coord)
	auto pivot_idx = priv::chull::getPivotPoint( input );

// step 2: sort points by angle of lines between the current point and pivot point
	auto v2 = priv::chull::sortPoints( input, pivot_idx );

	std::stack<size_t> firstPoint;
	priv::chull::Mystack hull;
	hull.push( v2[0] );
	hull.push( v2[1] );
	hull.push( v2[2] );

	firstPoint.push( 0 );
	firstPoint.push( 1 );

// step 3: iterate
	size_t idx1 = 1;
	size_t idx2 = 2;
	size_t idx3 = 3;
	do
	{
//		HOMOG2D_LOG( "** loop start, idx1=" << idx1 << ", idx2=" << idx2  << ", idx3=" << idx3 << ", hull " << hull.getVect() );
		auto p = input.at( v2[idx1] );
		auto q = input.at( v2[idx2] );
		auto r = input.at( v2[idx3] );
		auto orient = priv::chull::orientation( p, q, r );
//		HOMOG2D_LOG( "considering pts: " << v2[idx1] << "," << v2[idx2] << "," << v2[idx3] << ": or = " << orient );
		if( orient != 1 )
		{
			hull.push( v2[idx3] );
			idx1 = idx2;
			idx2 = idx3;
			idx3++;
			firstPoint.push( idx1 );
		}
		else
		{
			hull.pop();
			idx2=idx1; // idx3 stays the same
			firstPoint.pop();
			idx1 = firstPoint.top();
		}
	}
	while( idx3 < v2.size() );

// final step: copy hull indexes to vector of points
	const auto v = hull.getVect();
	std::vector<Point2d_<FPT>> vout( v.size() );
	std::transform(
		v.begin(),
		v.end(),
		vout.begin(),
		[&input]               // lambda
		(size_t idx)
		{
			return input.at( idx );
		}
	);

	return CPolyline_<FPT>( vout );
}

} // namespace base

/// Return convex hull (member function implementation)
template<typename CT,typename FPT>
CPolyline_<FPT>
base::PolylineBase<CT,FPT>::convexHull() const
{
	return h2d::convexHull( *this );
}

/////////////////////////////////////////////////////////////////////////////
// SECTION  - OPENCV BINDING - GENERAL
/////////////////////////////////////////////////////////////////////////////

#ifdef HOMOG2D_USE_OPENCV

//------------------------------------------------------------------
/// Copy matrix to Opencv \c cv::Mat
/**
The output matrix is passed by reference to avoid issues with Opencv copy operator, and is allocated here.

User can pass a type as second argument: CV_32F for \c float, CV_64F for \c double (default)
*/
template<typename W,typename FPT>
void
Hmatrix_<W,FPT>::copyTo( cv::Mat& mat, int type ) const
{
	auto& data = detail::Matrix_<FPT>::_mdata;
#ifndef HOMOG2D_NOCHECKS
	if( type != CV_64F && type != CV_32F )
		throw std::runtime_error( "invalid OpenCv matrix type" );
#endif
	mat.create( 3, 3, type ); // default:CV_64F
	size_t i=0;
	switch( type )
	{
		case CV_64F:
			for( auto it = mat.begin<double>(); it != mat.end<double>(); it++, i++ )
				*it = data[i/3][i%3];
			break;
		case CV_32F:
			for( auto it = mat.begin<float>(); it != mat.end<float>(); it++, i++ )
				*it = data[i/3][i%3];
			break;
		default: assert(0);
	}
}
//------------------------------------------------------------------
/// Get homography from Opencv \c cv::Mat
template<typename W,typename FPT>
Hmatrix_<W,FPT>&
Hmatrix_<W,FPT>::operator = ( const cv::Mat& mat )
{
	auto type = mat.type();
#ifndef HOMOG2D_NOCHECKS
	if( mat.rows != 3 || mat.cols != 3 )
		throw std::runtime_error( "invalid matrix size, rows=" + std::to_string(mat.rows) + " cols=" + std::to_string(mat.cols) );
	if( mat.channels() != 1 )
		throw std::runtime_error( "invalid matrix nb channels: " + std::to_string(mat.channels() ) );
	if( type != CV_64F && type != CV_32F )
		throw std::runtime_error( "invalid matrix type" );
#endif
	size_t i=0;

	auto& data = detail::Matrix_<FPT>::_mdata;
	switch( type )
	{
		case CV_64F:
			for( auto it = mat.begin<double>(); it != mat.end<double>(); it++, i++ )
				data[i/3][i%3] = *it;
			break;
		case CV_32F:
			for( auto it = mat.begin<float>(); it != mat.end<float>(); it++, i++ )
				data[i/3][i%3] = *it;
			break;
		default: assert(0);
	}
	return *this;
}
#endif // HOMOG2D_USE_OPENCV

/////////////////////////////////////////////////////////////////////////////
// SECTION .1 - CLASS DRAWING MEMBER FUNCTIONS (backend-agnostic)
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
/// Draw Line2d on image, backend independent
/**
Steps:
 -# find the intersection points between the line and the image rectangle, should find 2. (but 1 is possible)
 -# draw a line between these 2 points
*/
template<typename LP, typename FPT>
template<typename T>
void
base::LPBase<LP,FPT>::impl_draw_LP(
	img::Image<T>&  im,
	img::DrawParams dp,
	const detail::BaseHelper<typename typ::IsLine>&
) const
{
	assert( im.rows() > 2 );
	assert( im.cols() > 2 );

	Point2d_<FPT> pt1; // 0,0
	Point2d_<FPT> pt2( im.cols()-1, im.rows()-1 );

	auto ri = this->intersects( pt1,  pt2 );
	if( ri() )
	{
		if( ri.size() == 2 ) // if only one intersection point, do nothing
		{
			auto ppts = ri.get();
			h2d::Segment_<HOMOG2D_INUMTYPE> seg( ppts[0], ppts[1] );
			dp.showPoints(false);
			seg.draw( im, dp );
		}
	}
}

//------------------------------------------------------------------
/// Draw points on image implementation, backend independent.
/// Returns false if point not in image
template<typename LP, typename FPT>
template<typename T>
void
base::LPBase<LP,FPT>::impl_draw_LP(
	img::Image<T>&  im,
	img::DrawParams dp,
	const detail::BaseHelper<typename typ::IsPoint>& ///< dummy arg
) const
{
	if( getX()<0 || getX()>=im.cols() )      // check if point is in image
		return;
	if( getY()<0 || getY()>=im.rows() )
		return;

	std::vector<Point2d_<float>> vpt( 4, *this );
	switch( dp._dpValues._ptStyle )
	{
		case img::PtStyle::Dot:
			Circle_<float>( *this, dp._dpValues._pointSize ).draw( im, dp );
		break;

		case img::PtStyle::Plus:   // "+" symbol
			detail::drawPt( im, img::PtStyle::Plus, vpt, dp );
		break;

		case img::PtStyle::Star:
			detail::drawPt( im, img::PtStyle::Plus,  vpt, dp );
			detail::drawPt( im, img::PtStyle::Times, vpt, dp );
		break;

		case img::PtStyle::Diam:
			detail::drawPt( im, img::PtStyle::Plus, vpt, dp, true );
		break;

		case img::PtStyle::Squ:
			detail::drawPt( im, img::PtStyle::Squ, vpt, dp, true );
		break;

		case img::PtStyle::Times:      // "times" symbol
			detail::drawPt( im, img::PtStyle::Times, vpt, dp );
		break;

		default: assert(0);
	}
}

//------------------------------------------------------------------
/// Helper function used by the draw(OSegment) function, returns the 3 segment corresponding
/// to the "arrows" as 3 pairs of points
/**
Used both in the SVG and the Opencv backends

\verbatim
   /|\
    |
    |
    |
   ---
\endverbatim
*/
namespace priv {

template<typename FPT>
std::array<PointPair_<double>,3>
getArrowSegments(
	const base::SegVec<typ::IsOSeg,FPT>& vec
)
{
	std::array<std::pair<Point2d_<double>,Point2d_<double>>,3> out;
	auto ppts = vec.getPts();
	auto pt1 = ppts.first;
	auto pt2 = ppts.second;

	const int arSize = 8;

	Line2d_<double> liA = vec.getLine().getOrthogLine( pt1 );
	out[0] = liA.getPoints( pt1, arSize );

	auto ppts_B = vec.getLine().getPoints( pt2, arSize );

	Point2d_<double> p0 = ppts_B.first;
	if( dist( pt1, ppts_B.first) >  dist(pt1, ppts_B.second) )
		p0 = ppts_B.second;

	Line2d_<double> liB = vec.getLine().getOrthogLine( p0 );
	auto ppts_C = liB.getPoints( p0, arSize );

	out[1] = std::make_pair(ppts_C.first, pt2);
	out[2] = std::make_pair(ppts_C.second, pt2);

	return out;
}

} // namespace priv

/////////////////////////////////////////////////////////////////////////////
// SECTION .2 - CLASS DRAWING MEMBER FUNCTIONS (OpenCv)
/////////////////////////////////////////////////////////////////////////////

#ifdef HOMOG2D_USE_OPENCV
//------------------------------------------------------------------
/// Draw \c FRect (Opencv implementation)
template<typename FPT>
void
FRect_<FPT>::draw( img::Image<cv::Mat>& im, img::DrawParams dp ) const
{
	cv::rectangle(
		im.getReal(),
		_ptR1.getCvPti(),
		_ptR2.getCvPti(),
		dp.cvColor(),
		dp._dpValues._lineThickness,
		dp._dpValues._lineType==1?cv::LINE_AA:cv::LINE_8
	);
	if( dp._dpValues._showPoints )
	{
		_ptR1.draw( im, dp );
		_ptR2.draw( im, dp );
	}
}

//------------------------------------------------------------------
/// Draw \c Segment / \c OSegment (Opencv implementation)
namespace base {

template<typename SV,typename FPT>
void
SegVec<SV,FPT>::draw( img::Image<cv::Mat>& im, img::DrawParams dp ) const
{
	cv::line(
		im.getReal(),
		_ptS1.getCvPtd(),
		_ptS2.getCvPtd(),
		dp.cvColor(),
		dp._dpValues._lineThickness,
		dp._dpValues._lineType==1?cv::LINE_AA:cv::LINE_8
	);

	if constexpr( std::is_same_v<SV,typ::IsOSeg> )
	{
		auto arrsegs = priv::getArrowSegments( *this );
		for( auto ppts: arrsegs )
			cv::line(
				im.getReal(),
				ppts.first.getCvPtd(),
				ppts.second.getCvPtd(),
				dp.cvColor(),
				dp._dpValues._lineThickness,
				dp._dpValues._lineType==1?cv::LINE_AA:cv::LINE_8
			);
	}
	else
	{
		if( dp._dpValues._showPoints )
		{
			_ptS1.draw( im, dp );
			_ptS2.draw( im, dp );
		}
	}
}

} // namespace base
//------------------------------------------------------------------
/// Draw \c Circle (Opencv implementation)
template<typename FPT>
void
Circle_<FPT>::draw( img::Image<cv::Mat>& im, img::DrawParams dp ) const
{
	cv::circle(
		im.getReal(),
		_center.getCvPti(),
		static_cast<int>(_radius),
		dp.cvColor(),
		dp._dpValues._lineThickness,
		dp._dpValues._lineType==1?cv::LINE_AA:cv::LINE_8
	);
	if( dp._dpValues._showPoints )
 // WARNING: can't use the "Dot" style here, because it call this function, so infinite recursion
		_center.draw( im, dp.setPointStyle( img::PtStyle::Plus) );
}

//------------------------------------------------------------------
/// Draw \c Ellipse (Opencv implementation)
/**
- see https://docs.opencv.org/3.4/d6/d6e/group__imgproc__draw.html#ga28b2267d35786f5f890ca167236cbc69
*/
template<typename FPT>
void
Ellipse_<FPT>::draw( img::Image<cv::Mat>& im, img::DrawParams dp )  const
{
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	cv::ellipse(
		im.getReal(),
		cv::Point( par.x0,par.y0 ),
		cv::Size( par.a, par.b ),
		par.theta*180./M_PI,
		0., 360.,
		dp.cvColor(),
		dp._dpValues._lineThickness,
		dp._dpValues._lineType==1?cv::LINE_AA:cv::LINE_8
	);
}

//------------------------------------------------------------------
namespace base {
/// Draw PolylineBase (Opencv implementation)
template<typename PLT,typename FPT>
void
PolylineBase<PLT,FPT>::draw( img::Image<cv::Mat>& im, img::DrawParams dp ) const
{
	if( size() < 2 ) // nothing to draw
		return;

	for( size_t i=0; i<nbSegs(); i++ )
		getSegment(i).draw( im, dp );

	if( dp._dpValues._showPoints )
	{
		auto newPointStyle = dp._dpValues.nextPointStyle();
		getPoint(0).draw( im, dp.setColor(10,10,10).setPointStyle( newPointStyle ) );
	}

	if( dp._dpValues._showIndex )
		for( size_t i=0; i<size(); i++ )
		{
			auto pt = getPoint(i);
			cv::putText(
				im.getReal(),
				std::to_string(i),
				pt.getCvPti(),
				0,
				0.6,
				cv::Scalar( 20,20,20 ),
				2
			);
		}

	if( dp._dpValues._showAngles )
	{
		auto osegs = getOSegs();
//			std::cout << "osegs size=" << osegs.size() << "\n";

		const auto& pts = getPts();
		for( size_t i=0; i<osegs.size()-1; i++ )
		{
			auto seg1 = osegs[i];
			auto seg2 = osegs[i+1];
			auto angle = getAngle( seg1, seg2 );
//			std::cout << "angle " << i << "=" << angle*180/3.1415 << "\n";
			drawText( im, std::to_string(angle * 180./M_PI), pts[i+1] );
		}
		if( isClosed() )
		{
			auto seg1 = osegs.back();
			auto seg2 = osegs.front();
			auto angle = getAngle( seg1, seg2 );
//			std::cout << "final angle " << "=" << angle*180/3.1415 << "\n";
			drawText( im, std::to_string(angle * 180./M_PI), pts[0] );
		}
	}
}

} // namespace base

//------------------------------------------------------------------
#endif // HOMOG2D_USE_OPENCV

/////////////////////////////////////////////////////////////////////////////
// SECTION .3 - CLASS DRAWING MEMBER FUNCTIONS (SVG)
/////////////////////////////////////////////////////////////////////////////

namespace img {

/// Streaming operator (only defined for SVG)
inline
std::ostream&
operator << ( std::ostream& f, const Image<SvgImage>& im )
{
	f << "<svg version=\"1.1\" width=\"" << im._width
		<< "\" height=\"" << im._height
		<< "\" style=\"background-color:white;\" xmlns=\"http://www.w3.org/2000/svg\">\n"
		<< "<style>\n"
		<< ".txt1 { font: bold 12px sans-serif; };\n"   // text style, you can change or add classes as required
		<< "</style>\n";

	f << im._realImg._svgString.str();
	f << "</svg>\n";

	return f;
}
} // namespace img

/// Draw text on Svg image
/// \todo 20230118: find a way to add a default parameter for dp (not allowed on explicit instanciation)
template <>
inline
void
img::Image<img::SvgImage>::drawText( std::string str, Point2d_<float> pt, img::DrawParams dp )
{
	getReal()._svgString << "<text x=\"" << pt.getX()
		<< "\" y=\"" << pt.getY()
		<< "\" font-size=\"" << dp._dpValues._fontSize << "px"
		<< "\" fill=\"#" << std::hex << std::setfill('0')
			<< std::setw(2) << (int)dp.color().r
			<< std::setw(2) << (int)dp.color().g
			<< std::setw(2) << (int)dp.color().b
		<< std::dec << "\">" << str << "</text>\n";
}

#ifdef HOMOG2D_USE_OPENCV
/// Draw text on Opencv image
template <>
inline
void
img::Image<cv::Mat>::drawText( std::string str, Point2d_<float> pt, img::DrawParams dp )
{
	auto col = dp.color();
	cv::putText(
		getReal(), str, pt.getCvPtd(),
		cv::FONT_HERSHEY_SIMPLEX,   // font id, see https://docs.opencv.org/4.7.0/
		0.03 * dp._dpValues._fontSize, // scale factor (approximate...)
		cv::Scalar( col.b, col.g, col.r )
	);
}
#endif

//------------------------------------------------------------------
/// Draw \c Circle (SVG implementation)
template<typename FPT>
void
Circle_<FPT>::draw( img::Image<img::SvgImage>& im, img::DrawParams dp ) const
{
	im.getReal()._svgString
		<< "<circle cx=\"" << center().getX()
		<< "\" cy=\"" << center().getY()
		<< "\" r=\"" << radius()
		<< "\" stroke=\""
		<< dp.getSvgRgbColor()
		<< "\" stroke-width=\"" << dp._dpValues._lineThickness << "\" ";
	if( !dp.holdsFill() )
		im.getReal()._svgString << "fill=\"none\" ";
	im.getReal()._svgString << dp.getAttrString()
		<< "/>\n";
}

//------------------------------------------------------------------
/// Draw \c Ellipse (SVG implementation)
template<typename FPT>
void
Ellipse_<FPT>::draw( img::Image<img::SvgImage>& im, img::DrawParams dp )  const
{
	im.getReal()._svgString << "<ellipse cx=\""
		<< getCenter().getX()
		<< "\" cy=\""
		<< getCenter().getY()
		<< "\" rx=\""
		<< getMajMin().first
		<< "\" ry=\""
		<< getMajMin().second
		<< "\" stroke=\""
		<< dp.getSvgRgbColor()
		<< "\" stroke-width=\"" << dp._dpValues._lineThickness << "\" ";
	if( !dp.holdsFill() )
		im.getReal()._svgString << "fill=\"none\" ";
	im.getReal()._svgString << dp.getAttrString()
		<< "transform=\"rotate(" << angle()*180./M_PI << ',' << getCenter().getX() << ',' << getCenter().getY()
		<< ")\" />\n";
}

//------------------------------------------------------------------
/// Draw \c FRect (SVG implementation)
template<typename FPT>
void
FRect_<FPT>::draw( img::Image<img::SvgImage>& im, img::DrawParams dp ) const
{
	im.getReal()._svgString << "<rect x=\""
		<< getPts().first.getX()
		<< "\" y=\""
		<< getPts().first.getY()
		<< "\" width=\""
		<< width()
		<< "\" height=\""
		<< height()
		<< "\" stroke=\""
		<< dp.getSvgRgbColor()
		<< "\" stroke-width=\"" << dp._dpValues._lineThickness << "\" ";
	if( !dp.holdsFill() )
		im.getReal()._svgString << "fill=\"none\" ";
	im.getReal()._svgString << dp.getAttrString() << "/>\n";
}

//------------------------------------------------------------------
namespace priv {

/// Helper function to draw SVG segment
template<typename T>
void drawSvgSeg(
	img::Image<img::SvgImage>& im,
	const Point2d_<T>& pt1,
	const Point2d_<T>& pt2,
	std::string    color,
	int            thickness,
	std::string    attribs=std::string()
)
{
	im.getReal()._svgString << "<line x1=\""
		<< pt1.getX()
		<< "\" y1=\""
		<< pt1.getY()
		<< "\" x2=\""
		<< pt2.getX()
		<< "\" y2=\""
		<< pt2.getY()
		<< "\" stroke=\"" << color
		<< "\" stroke-width=\"" << thickness << "\" "
		<< attribs << "/>\n";
}

template<typename T>
void drawSvgSeg(
	img::Image<img::SvgImage>& im,
	const std::pair<Point2d_<T>,Point2d_<T>>& ppts,
	std::string    color,
	int            thickness,
	std::string    attribs=std::string()
)
{
	drawSvgSeg( im, ppts.first, ppts.second, color, thickness, attribs );
}

} // namespace priv
//------------------------------------------------------------------
/// Draw \c Segment / \c OSegment (SVG implementation)
/// \todo 20250127: implement arrows for the Opencv/png version, and share the code between the two versions
namespace base {

template<typename SV, typename FPT>
void
SegVec<SV,FPT>::draw( img::Image<img::SvgImage>& im, img::DrawParams dp ) const
{
	auto group = false;
	if( dp._dpValues._showPoints || std::is_same_v<SV,typ::IsOSeg> )
		group = true;

	if( group )
		im.getReal()._svgString << "<g>";

	auto pts = getPts();
	priv::drawSvgSeg( im, pts, dp.getSvgRgbColor(), dp._dpValues._lineThickness, dp.getAttrString() );

	if constexpr( std::is_same_v<SV,typ::IsOSeg> )
	{
		auto arrsegs = priv::getArrowSegments( *this );
		for( auto ppts: arrsegs )
			priv::drawSvgSeg( im, ppts, dp.getSvgRgbColor(), dp._dpValues._lineThickness );
	}
	else
	{
		if( dp._dpValues._showPoints )
		{
			_ptS1.draw( im, dp );
			_ptS2.draw( im, dp );
		}
	}

	if( group )
		im.getReal()._svgString << "</g>\n";
}

} // namespace base
//------------------------------------------------------------------
#ifdef HOMOG2D_ENABLE_PRTP
namespace rtp {
/// Stream operator for \c Root type
/** \todo replace this by a call to a virtual function `print()`
(that needs to be defined in all the child classes as:
\code
void print( std::ostream& f )
{
	f << *this;
}
\endcode
*/
inline
std::ostream&
operator << ( std::ostream& f, const Root& p )
{
	f << "type="<< getString( p.type() ) << std::endl;
	switch( p.type() )
	{
		case Type::Circle:
		{
			const Circle_<double>* p2 = static_cast<const Circle_<double>*>( &p );
			f << *p2;
		}
		break;
		case Type::Ellipse:
		{
			const Ellipse_<double>* p2 = static_cast<const Ellipse_<double>*>( &p );
			f << *p2;
		}
		break;
		case Type::FRect:
		{
			const FRect_<double>* p2 = static_cast<const FRect_<double>*>( &p );
			f << *p2;
		}
		break;
		case Type::Line2d:
		{
			const Line2d_<double>* p2 = static_cast<const Line2d_<double>*>( &p );
			f << *p2;
		}
		break;
		case Type::Point2d:
		{
			const Point2d_<double>* p2 = static_cast<const Point2d_<double>*>( &p );
			f << *p2;
		}
		break;
		case Type::Segment:
		{
			const Segment_<double>* p2 = static_cast<const Segment_<double>*>( &p );
			f << *p2;
		}
		break;
		case Type::OSegment:
		{
			const OSegment_<double>* p2 = static_cast<const OSegment_<double>*>( &p );
			f << *p2;
		}
		break;
		case Type::OPolyline:
		{
			const OPolyline_<double>* p2 = static_cast<const OPolyline_<double>*>( &p );
			f << *p2;
		}
		break;
		case Type::CPolyline:
		{
			const CPolyline_<double>* p2 = static_cast<const CPolyline_<double>*>( &p );
			f << *p2;
		}
		break;
		default: assert(0);
	}
	return f;
}

} // namespace rtp
#endif // HOMOG2D_ENABLE_PRTP

//------------------------------------------------------------------
namespace base {
/// Draw Polyline (SVG implementation)
/**
\note To show the points index, we don't use the svg "marker-start/marker-mid/marker-end" syntax
so that the dots always have the same color as the segments
*/
template<typename PLT,typename FPT>
void
PolylineBase<PLT,FPT>::draw( img::Image<img::SvgImage>& im, img::DrawParams dp ) const
{
	if( size() < 2 ) // nothing to draw
		return;

	if( dp._dpValues._showIndex || dp._dpValues._showPoints )
		im.getReal()._svgString << "<g>\n";

	im.getReal()._svgString << '<' << (isClosed() ? "polygon" : "polyline")
		<< " stroke=\""
		<< dp.getSvgRgbColor()
		<< "\" stroke-width=\"" << dp._dpValues._lineThickness << "\" ";
	if( !dp.holdsFill() )
		im.getReal()._svgString << "fill=\"none\" ";
	im.getReal()._svgString << dp.getAttrString()
		<< "points=\"";
	for( const auto& pt: getPts() )
		im.getReal()._svgString << pt.getX() << ',' << pt.getY() << ' ';
	im.getReal()._svgString << "\"/>\n";

/// \todo 20240215 Why don't we use the drawText() function ?
	if( dp._dpValues._showIndex )
	{
		im.getReal()._svgString << "<g>\n";
		for( size_t i=0; i<size(); i++ )
		{
			auto pt = getPoint(i);
			im.getReal()._svgString << "<text x=\""
				<< (int)pt.getX()
				<< "\" y=\""
				<< (int)pt.getY()
				<< "\" class=\"txt1\">"
				<< i
				<< "</text>\n";
		}
		im.getReal()._svgString << "</g>\n";
	}

	if( dp._dpValues._showPoints )
	{
		im.getReal()._svgString << "<g>\n";
		for( size_t i=0; i<size(); i++ )
			getPoint(i).draw( im, dp );
		im.getReal()._svgString << "</g>\n";
	}

	if( dp._dpValues._showAngles )
	{
		auto osegs = getOSegs();
//			std::cout << "osegs size=" << osegs.size() << "\n";

		const auto& pts = getPts();
		for( size_t i=0; i<osegs.size()-1; i++ )
		{
			auto seg1 = osegs[i];
			auto seg2 = osegs[i+1];
			auto angle = getAngle( seg1, seg2 );
//			std::cout << "angle " << i << "=" << angle*180/3.1415 << "\n";
			drawText( im, std::to_string(angle * 180./M_PI), pts[i+1] );
		}
	}

	if( dp._dpValues._showIndex || dp._dpValues._showPoints )
		im.getReal()._svgString << "</g>\n";
}

} // namespace base


/////////////////////////////////////////////////////////////////////////////
// SECTION  - TYPEDEFS
/////////////////////////////////////////////////////////////////////////////

/// Default line type, uses \c double as numerical type
using Line2d = Line2d_<HOMOG2D_INUMTYPE>;

/// Default point type, uses \c double as numerical type
using Point2d = Point2d_<HOMOG2D_INUMTYPE>;

/// Default homography (3x3 matrix) type, uses \c double as numerical type
using Homogr = Homogr_<HOMOG2D_INUMTYPE>;

/// Default homogeneous matrix, uses \c double as numerical type
using Epipmat = Hmatrix_<typ::IsEpipmat,HOMOG2D_INUMTYPE>;

/// Default segment type
using Segment  = Segment_<HOMOG2D_INUMTYPE>;
using OSegment = OSegment_<HOMOG2D_INUMTYPE>;

/// Default circle type
using Circle = Circle_<HOMOG2D_INUMTYPE>;

/// Default rectangle type
using FRect = FRect_<HOMOG2D_INUMTYPE>;

/// Default polyline type
using CPolyline = CPolyline_<HOMOG2D_INUMTYPE>;
using OPolyline = OPolyline_<HOMOG2D_INUMTYPE>;

/// Default ellipse type
using Ellipse = Ellipse_<HOMOG2D_INUMTYPE>;

// float types
using Line2dF   = Line2d_<float>;
using Point2dF  = Point2d_<float>;
using HomogrF   = Homogr_<float>;
using SegmentF  = Segment_<float>;
using OSegmentF = OSegment_<float>;
using CircleF   = Circle_<float>;
using FRectF    = FRect_<float>;
using EllipseF  = Ellipse_<float>;

// double types
using Line2dD   = Line2d_<double>;
using Point2dD  = Point2d_<double>;
using HomogrD   = Homogr_<double>;
using SegmentD  = Segment_<double>;
using OSegmentD = OSegment_<double>;
using CircleD   = Circle_<double>;
using FRectD    = FRect_<double>;
using EllipseD  = Ellipse_<double>;

// long double types
using Line2dL   = Line2d_<long double>;
using Point2dL  = Point2d_<long double>;
using HomogrL   = Homogr_<long double>;
using SegmentL  = Segment_<long double>;
using OSegmentL = OSegment_<long double>;
using CircleL   = Circle_<long double>;
using FRectL    = FRect_<long double>;
using EllipseL  = Ellipse_<long double>;

using CPolylineF = CPolyline_<float>;
using CPolylineD = CPolyline_<double>;
using CPolylineL = CPolyline_<long double>;

using OPolylineF = OPolyline_<float>;
using OPolylineD = OPolyline_<double>;
using OPolylineL = OPolyline_<long double>;

using PointPairF = PointPair_<float>;
using PointPairD = PointPair_<double>;
using PointPairL = PointPair_<long double>;
using PointPair  = PointPair_<double>;

#ifdef HOMOG2D_ENABLE_VRTP
// variant type
using CommonType  = CommonType_<HOMOG2D_INUMTYPE>;
using CommonTypeF = CommonType_<float>;
using CommonTypeD = CommonType_<double>;
using CommonTypeL = CommonType_<long double>;
#endif

#ifdef HOMOG2D_USE_SVG_IMPORT

/// Holds stuff related to SVG import
namespace svg {

/// Private functions related to the SVG import code
namespace svgp {

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
//------------------------------------------------------------------
/// Importing rotated ellipse from SVG data
inline
std::pair<Point2d_<HOMOG2D_INUMTYPE>,HOMOG2D_INUMTYPE>
getEllipseRotateAttr( const char* rot_str )
{
	std::string s(rot_str);
//	std::cout << __FUNCTION__ << "(): " << s << "\n";
	auto v1 = tokenize( s, '(' );
	if( v1.size() == 2 )
		if( v1[0] == "rotate" )
		{
			auto v2 = v1[1].substr(0, v1[1].size() - 1 );
//			std::cout << __FUNCTION__ << "(): v2=" << v2 << "\n";
			auto v3 = tokenize( v2, ',' );
			if( v3.size() == 3 )
			{
				try  // in case of an incorrect numerical string
				{
					auto angle = std::stod( v3[0] );
					auto x0    = std::stod( v3[1] );
					auto y0    = std::stod( v3[2] );
					return std::make_pair( Point2d_<HOMOG2D_INUMTYPE>(x0,y0),angle*M_PI/180. );
				}
				catch( std::exception& err )
				{
					HOMOG2D_THROW_ERROR_2( "invalid 'transform' attribute for svg ellipse import",  err.what() );
				}
			}
		}
	HOMOG2D_THROW_ERROR_1( "invalid 'transform' attribute for svg ellipse import" );
}

//------------------------------------------------------------------
/// Svg import: Basic parsing of points that are in the format "10,20 30,40 50,60"
/// \todo 20240326: this is used to import SVG polygon type. Maybe this can be replaced by
/// the "path" import code?
inline
std::vector<Point2d>
parsePoints( const char* pts )
{
	std::vector<Point2d> out;
	std::string s(pts);
//	std::cout << "processing " << s << '\n';
//	trimString( s );
	auto v1 = tokenize( s, ' ' );
	for( const auto& pt: v1 )
	{
		auto v2 = tokenize( pt, ',' );
		if( v2.size() != 2 )
			throw "h2d:img::svg: invalid point format in importing svg element: " + s;
		auto x = std::stod( v2[0] );
		auto y = std::stod( v2[1] );
		out.emplace_back( Point2d(x,y) );
	}
	return out;
}


inline
bool isDigit( char c )
{
	return ( (c >= '0' && c <= '9') || c == '.' || c == '-' );
}

inline
bool svgPathCommandIsAllowed(char c)
{
	if( c == 'M' || c == 'L' || c == 'H'|| c == 'V' || c == 'Z' )
		return true;
	return false;
}

/// Get next element in svg path string.
inline
std::string
getNextElem( const std::string& str, std::string::const_iterator& it )
{
	std::string out;
	while( *it == ' ' || *it == ',' ) // skip spaces and commas
		it++;

	if( it >= str.cend() )
		return out;         // return empty string

	if( !isDigit(*it) )        // alpha character
	{
		out.push_back( *it );
		it++;
	}
	else
	{
		while( isDigit(*it) && it < str.cend() )
		{
			out.push_back( *it );
			it++;
		}
//		std::cout << "getNextElem() DIGITS:-" << out << "- #=" << out.size() << '\n';
//		return out;
	}
	return out;
}

/// Returns nb of expected values for a given SVG path command.
/// Ref: https://www.w3.org/TR/SVG2/paths.html
inline
std::map<char,int>&
numberValues()
{
	static std::map<char,int> nbval;
	nbval['M'] = 2; // M-m
	nbval['L'] = 2; // L-l
	nbval['H'] = 1; // H-h
	nbval['V'] = 1; // V-v
	nbval['C'] = 6; // C-c !NOT HANDLED!
	nbval['S'] = 4; // S-s !NOT HANDLED!
	nbval['Q'] = 4; // Q-q !NOT HANDLED!
	nbval['T'] = 2; // T t !NOT HANDLED!
	nbval['A'] = 7; // A-a !NOT HANDLED!
	nbval['Z'] = 0; // Z-z
	return nbval;
}

enum class PathMode { Absolute, Relative };

/// Holds the current SVG "path" command, and the number of required numerical values
/// \sa SvgValuesBuffer
struct SvgPathCommand
{
	PathMode _absRel = PathMode::Absolute;
	char     _command = 'M';
	uint8_t  _nbValues = 2;
	bool isAbsolute() const
	{
		return _absRel == PathMode::Absolute ? true : false;
	}
	void setCommand( char c )
	{
		_command = c;
		_nbValues = numberValues().at(c);
	}
	bool isNewPolyline() const
	{
		if( _command == 'm' || _command == 'M' || _command == 'z' || _command == 'Z' )
			return true;
		return false;
	}
};


/// Generate new point from current mode and previous point, handles absolute/relative coordinates
inline
Point2d_<double>
generateNewPoint(
	SvgPathCommand             mode,     ///< SVG path command
	Point2d_<double>           prevPt,   ///< previous point, is needed if relative mode
	const std::vector<double>& val       ///< numerical values that have been stored
)
{
//	std::cout << "generateNewPoint(): command=" << mode._command
//		<< std::hex << " hex=" << (int)mode._command << std::dec << '\n';
	auto nb = val.size();
//	priv::printVector( val,"generateNewPoint()" );
	HOMOG2D_LOG( "abs/rel=" << (mode.isAbsolute()?"ABS":"REL") );
// some checking to lighten up the following code, maybe can be removed afterwards
	switch( mode._command )
	{
		case 'M':
		case 'L':
			assert( nb == 2 );
		break;
		case 'H':
		case 'V':
			assert( nb == 1 );
		break;
		case 'Z':
			assert( nb == 0 );
		break;
		default: assert(0);
	}

	Point2d_<double> out;
	switch( mode._command )
	{
		case 'M':
		case 'L':
			if( mode.isAbsolute() )
				out.set( val[0], val[1] );
			else
				out.set( prevPt.getX() + val[0], prevPt.getY() + val[1] );
		break;

		case 'H':   // Horizontal
			if( mode.isAbsolute() )
				out.set( val[0],                  prevPt.getY() );
			else
				out.set( val[0] + prevPt.getX() , prevPt.getY() );
		break;

		case 'V':    // Vertical
			if( mode.isAbsolute() )
				out.set( prevPt.getX(), val[0]                 );
			else
				out.set( prevPt.getX(), val[0] + prevPt.getY() );
		break;

		default: assert(0);
	}
	return out;
}

inline
SvgPathCommand
getCommand( char c )
{
	HOMOG2D_LOG( "search command for " << c );

	static std::string commands( "MLHVCSQTAZ" ); // allowed SVG path commands (and their counterparts relative)
	SvgPathCommand out;
	std::string str( 1, c );
	HOMOG2D_LOG( " str=" << str << " #=" << str.size() );
	auto pos = commands.find( str );
//	HOMOG2D_LOG( "pos=" << pos << " str=" << str );
	bool invalid = false;
	if( pos == std::string::npos )
	{
		if( c>='a' && c <= 'z' ) // check if lowercase
		{
			c = (char)((int)c+'A'-'a');
//			std::cout << "lowercase, new c=" << c << '\n';
			str[0] = c;
//			HOMOG2D_LOG( " str2=" << str << "#=" << str.size() );
			pos = commands.find( str );
			if( pos != std::string::npos )
				out._absRel = PathMode::Relative;
			else
				invalid = true;
		}
		else
			invalid = true;
	}
	if( invalid )
		HOMOG2D_THROW_ERROR_1(
				"Illegal character in SVG path element:-" << str
				<< "- ascii=" << std::hex << str[0] << std::dec
		);

	out.setCommand( commands[pos] );

//	std::cout << "pos=" << pos << " _command=" << out._command << " _nbValues=" << (int)out._nbValues << '\n';
	return out;
}

/// Removes dupes in set of points. Needed when importing SVG files using a "path" command,
/// because sometimes they hold duplicates points, and that can't be in polylines
template<typename FPT>
std::vector<Point2d_<FPT>>
purgeSetDupes( const std::vector<Point2d_<FPT>>& pts )
{
	std::vector<Point2d_<FPT>> out;
	out.reserve( pts.size() );
	for( auto it=pts.begin(); it!=std::prev(pts.end()); it++ )
	{
		const auto& elem = *it;
		const auto& next = *std::next(it);
		if( elem != next )
			out.push_back( elem );
	}
	out.push_back( pts.back() ); // add last one
	return out;
}

/// This will hold the values read from the SVG Path parsing code, before they are
/// converted to points
/// \sa SvgPathCommand
struct SvgValuesBuffer
{
	std::vector<double> _values;
	Point2d             _previousPt;

	size_t size() const
	{
		return _values.size();
	}

	void storeValues( std::vector<Point2d>& out, SvgPathCommand mode )
	{
		if( _values.size() != (size_t)mode._nbValues )
			HOMOG2D_THROW_ERROR_1(
				"SVG path command: inconsistency with stored values, expected "
				<< (size_t)mode._nbValues << ", got "
				<< _values.size()
			);
		auto pt = generateNewPoint( mode, _previousPt, _values );
		HOMOG2D_LOG( "new point added: " << pt );
		out.push_back( pt );
		_previousPt = pt;
		_values.clear();
	}
	void addValue( std::string elem )
	{
		_values.push_back( std::stod(elem) );
	}
};

/// Parse a SVG "path" string and convert it to a vector holding a set (vector) of points
/**
Input string example:
\verbatim
m 261.68497,138.79393 2.57,3.15 -0.72,1.27 2.18,1.94 -0.7,4.93 1.88,0.9
\endverbatim
The return value holds as 'second' a bool value, will be true if closed polyline
*/
inline
auto
parsePath( const char* s )
{
	SvgPathCommand mode;
	std::vector<std::vector<Point2d>> vout(1);
	SvgValuesBuffer values;
	std::string str(s);
	size_t idx = 0;
	HOMOG2D_LOG( "parsing string -" << str << "- #=" << str.size() );
	if( str.size() == 0 )
		HOMOG2D_THROW_ERROR_1( "SVG path string is empty" );

	auto it = str.cbegin();
	Point2d previousPt;
	do
	{
		auto e = getNextElem( str, it );
//		HOMOG2D_LOG( "parsing element -" << e << "- #=" << e.size() );

		if( e.size() == 1 && !isDigit(e[0]) ) // we have a command !
		{
			if( values.size() != 0 )              // if we have some values stored,
				values.storeValues( vout[idx], mode );  //  first process them and add new point

			mode = getCommand( e[0] );
			HOMOG2D_LOG( "command=" << e[0] );
			if( !svgPathCommandIsAllowed(mode._command) )
				HOMOG2D_THROW_ERROR_1( "SVG path command -" << mode._command << "- not handled" );

			if( mode.isNewPolyline() && !vout[idx].empty() )
			{
				vout.push_back( std::vector<Point2d>() );
				idx++;
				HOMOG2D_LOG( "NEW vector idx=" << idx );
			}
		}
		else // not a command, but a value
		{
//			HOMOG2D_LOG( "process value, values size=" << values.size() );
			if( values.size() == (size_t)mode._nbValues ) // already got enough values
				values.storeValues( vout[idx], mode );
			values.addValue( e );
		}
	}
	while( it < str.cend() );

	if( values.size() )                  // process remaining values that have been stored
		values.storeValues( vout[idx], mode );

	HOMOG2D_LOG( "Nb vectors=" << vout.size() );
	for( auto& v: vout )
		if( v.size() )
			v = purgeSetDupes( v );
	if( vout.back().empty() )
		vout.pop_back();

	return std::make_pair(
		vout,
		mode._command == 'Z' ? true : false
	);
}

} // namespace svgp

//------------------------------------------------------------------
/// Visitor class, derived from the tinyxml2 visitor class. Used to import SVG data.
/**
Holds the imported data through std::variant
*/
class Visitor: public tinyxml2::XMLVisitor
{
/// This type is used to provide a type that can be used in a switch (see VisitExit() ),
/// as this cannot be done with a string |-(
	enum SvgType {
		T_circle, T_rect, T_line, T_polygon, T_polyline, T_ellipse, T_path,
		T_other ///< for other elements (\c <svg>) or illegal ones, that will just be ignored
	};

/// A map holding correspondences between type as a string and type as a SvgType.
/// Populated in constructor
	std::map<std::string,SvgType> _svgTypesTable;

	std::vector<CommonType_<double>> _vecVar; ///< all the data is stored here

public:
/// Constructor, populates the table giving type from svg string
	Visitor()
	{ //               svg name      local type id
		_svgTypesTable["circle"]   = T_circle;
		_svgTypesTable["rect"]     = T_rect;
		_svgTypesTable["line"]     = T_line;
		_svgTypesTable["polyline"] = T_polyline;
		_svgTypesTable["polygon"]  = T_polygon;
		_svgTypesTable["ellipse"]  = T_ellipse;
		_svgTypesTable["path"]     = T_path;
	}

/// Returns the type as a member of enum SvgType, so the type can be used in a switch
	SvgType
	getSvgType( std::string s ) const
	{
		auto it = _svgTypesTable.find( s );
		if( it == std::cend(_svgTypesTable) )
			return T_other;
		return it->second;
	}

/// Used to access the data once the file has been read
	const std::vector<CommonType_<double>>&
	get() const
	{
		return _vecVar;
	}

	bool VisitExit( const tinyxml2::XMLElement& ) override;
};

namespace svgp {
//------------------------------------------------------------------
/// Fetch attribute from XML element. Tag \c e_name is there just in case of trouble.
inline
double
getAttribValue( const tinyxml2::XMLElement& e, const char* str, std::string e_name )
{
	double value=0.;
	if( tinyxml2::XML_SUCCESS != e.QueryDoubleAttribute( str, &value ) )
		HOMOG2D_THROW_ERROR_1( "h2d::svg::import error, failed to read attribute '"
			<< std::string{str} << "' while reading element '" << e_name << "'" );
	return value;
}

/// Helper function for SVG import
/**
\todo Who owns the data? Should we return a string and/or release the memory?
*/
inline
const char*
getAttribString( const char* attribName, const tinyxml2::XMLElement& e )
{
	const char *pts = e.Attribute( attribName );
	if( !pts )
		throw std::string("h2d::img::svg Error: unable to find attribute '") + attribName + "' in tag " + e.Name();
	return pts;
}

/// Helper function called by Visitor::VisitExit() to process Polyline/Polygons
inline
std::vector<Point2d>
importSvgPoints( const tinyxml2::XMLElement& e )
{
	auto pts_str = svgp::getAttribString( "points", e );
	auto vec_pts = svgp::parsePoints( pts_str );

	if( vec_pts.front() == vec_pts.back() ) // if first point equal to last
		vec_pts.pop_back();                  //  point, remove last point

	return vec_pts;
}

} // namespace svgp


/// This is the place where actual SVG data is converted and stored into vector
/**
(see manual, section "SVG import")

Overload of the root class `VisitExit()` member function

*/
inline
bool
Visitor::VisitExit( const tinyxml2::XMLElement& e )
{
	std::string n = e.Name();
//	std::cout << "element name:" << n << '\n';
	try
	{
		switch( getSvgType( n ) )
		{
			case T_circle:
				_vecVar.emplace_back(
					CircleD(
						svgp::getAttribValue( e, "cx", n ),
						svgp::getAttribValue( e, "cy", n ),
						svgp::getAttribValue( e, "r", n )
					)
				);
			break;

			case T_rect:
			{
				auto x1 = svgp::getAttribValue( e, "x", n );
				auto y1 = svgp::getAttribValue( e, "y", n );
				auto w  = svgp::getAttribValue( e, "width", n );
				auto h  = svgp::getAttribValue( e, "height", n );
				_vecVar.emplace_back( FRectD( x1, y1, x1+w, y1+h ) );
			}
			break;

			case T_line:
				_vecVar.emplace_back(
					SegmentD(
						svgp::getAttribValue( e, "x1", n ),
						svgp::getAttribValue( e, "y1", n ),
						svgp::getAttribValue( e, "x2", n ),
						svgp::getAttribValue( e, "y2", n )
					)
				);
			break;

			case T_polygon:
			{
				auto vpts = svgp::importSvgPoints( e );
				_vecVar.emplace_back( CPolylineD(vpts) );
			}
			break;

			case T_polyline:
			{
				auto vpts = svgp::importSvgPoints( e );
				_vecVar.emplace_back( OPolylineD(vpts) );
			}
			break;

			case T_path: // a path can hold multiple polygons (because of the 'L' command)
			{
				auto pts_str = svgp::getAttribString( "d", e );
				try
				{
					auto parse_res = svgp::parsePath( pts_str );
					const auto& vec_vec_pts = parse_res.first;  //
					for( auto vec_pts: vec_vec_pts ) // we need a copy so we may remove last point if equal to first
//					for( const auto& vec_pts: vec_vec_pts ) // we need a copy so we may remove last point if equal to first
					{
						if( vec_pts.front() == vec_pts.back() ) // if first point equal to last
							vec_pts.pop_back();                  //  point, remove last point

						if( parse_res.second == true )
							_vecVar.emplace_back( CPolylineD(vec_pts) );
						else
							_vecVar.emplace_back( OPolylineD(vec_pts) );
					}
				}
				catch( std::exception& err )    // an unhandled path command will just get the whole path command ignored
				{
					HOMOG2D_LOG_WARNING( "Unable to import SVG path command\n -msg="
						<< err.what() << "\n -input string=" << pts_str
					);
				}
			}
			break;

			case T_ellipse:
			{
				auto x  = svgp::getAttribValue( e, "cx", n );
				auto y  = svgp::getAttribValue( e, "cy", n );
				auto rx = svgp::getAttribValue( e, "rx", n );
				auto ry = svgp::getAttribValue( e, "ry", n );
				auto rot = svgp::getEllipseRotateAttr( svgp::getAttribString( "transform", e ) );
				auto ell = EllipseD( x, y, rx, ry );

				auto H = Homogr().addTranslation(-x,-y).addRotation(rot.second).addTranslation(x,y);
				_vecVar.push_back( H * ell );
			}
			break;

			default:  // for T_other elements
				if( n != "svg" ) // because that one will be always there, so no need to show a warning
					HOMOG2D_LOG_WARNING( "found SVG element '" << n << "' in SVG file, left unprocessed" );
			break;
		}
	}
	catch( std::string& msg )
	{
		HOMOG2D_THROW_ERROR_1( "h2d: Tinyxml read error: " << msg );
		return false; // to avoid a compile warning
	}
	return true;
}

inline
void
printFileAttrib( const tinyxml2::XMLDocument& doc )
{
	const tinyxml2::XMLElement* root = doc.RootElement();
	const tinyxml2::XMLAttribute* pAttrib = root->FirstAttribute();
	size_t i=0;
	while( pAttrib )
	{
		std::cout << "Attrib " << i++ << ": Name=" << pAttrib->Name() << "; Value=" << pAttrib->Value() << '\n';
		pAttrib=pAttrib->Next();
	}
}

/// Fetch size of image in SVG file
inline
auto
getImgSize( const tinyxml2::XMLDocument& doc )
{
	const tinyxml2::XMLElement* root = doc.RootElement();
	const tinyxml2::XMLAttribute* pAttrib = root->FirstAttribute();
	double w = -1., h = -1.;
	while( pAttrib )
	{
		auto attr = std::string( pAttrib->Name() );
		if( attr == "width" )
			w = std::stod( pAttrib->Value() );
		if( attr == "height" )
			h = std::stod( pAttrib->Value() );
		pAttrib=pAttrib->Next();
	}
	if( w == -1. || h == -1. )
		HOMOG2D_THROW_ERROR_1( "unable to find size in SVG file" );
	return std::make_pair( w, h );
}

} // namespace svg

#endif // HOMOG2D_USE_SVG_IMPORT

} // namespace h2d

#endif // HG_HOMOG2D_HPP

