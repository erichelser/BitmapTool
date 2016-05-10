# BitmapTool

Analyze bitmap images

Usage:
Image comparison tool

./test.exe -h filename_in filename_out [inters]
Read in 24-bit BMP format image from <filename_in> and write the image hash (i.e. numeric pixel coordinates of points of interest) to <filename_out>. An optional numeric value, 'inters' may be provided which adds extra points of interest along edges of interest, between calculated points. For example, if you had two points of interest at 0 and 4, and you set inters to 4, you would also receive 0.8, 1.6, 2.4, and 3.2 as additional points of interest.

./test.exe -i filename_in filename_out [factor]
Read in 24-bit BMP format image from <filename_in> and write the image hash as a 24-bit BMP format image to <filename_out>. The output image will contain a white background with black dots for points of interest and red lines along edges of interest. Optional argument 'factor' determines how scaled-up the output image should be. For example, a 100x100 image will result in a 200x200 output image when factor is set to 2.

./test.exe -c file1 file2 [file3 file4 file5...]
Compare the 'target' image hash in <file1> to the 'scene' images <file2>, <file3>, etc. The 'target' image is considered to be a subset of the 'scene' image. Program will return a numeric value indicating image similarity, with a higher number indicating a better match.

./test.exe -q file1 file2 file3 res_scene_x res_scene_y res_target_x res_target_y offset_x offset_y
Compare target image hash <file1> to scene image hash <file2>, which are transformed by the 6 final arguments, and written out to output <file3>. This output file displays the results of the comparison. Pixels will be one of 4 colors: white if neither input file contains a vertex in that location, red if only the target image contains a vertex there, blue if only the scene image contains a vertex there, and green if both do.

