/**
\file
\brief Numerical computation, see
https://github.com/skramm/homog2d/blob/master/docs/homog2d_showcase3.md

Run this with `$ make showcase3`
*/

#include "homog2d.hpp"

using namespace h2d;

int main() {
    FRect rect(0,0,3,2); // any values should be fine, actually

    auto pts = get4Pts( rect );
    auto segs = getSegs( rect );

    auto seg1 = Segment( pts[0], getCenter( segs[1] ) );
    auto seg2 = Segment( pts[3], getCenter( segs[1] ) );
    auto seg3 = Segment( pts[1], getCenter( segs[2] ) );

    std::vector<Point2d> vpts{
        seg1.intersects(seg3).get(),
        seg2.intersects(seg3).get(),
        getCenter( segs[1] )
    };

    CPolyline poly( vpts );
    std::cout << "solution=" << 1.0 - poly.area() / rect.area() << '\n';
}

