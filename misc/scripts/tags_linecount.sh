set +x

cd /tmp
git clone git@github.com:skramm/homog2d.git
cd homog2d

taglist=$(git tag -l)
for t in $taglist
do
#	echo "tag=$t"
	p=""
	if [ "$t" != "v1.0" ]; then p=misc/; fi
	git checkout $t
	c1=$(cat homog2d.hpp|wc -l)
	c2=$(cat ${p}homog2d_test.cpp|wc -l)
	echo "$t;$c1;$c2"
done
