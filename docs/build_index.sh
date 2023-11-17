
sort --ignore-case index.csv >/tmp/index.csv
outfn=index.md
echo "# Index">$outfn

while IFS=";" read -ra tab
do
	lname=${tab[0]}
	if [ "${lname:0:1}" != "#" ]
	then
		if ! [ -z "$lname" ]
		then
			fn=${tab[1]}
			anchor=${tab[2]}
			echo "* [$lname](homog2d_$fn.md#$anchor)">>$outfn
		fi
	fi
done < /tmp/index.csv

echo "<hr>">>$outfn
echo "Page generated on $(date '+%Y-%m-%d')">>$outfn
