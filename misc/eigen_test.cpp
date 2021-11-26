
#include "homog2d.hpp"
#include <Eigen/LU>
//#include <Eigen/Dense>

using namespace homog2d;

int main()
{
	Homogr h (3,4); // translation



using Eigen::MatrixXd;

  MatrixXd m(2,2);
  m(0,0) = 3;
  m(1,0) = 2.5;
  m(0,1) = -1;
  m(1,1) = m(1,0) + m(0,1);
  std::cout << m << std::endl;

  auto m2 = m.inverse();
  std::cout << m2 << std::endl;
}
