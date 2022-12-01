# preliminary
# build mp4 video from still images
while read -r fname app arg1 arg2 arg3;
do
   echo "vname=$vname vapp=$vapp arg1=$arg1 arg2=$arg2 arg3=$arg3"
   ffmpeg -y -framerate 8 -s 400x400 -i ${app}_%02d.png $arg1 $arg2 $arg3 fname.mp4
done < run_parameters.txt



