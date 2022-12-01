# preliminary
# build mp4 video from still images
# requires ffmpeg, tested with version 4.2.7

set +x

# step 1: build programs
mkdir -p BUILD/showcase_v/png
mkdir -p BUILD/showcase_v/vid

for f in misc/showcase_v/*.cpp
do
	fn=$(basename -- "$f")
	ext="${fn##*.}"
	fn2="${fn%.*}"
	echo "f=$f fn=$fn ext=$ext fn2=$fn2"
	g++ `pkg-config --cflags opencv` -o BUILD/showcase_v/$fn2 $f `pkg-config --libs opencv`
done

# step 2: run programs (will generate png images in BUILD/showcase_v/png) and build videos

cd BUILD/showcase_v/
while read -r fname app arg1 arg2 arg3;
do
   if [ "$fname" != "" ] && [ "$fname" != "#" ]; then
	   echo "fname=$fname app=$app arg1=$arg1 arg2=$arg2 arg3=$arg3"
	 	./$app $arg1 $arg2 $arg3
	 	rm png/*.png
	 	mv *.png png/  
	   ffmpeg -y -framerate 8 -s 400x400 -i png/${app}_%02d.png vid/${fname}.mp4
	fi
done < ../../misc/showcase_v/run_parameters.txt



