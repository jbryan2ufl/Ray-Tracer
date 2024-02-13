in order to run the program you will need g++ and make.
if you run "make" it should generate the .exe and you can run make.exe from there

to move around you can use WASD, SPACE for up, and SHIFT for down
to move the camera you can use the arrow keys

P will shift between orthographic and perspective
you can select render size which will alter the performance
you can change the render resolution and click "resize" to redraw with a different resolution

under the "Export" tab you can change the export resolution and Save the image
images and video frames save to /images/

the pregenerated report videos and images can be found in /report/

to generate videos place keyframes and click play with the correct export resolution selected
frames will be generated in /images/ and can be compiled with ffmpeg
the ffmpeg command used is below:
	ffmpeg -framerate 24 -i frame_%d.jpg -c:v libx264 -crf 0 output.mp4