
echo "- Start test with float"
./homog2d_test_f >stderr_f
if [ $? = 0 ]; then echo "Success"; else echo "Failure !"; fi

echo "- Start test with double"
./homog2d_test_d  >stderr_d
if [ $? = 0 ]; then echo "Success"; else echo "Failure !"; fi

echo "- Start test with long double"
./homog2d_test_l  >stderr_l
if [ $? = 0 ]; then echo "Success"; else echo "Failure !"; fi


