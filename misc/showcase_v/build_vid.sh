# Build mp4 videos from still images, and generate those images
# Requires ffmpeg, tested with version 4.2.7

# The tasks to be done are in misc/showcase_v/run_parameters.txt

# Run from root level of repo with:
# $ misc/showcase_v/build_vid.sh

# step 1: build programs
mkdir -p BUILD/showcase_v/vid
rm BUILD/showcase_v/vid/*.mp4

for f in misc/showcase_v/*.cpp
do
	fn=$(basename -- "$f")
	ext="${fn##*.}"
	fn2="${fn%.*}"
#	echo "f=$f fn=$fn ext=$ext fn2=$fn2"
	g++ `pkg-config --cflags opencv` -o BUILD/showcase_v/$fn2 $f `pkg-config --libs opencv`
done

# step 2: run programs (will generate png images in BUILD/showcase_v/png) and build videos
# ffmpeg option -nostdin is there to avoid bash variable values to be crunched, see https://unix.stackexchange.com/a/718279/73337

cd BUILD/showcase_v/
while read -r fname app arg1 arg2 arg3;
do
	if [ "$fname" != "" ] && [ "$fname" != "#" ]; then
		./$app $arg1 $arg2 $arg3
		mkdir -p png_${fname}
	 	rm png_${fname}/*.png
	 	mv *.png png_${fname}/
	   ffmpeg -nostdin -y -framerate 12 -s 400x400 -i png_${fname}/${app}_%03d.png vid/${fname}.mp4 1>ffmpeg_${fname}.stdout 2>ffmpeg_${fname}.stderr
	fi
done < ../../misc/showcase_v/run_parameters.txt



