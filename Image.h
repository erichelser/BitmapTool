#ifndef __IMAGE__
#define __IMAGE__

#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <math.h>
#include "Bag.h"
#include "Graph.h"

using namespace std;

struct Coord
{
	int x, y;
};

struct Pixel
{
	int x, y;
	char R,G,B;
	Coord N, S, W, E, NW, SE, SW, NE;

};

class Graph;

class Image
{	
	public:
		static double inline abs(double x){return (x<0)?-x:x;};
		static double inline pow(double base, int pow)
			{double ret=1; for(int i=0; i<pow; i++) ret*=base; return ret;};
		Image(int,int,int,int,int);
		Image(char* filename);
		//Image(Image& q){init_resize(q,1);};
		inline Image(Image& q,int r){init_resize(q,r);};
		Image(Graph& graph, int rows, int cols, double& factor);
		Image(Graph& target, Graph& scene, double rtx, double rty, double rsx, double rsy, double ox, double oy);
		void init_resize(Image&,int);
		inline ~Image()
		{
			//if(bag!=NULL) delete bag;
			//if(data!=NULL) delete data;
			//if(img!=NULL) delete[] img;
		};
		void readfile(char* filename);
		void writefile(char* filename);

		void printpixel(int r, int c);
		void setpixel(int r, int c, int  R, int  G, int  B);
		void getpixel(int r, int c, int &R, int &G, int &B);
		bool pixel_is_black(int r, int c);
		Image trace();
		
		int datablock(); //location where pixel data begins
		int rowblock();  //bytes per row incl padding
		int bits_per_pixel(); 
		int getrows();   //these are stored bottom to top
		int getcols();   //stored left to right
		int getsize();   //total file size
		
		int getrowcount(){return imgrows;};   //these methods don't refer to the binary data
		int getcolcount(){return imgcols;};   //so they can be called at any time

		void init(int,int,int,int,int); //initialize size and color
		
		void print_trace_stats();

		void build_meta_graph();

		inline Pixel getrawpixel(int i, int j) { return img[i][j]; };

		void drawline(int, int, int, int, int, int, int);

		int black_pixel_count();
		int link_count();

	private:
		char* data;  //raw file data
		int length;
		
		Pixel** img; //interpreted image data
		int imgrows;
		int imgcols;

		int interpret_int(int loc, int size);
		void interpret_data();
		void un_interpret_data();
		inline Pixel getPixel(int r, int c){return img[r][c];};
		void writedata(int pos, int len, int value);
		double colordist(Pixel, Pixel);
		void erasepixel(Pixel**&, int i, int j);
		void erasepixel(Pixel**&, int i, int j, int _r, int _g, int _b);

		Bag* bag; //for sorting pixel color-dist importance
		
		inline double max(double x, double y, double z){return max(max(x,y),z);};
		inline double max(double x, double y){return x>y?x:y;};
		void setCoord(Coord&, int, int, bool);
		inline void printpixel(Pixel **& p, int i, int j)
		{
			cout<<i<<", "<<j<<endl;
			cout<<"N  "<<p[i][j].N .x<<", "<<p[i][j].N .y<<endl;
			cout<<"S  "<<p[i][j].S .x<<", "<<p[i][j].S .y<<endl;
			cout<<"W  "<<p[i][j].W .x<<", "<<p[i][j].W .y<<endl;
			cout<<"E  "<<p[i][j].E .x<<", "<<p[i][j].E .y<<endl;
			cout<<"NW "<<p[i][j].NW.x<<", "<<p[i][j].NW.y<<endl;
			cout<<"SE "<<p[i][j].SE.x<<", "<<p[i][j].SE.y<<endl;
			cout<<"SW "<<p[i][j].SW.x<<", "<<p[i][j].SW.y<<endl;
			cout<<"NE "<<p[i][j].NE.x<<", "<<p[i][j].NE.y<<endl;
			cout<<endl;
		};
		inline double roundv(double x){return (double)(int)(x+0.5);};
};

#endif
