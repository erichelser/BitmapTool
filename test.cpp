#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <math.h>
#include <cstring>
#include <cstdlib>

#include "Image.h"
#include "Bag.h"
#include "Graph.h"

using namespace std;
int main(int argc, char* argv[])
{
	if(argc<2 || strcmp(argv[1],"?")==0 || strcmp(argv[1],"-?")==0)
	{
		cout<<"Help:\nUsage:\nImage comparison tool\n\n"

			<<"./program.exe -h filename_in filename_out [inters]\n"
			<<"Read in 24-bit BMP format image from <filename_in> and write the "
			<<"image hash (i.e. numeric pixel coordinates of points of interest) "
			<<"to <filename_out>. An optional numeric value, 'inters' may be provided "
			<<"which adds extra points of interest along edges of interest, between "
			<<"calculated points. For example, if you had two points of interest at 0 and 4, "
			<<"and you set inters to 4, you would also receive 0.8, 1.6, 2.4, and 3.2 as "
			<<"additional points of interest.\n\n"

			<<"./program.exe -i filename_in filename_out [factor]\n"
			<<"Read in 24-bit BMP format image from <filename_in> and write the "
			<<"image hash as a 24-bit BMP format image to <filename_out>. The "
			<<"output image will contain a white background with black dots for points of interest and "
			<<"red lines along edges of interest. Optional argument 'factor' determines how scaled-up "
			<<"the output image should be. For example, a 100x100 image will result in a 200x200 output "
			<<"image when factor is set to 2.\n\n"

			<<"./program.exe -c file1 file2 [file3 file4 file5...]\n"
			<<"Compare the 'target' image hash in <file1> to the 'scene' images "
			<<"<file2>, <file3>, etc. The 'target' image is considered to be a "
			<<"subset of the 'scene' image. Program will return a numeric value indicating "
			<<"image similarity, with a higher number indicating a better match.\n\n"

			<<"./program.exe -q file1 file2 file3 res_scene_x res_scene_y res_target_x res_target_y offset_x offset_y\n"
			<<"Compare target image hash <file1> to scene image hash <file2>, which are transformed "
			<<"by the 6 final arguments, and written out to output <file3>. This output file "
			<<"displays the results of the comparison. Pixels will be one of 4 colors: "
			<<"white if neither input file contains a vertex in that location, "
			<<"red if only the target image contains a vertex there, "
			<<"blue if only the scene image contains a vertex there, "
			<<"and green if both do.\n\n"
			;
	}
	else if(strcmp(argv[1],"h")==0 || strcmp(argv[1],"-h")==0)
	{
		Image f(argv[2]);
		Image g=f.trace();
		Graph target(g);
		target.condense();
		target.smooth();
		if(argc==4)
			target.generate_vertex_list();
		else
			target.generate_vertex_list(atoi(argv[4]));
		target.dump(argv[3]);
	}
	else if(strcmp(argv[1],"i")==0 || strcmp(argv[1],"-i")==0)
	{
		Image f(argv[2]);
		Image g=f.trace();
		Graph target(g);
		target.condense();
		target.smooth();
		target.generate_vertex_list();
		double factor=1;
		if(argc>4)
			factor=atof(argv[4]);
		Image out(target,target.getrows(),target.getcols(),factor);
		out.writefile(argv[3]);
	}
	else if(strcmp(argv[1],"c")==0 || strcmp(argv[1],"-c")==0)
	{
		Graph target(argv[2]); //un-dump

		for(int x=3; x<argc; x++)
		{
			Graph scene(argv[x]);
			double score=scene.compare(target);
			cout<<argv[x]<<" = "<<score<<endl;
		}
	}
	else if(strcmp(argv[1],"q")==0 || strcmp(argv[1],"-q")==0)
	{
		Graph target(argv[2]); //un-dump
		Graph scene (argv[3]); //un-dump

		double res_scene_x=atof(argv[5]);
		double res_scene_y=atof(argv[6]);
		double res_target_x=atof(argv[7]);
		double res_target_y=atof(argv[8]);
		double offset_x=atof(argv[9]);
		double offset_y=atof(argv[10]);

		Image out(target,scene,res_scene_x,res_scene_y,res_target_x,res_target_y,offset_x,offset_y);
		out.writefile(argv[4]);

	}
	return 0;
}
