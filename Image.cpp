#ifndef IMAGECC
#define IMAGECC
#include "Image.h"

double minv1(double v1,double v2){return (v1<v2)?v1:v2;};
double maxv1(double v1,double v2){return -minv1(-v1,-v2);};

Image::Image(int r, int c, int R, int G, int B)
{
	bag=new Bag(true);
	img=NULL;
	init(r,c,R,G,B);
}

Image::Image(char* filename)
{
	bag=new Bag(true);
	img=NULL;
	readfile(filename);
}

Image::Image(Graph& graph, int rows, int cols, double& factor)
{
	img=NULL;

	init(rows*factor,cols*factor,255,255,255);
	const double delta=0.01;
	Edge* root=graph.get_edge_front();
	for(Edge* e=root;e!=NULL;e=e->next)
	{
		for(double q=0; q<=1; q+=delta)
		{
			int dx=roundv((e->from->x*(1-q) + e->to->x*q)*factor);
			int dy=roundv((e->from->y*(1-q) + e->to->y*q)*factor);
			setpixel(dx,dy,180,0,0);
		}
	}
	for(Edge* e=root;e!=NULL;e=e->next)
	{
		setpixel(roundv(e->from->x*factor),
		         roundv(e->from->y*factor),
		         0,0,0);
		setpixel(roundv(e->to->x*factor),
		         roundv(e->to->y*factor),
		         0,0,0);
	}
}

Image::Image(Graph& target, Graph& scene, double rtx, double rty, double rsx, double rsy, double ox, double oy)
{
	img=NULL;

	target.generate_hash_points(rtx,rty,ox,oy);
	scene.generate_hash_points(rsx,rsy,0,0);

	map <int, map <int, bool> > scene_hash_points=scene.get_hash_points();
	map <int, map <int, bool> > target_hash_points=target.get_hash_points();
	map <int, map <int, short> > combined_hash_points; //short = 0,1,2,3 - neither, target only, scene only, both/hit

	int min_x= 1e8,
	    max_x=-1e8,
            min_y= 1e8,
            max_y=-1e8;

	for(map <int, map <int, bool> >::iterator iterX=target_hash_points.begin(); iterX!=target_hash_points.end(); iterX++)
	{
		map <int,bool> row=iterX->second;
		for(map <int, bool>::iterator iterY=row.begin(); iterY!=row.end(); iterY++)
		{
			min_x=(int)minv1(min_x,iterX->first);
			max_x=(int)maxv1(max_x,iterX->first);
			min_y=(int)minv1(min_y,iterY->first);
			max_y=(int)maxv1(max_y,iterY->first);
			combined_hash_points[iterX->first][iterY->first]+=1;
		}
	}

	for(map <int, map <int, bool> >::iterator iterX=scene_hash_points.begin(); iterX!=scene_hash_points.end(); iterX++)
	{
		map <int,bool> row=iterX->second;
		for(map <int, bool>::iterator iterY=row.begin(); iterY!=row.end(); iterY++)
		{
			min_x=(int)minv1(min_x,iterX->first);
			max_x=(int)maxv1(max_x,iterX->first);
			min_y=(int)minv1(min_y,iterY->first);
			max_y=(int)maxv1(max_y,iterY->first);
			combined_hash_points[iterX->first][iterY->first]+=2;
		}
	}

	init(max_x-min_x+1,max_y-min_y+1,255,255,255);
	for(map <int, map <int, short> >::iterator iterX=combined_hash_points.begin(); iterX!=combined_hash_points.end(); iterX++)
	{
		map <int,short> row=iterX->second;
		for(map <int, short>::iterator iterY=row.begin(); iterY!=row.end(); iterY++)
		{
			if(iterY->second==1)
				setpixel(iterX->first-min_x, iterY->first-min_y, 255,   0,   0);
			else if(iterY->second==2)
				setpixel(iterX->first-min_x, iterY->first-min_y,   0,   0, 255);
			else if(iterY->second==3)
				setpixel(iterX->first-min_x, iterY->first-min_y,   0, 255,   0);
		}
	}
}

void Image::readfile(char* filename)
{
	ifstream inf;
	inf.open(filename,ios::binary|ios::in);
	long begin, end;
	begin=inf.tellg();
	inf.seekg(0,ios::end);
	end=inf.tellg();
	inf.seekg(0,ios::beg);
	length=end-begin;
	data=new char[length];
	inf.read(data,length);
	inf.close();
	interpret_data();
}

void Image::writefile(char* filename)
{
	un_interpret_data();
	ofstream ouf;
	ouf.open(filename,ios::binary|ios::out);
	ouf.write(data,length);
	ouf.close();
}


void Image::interpret_data()
{
	int rows=getrows();
	int cols=getcols();
	img=new Pixel* [rows];
	int R, G, B;
	for(int i=0; i<rows; i++)
	{
		img[i]=new Pixel[cols];
		for(int j=0; j<cols; j++)
		{
			getpixel(i,j,R,G,B);
			img[i][j].R=R;
			img[i][j].G=G;
			img[i][j].B=B;
		}
	}
	imgrows=rows;
	imgcols=cols;
}

void Image::un_interpret_data()
{
	int bitsperpixel=24;
	int rows=imgrows;
	int cols=imgcols;

	int rowblock=cols*(bitsperpixel/8);
	if(rowblock%4!=0)
		rowblock+=4-rowblock%4;

	length=rowblock*rows+54;
	if(data!=NULL)
		delete[] data;
	data=new char[length];
	for(int i=0; i<length; i++)
		data[i]=0;

	writedata(0x00,1,'B');				//windows-format bmp
	writedata(0x01,1,'M');				//windows-format bmp, cont'd.
	writedata(0x02,4,length);			//total file size
	writedata(0x0A,4,54);				//offset of raw pixel data
	writedata(0x0E,2,40);				//size of this header (40 bytes)
	writedata(0x12,4,cols);	//bitmap width in pixels
	writedata(0x16,4,rows);	//bitmap height in pixels
	writedata(0x1A,2,1);					//number of color planes being used. hard-coded to 1.
	writedata(0x1C,2,bitsperpixel);	//color depth of image
	writedata(0x1E,2,0);					//compression method being used
	writedata(0x22,4,length-54);		//image size i.e. size of raw bitmap data in bytes
	writedata(0x26,4,0);					//horiz resolution of image, pixels per meter
	writedata(0x2A,4,0);					//vert resolution of image, pixels per meter
	writedata(0x2E,4,0);					//number of colors in color palette, or 0 to default to 2^n
	Pixel p;

	for(int i=0; i<rows; i++)
	for(int j=0; j<cols; j++)
	{
		p=getPixel(rows-i-1,j);
		writedata(54+rowblock*i+3*j+0,1,p.B);
		writedata(54+rowblock*i+3*j+1,1,p.G);
		writedata(54+rowblock*i+3*j+2,1,p.R);
	}
}

int Image::datablock()
	{ return interpret_int( 0xA,4); } //location where pixel data begins
int Image::rowblock()
	{ return interpret_int(0x22,4)/getrows(); } //bytes per row incl padding
int Image::bits_per_pixel()
	{ return interpret_int(0x1C,2); }
int Image::getrows()
	{ return interpret_int(0x16,4); } //these are stored bottom to top
int Image::getcols()
	{ return interpret_int(0x12,4); } //stored left to right
int Image::getsize()
	{ return interpret_int( 0x2,4); } //total file size

int Image::interpret_int(int loc, int size)
{
	int ret=0;
	for(int i=size-1; i>=0; i--)
	{
		int piece=(((int)data[loc+i])&255)<<(i*8);
		ret=ret|piece;
	}
	return ret;
}

void Image::printpixel(int row, int col)
{
	cout<<"["<<((int)(255)&(img[row][col].R))<<",";
	cout<<((int)(255)&(img[row][col].G))<<",";
	cout<<((int)(255)&(img[row][col].B))<<"]\n";
}

void Image::setpixel(int r, int c, int R, int G, int B)
{
	img[r][c].B = (char)B;
	img[r][c].G = (char)G;
	img[r][c].R = (char)R;
}

void Image::getpixel(int r, int c, int& R, int& G, int& B)
{
	int mem=datablock()+rowblock()*(getrows()-r-1)+c*3;
	B=(int)(255)&data[mem+0];
	G=(int)(255)&data[mem+1];
	R=(int)(255)&data[mem+2];
}

bool Image::pixel_is_black(int r, int c)
{
	return img[r][c].R==0 && img[r][c].G==0 && img[r][c].B==0;
}

void Image::writedata(int pos, int len, int value)
{
	for(int i=0; i<len; i++)
	{
		data[pos+i]=(char)(255&value);
		value=value>>8;
	}
}

void Image::init(int rows, int cols, int R, int G, int B) //initialize size and color
{
	if(img!=NULL)
		delete[] img;
	img=new Pixel* [rows];
	for(int i=0; i<rows; i++)
	{
		img[i]=new Pixel[cols];
		for(int j=0; j<cols; j++)
		{
			setpixel(i,j,R,G,B);
		}
	}
	imgrows=rows;
	imgcols=cols;
	data=NULL;
}

void Image::init_resize(Image& orig, int factor)
{
	int rows=orig.getrowcount();
	int cols=orig.getcolcount();
	init(rows*factor,cols*factor, 255, 255, 255);
	for(int i=0; i<rows; i++)
	for(int j=0; j<cols; j++)
	for(int k=0; k<factor; k++)
	for(int l=0; l<factor; l++)
	{
		Pixel p=orig.getrawpixel(i,j);
		setpixel(i*factor+k, j*factor+l,p.R,p.G,p.B);
	}

	if(factor==3)
	for(int i=0; i<rows; i++)
	for(int j=0; j<cols; j++)
	{
		Pixel p=orig.getrawpixel(i,j);
		if(p.NW.x!=-1)	drawline(i*factor  , j*factor  , p.NW.x*factor+2, p.NW.y*factor+2, 255,   0,   0);
		if(p.N .x!=-1)	drawline(i*factor  , j*factor+1, p.N .x*factor+2, p.N .y*factor+1,   0, 255,   0);
		if(p.NE.x!=-1)	drawline(i*factor  , j*factor+2, p.NE.x*factor+2, p.NE.y*factor  ,   0,   0, 255);

		if(p.W.x!=-1)	drawline(i*factor+1, j*factor  , p. W.x*factor+1, p. W.y*factor+2, 255, 128,   0);
		if(p.E.x!=-1)	drawline(i*factor+1, j*factor+2, p. E.x*factor+1, p. E.y*factor  , 128, 128, 128);

		if(p.SW.x!=-1)	drawline(i*factor+2, j*factor  , p.SW.x*factor  , p.SW.y*factor+2, 255, 255,   0);
		if(p.S .x!=-1)	drawline(i*factor+2, j*factor+1, p.S .x*factor  , p.S .y*factor+1, 255,   0, 255);
		if(p.SE.x!=-1)	drawline(i*factor+2, j*factor+2, p.SE.x*factor  , p.SE.y*factor  ,   0, 255, 255);
	}
}

Image Image::trace()
{
	Image ret(imgrows-1,imgcols-1,255,255,255);

	//threshold: nn% of the values seen are at or below this level,
	//so leave them as white
	//during second pass this var will contain the actual value
	//to compare against
	double threshold=.72;
	
	//pass through once to collect stats about color distribution
	//then pass through a second time to determine which colors
	//qualify as significant points
	for(int pass=0; pass<2; pass++)
	{
		for(int i=0; i<imgrows-1; i++)
		for(int j=0; j<imgcols-1; j++)
		{
			double t1=colordist(img[i][j],img[i  ][j+1]);
			double t2=colordist(img[i][j],img[i+1][j+1]);
			double t3=colordist(img[i][j],img[i+1][j  ]);
			double tmax = max(t1,t2,t3);
			if(pass==0)
				bag->insert(tmax);		//pass 1
			else if(tmax>threshold)
				ret.setpixel(i,j,0,0,0);	//pass 2
		}
		if(pass==0)
			threshold=bag->index((int)(bag->size()*threshold)-1);
	}
	ret.build_meta_graph();
	return ret;
}

void Image::build_meta_graph()
{
	for(int i=0; i<imgrows; i++)
	for(int j=0; j<imgcols; j++)
	{
		img[i][j].x=i;
		img[i][j].y=j;
		bool has_south=i<imgrows-1;
		bool has_north=i>0;
		bool has_east=j<imgcols-1;
		bool has_west=j>0;

		setCoord(img[i][j].N,i-1,j  ,pixel_is_black(i,j) && has_north && pixel_is_black(i-1,j  ));
		setCoord(img[i][j].S,i+1,j  ,pixel_is_black(i,j) && has_south && pixel_is_black(i+1,j  ));
		setCoord(img[i][j].W,i  ,j-1,pixel_is_black(i,j) && has_west  && pixel_is_black(i  ,j-1));
		setCoord(img[i][j].E,i  ,j+1,pixel_is_black(i,j) && has_east  && pixel_is_black(i  ,j+1));

		setCoord(img[i][j].NW,i-1,j-1,pixel_is_black(i,j) && has_north && has_west && pixel_is_black(i-1,j-1));
		setCoord(img[i][j].SE,i+1,j+1,pixel_is_black(i,j) && has_south && has_east && pixel_is_black(i+1,j+1));
		setCoord(img[i][j].SW,i+1,j-1,pixel_is_black(i,j) && has_south && has_west && pixel_is_black(i+1,j-1));
		setCoord(img[i][j].NE,i-1,j+1,pixel_is_black(i,j) && has_north && has_east && pixel_is_black(i-1,j+1));
	}

	Pixel** img2=new Pixel*[imgrows];

for(int qq=0; qq<2; qq++)
{
	for(int i=0; i<imgrows; i++)
	{
		img2[i]=new Pixel[imgcols];
		for(int j=0; j<imgcols; j++)
			img2[i][j]=img[i][j];

	}

	for(int i=0; i<imgrows; i++)
	for(int j=0; j<imgcols; j++)
		//completely surrounded, or completely surrounded minus one corner
		if((img[i][j].N .x!=-1 && img[i][j].S .x!=-1) &&
		   (img[i][j].W .x!=-1 && img[i][j].E .x!=-1))
		{
			int count=0;
			if (img[i][j].NW.x!=-1) count++;
			if (img[i][j].SE.x!=-1) count++;
			if (img[i][j].SW.x!=-1) count++;
			if (img[i][j].NE.x!=-1) count++;
			if(count>=4)
			{
				//remove all inbound links
				if(img2[i][j].N .x!=-1) img2[img2[i][j].N .x][img2[i][j].N .y].S .x=-1;
				if(img2[i][j].S .x!=-1) img2[img2[i][j].S .x][img2[i][j].S .y].N .x=-1;
				if(img2[i][j].W .x!=-1) img2[img2[i][j].W .x][img2[i][j].W .y].E .x=-1;
				if(img2[i][j].E .x!=-1) img2[img2[i][j].E .x][img2[i][j].E .y].W .x=-1;
				if(img2[i][j].NW.x!=-1) img2[img2[i][j].NW.x][img2[i][j].NW.y].SE.x=-1;
				if(img2[i][j].SE.x!=-1) img2[img2[i][j].SE.x][img2[i][j].SE.y].NW.x=-1;
				if(img2[i][j].SW.x!=-1) img2[img2[i][j].SW.x][img2[i][j].SW.y].NE.x=-1;
				if(img2[i][j].NE.x!=-1) img2[img2[i][j].NE.x][img2[i][j].NE.y].SW.x=-1;
	
				erasepixel(img2,i,j);
			}
		}

	for(int i=0; i<imgrows; i++)
	for(int j=0; j<imgcols; j++)
		//completely isolated
		if(pixel_is_black(i,j) &&
		   (img[i][j].N .x==-1 && img[i][j].S .x==-1) &&
		   (img[i][j].W .x==-1 && img[i][j].E .x==-1) &&
		   (img[i][j].NW.x==-1 && img[i][j].SE.x==-1) &&
		   (img[i][j].SW.x==-1 && img[i][j].NE.x==-1))
			erasepixel(img2,i,j);

	for(int i=0; i<imgrows; i++)
	for(int j=0; j<imgcols; j++)
		//"northern coast"
		if(true               && img[i][j].N.x==-1 && true               &&
		   img[i][j].W .x!=-1 &&                      img[i][j].E .x!=-1 &&
		   img[i][j].SW.x!=-1 && img[i][j].S.x!=-1 && img[i][j].SE.x!=-1 )
		{
			//link up new neighbors
			if(img2[i][j].E .x!=-1) img2[img2[i][j].E.x][img2[i][j].E.y].W=img2[i][j].W;
			if(img2[i][j].W .x!=-1) img2[img2[i][j].W.x][img2[i][j].W.y].E=img2[i][j].E;

			//remove all inbound links
			if(img2[i][j].SE.x!=-1)	img2[img2[i][j].SE.x][img2[i][j].SE.y].NW.x=-1;
			if(img2[i][j].SW.x!=-1) img2[img2[i][j].SW.x][img2[i][j].SW.y].NE.x=-1;
			if(img2[i][j].S .x!=-1) img2[img2[i][j].S .x][img2[i][j].S .y].N .x=-1;
			if(img2[i][j].NW.x!=-1) img2[img2[i][j].NW.x][img2[i][j].NW.y].SE.x=-1;
			if(img2[i][j].NE.x!=-1) img2[img2[i][j].NE.x][img2[i][j].NE.y].SW.x=-1;

			erasepixel(img2,i,j);
		}

	for(int i=0; i<imgrows; i++)
	for(int j=0; j<imgcols; j++)
		//"southern coast"
		if(img[i][j].NW.x!=-1 && img[i][j].N.x!=-1 && img[i][j].NE.x!=-1 &&
		   img[i][j].W .x!=-1 &&                      img[i][j].E .x!=-1 &&
		   true               && img[i][j].S.x==-1 && true )
		{
			//link up new neighbors
			if(img2[i][j].E .x!=-1) img2[img2[i][j].E.x][img2[i][j].E.y].W=img2[i][j].W;
			if(img2[i][j].W .x!=-1) img2[img2[i][j].W.x][img2[i][j].W.y].E=img2[i][j].E;

			//remove all inbound links
			if(img2[i][j].NW.x!=-1) img2[img2[i][j].NW.x][img2[i][j].NW.y].SE.x=-1;
			if(img2[i][j].N .x!=-1) img2[img2[i][j].N .x][img2[i][j].N .y].S .x=-1;
			if(img2[i][j].NE.x!=-1)	img2[img2[i][j].NE.x][img2[i][j].NE.y].SW.x=-1;
			if(img2[i][j].SE.x!=-1)	img2[img2[i][j].SE.x][img2[i][j].SE.y].NW.x=-1;
			if(img2[i][j].SW.x!=-1) img2[img2[i][j].SW.x][img2[i][j].SW.y].NE.x=-1;


			erasepixel(img2,i,j);
		}

	for(int i=0; i<imgrows; i++)
	for(int j=0; j<imgcols; j++)
		//"eastern coast"
		if(img[i][j].NW.x!=-1 && img[i][j].N.x!=-1 && true &&
		   img[i][j].W .x!=-1 &&                      img[i][j].E .x==-1 &&
		   img[i][j].SW.x!=-1 && img[i][j].S.x!=-1 && true )
		{
			//link up new neighbors
			if(img2[i][j].N .x!=-1) img2[img2[i][j].N.x][img2[i][j].N.y].S=img2[i][j].S;
			if(img2[i][j].S .x!=-1) img2[img2[i][j].S.x][img2[i][j].S.y].N=img2[i][j].N;

			//remove all inbound links
			if(img2[i][j].NW.x!=-1)	img2[img2[i][j].NW.x][img2[i][j].NW.y].SE.x=-1;
			if(img2[i][j].SW.x!=-1) img2[img2[i][j].SW.x][img2[i][j].SW.y].NE.x=-1;
			if(img2[i][j].W .x!=-1) img2[img2[i][j].W .x][img2[i][j].W .y].E .x=-1;
			if(img2[i][j].NE.x!=-1)	img2[img2[i][j].NE.x][img2[i][j].NE.y].SW.x=-1;
			if(img2[i][j].SE.x!=-1)	img2[img2[i][j].SE.x][img2[i][j].SE.y].NW.x=-1;

			erasepixel(img2,i,j);
		}

	for(int i=0; i<imgrows; i++)
	for(int j=0; j<imgcols; j++)
		//"western coast"
		if(true               && img[i][j].N.x!=-1 && img[i][j].NE.x!=-1 &&
		   img[i][j].W .x==-1 &&                      img[i][j].E .x!=-1 &&
		   true               && img[i][j].S.x!=-1 && img[i][j].SE.x!=-1 )
		{
			//link up new neighbors
			if(img2[i][j].N .x!=-1) img2[img2[i][j].N.x][img2[i][j].N.y].S=img2[i][j].S;
			if(img2[i][j].S .x!=-1) img2[img2[i][j].S.x][img2[i][j].S.y].N=img2[i][j].N;

			//remove all inbound links
			if(img2[i][j].NE.x!=-1)	img2[img2[i][j].NE.x][img2[i][j].NE.y].SW.x=-1;
			if(img2[i][j].SE.x!=-1) img2[img2[i][j].SE.x][img2[i][j].SE.y].NW.x=-1;
			if(img2[i][j].E .x!=-1) img2[img2[i][j].E .x][img2[i][j].E .y].W .x=-1;
			if(img2[i][j].NW.x!=-1)	img2[img2[i][j].NW.x][img2[i][j].NW.y].SE.x=-1;
			if(img2[i][j].SW.x!=-1) img2[img2[i][j].SW.x][img2[i][j].SW.y].NE.x=-1;

			erasepixel(img2,i,j);
		}



	for(int i=0; i<imgrows; i++)
	for(int j=0; j<imgcols; j++)
		//"N-S bridge"
		if(img[i][j].NW.x==-1  && img[i][j].N.x!=-1 && img[i][j].NE.x==-1 &&
		   img[i][j].W .x==-1  &&                      img[i][j].E .x==-1 &&
		   img[i][j].SW.x==-1  && img[i][j].S.x!=-1 && img[i][j].SE.x==-1 )
		{
			//link up new neighbors
			if(img2[i][j].N .x!=-1) img2[img2[i][j].N.x][img2[i][j].N.y].S=img2[i][j].S;
			if(img2[i][j].S .x!=-1) img2[img2[i][j].S.x][img2[i][j].S.y].N=img2[i][j].N;

			erasepixel(img2,i,j,255,200,255);
		}

	for(int i=0; i<imgrows; i++)
	for(int j=0; j<imgcols; j++)
		//W-E lines
		if(pixel_is_black(i,j) &&
		   img2[i][j].W.x==img2[i][j].x && img2[i][j].E.x==img2[i][j].x &&
		   img2[i][j].NW.x==-1 && img2[i][j].N .x==-1 && img2[i][j].NE.x==-1 && 
		   img2[i][j].SW.x==-1 && img2[i][j].S .x==-1 && img2[i][j].SE.x==-1)
		{
			img2[img2[i][j].W.x][img2[i][j].W.y].E=img2[i][j].E;
			img2[img2[i][j].E.x][img2[i][j].E.y].W=img2[i][j].W;
			erasepixel(img2,i,j,200,255,200);
		}

	for(int i=0; i<imgrows; i++)
	for(int j=0; j<imgcols; j++)
		img[i][j]=img2[i][j];
}
	delete[] img2;
}

int Image::black_pixel_count()
{
	int ret=0;
	for(int i=0; i<imgrows; i++)
	for(int j=0; j<imgcols; j++)
		if(pixel_is_black(i,j))
			ret++;
	return ret;
}

int Image::link_count()
{
	int ret=0;
	for(int i=0; i<imgrows; i++)
	for(int j=0; j<imgcols; j++)
		if(pixel_is_black(i,j))
		{
			ret+=(img[i][j].N .x!=-1)+
			     (img[i][j].S .x!=-1)+
			     (img[i][j].W .x!=-1)+
			     (img[i][j].E .x!=-1)+
			     (img[i][j].NW.x!=-1)+
			     (img[i][j].SE.x!=-1)+
			     (img[i][j].SW.x!=-1)+
			     (img[i][j].NE.x!=-1);
		}
	return ret;
}


void Image::erasepixel(Pixel**& img2, int i, int j)
{
	//remove all outbound links
	img2[i][j].N .x=-1;
	img2[i][j].S .x=-1;
	img2[i][j].E .x=-1;
	img2[i][j].W .x=-1;
	img2[i][j].NW.x=-1;
	img2[i][j].SE.x=-1;
	img2[i][j].SW.x=-1;
	img2[i][j].NE.x=-1;
	img2[i][j].R=200;
	img2[i][j].G=200;
	img2[i][j].B=200;
}
void Image::erasepixel(Pixel **& img2, int i, int j, int _r, int _g, int _b)
{
	erasepixel(img2,i,j);
	img2[i][j].R=_r;
	img2[i][j].G=_g;
	img2[i][j].B=_b;
}

double Image::colordist(Pixel x, Pixel y)
{
	double dR=abs((255l&x.R)-(255l&y.R));
	double dG=abs((255l&x.G)-(255l&y.G));
	double dB=abs((255l&x.B)-(255l&y.B));
	double total=pow(dR,3)+pow(dG,3)+pow(dB,3);
	return total;
}

//If an image is a trace (i.e. all black and white), use this function
//to collect a distribution of black-pixel relationships
//(distance and angle)
void Image::print_trace_stats()
{
	Pixel p,p2;
	for(int i=0; i<getrows(); i++)
	for(int j=0; j<getcols(); j++)
	{
		p=getPixel(i,j);
		if(p.R==0 && p.G==0 && p.B==0) //is black
		{
			for(int i2=i;   i2<getrows(); i2++)
			for(int j2=j+1; j2<getcols(); j2++)
			{
				p2=getPixel(i2,j2);
					if(p2.R==0 && p2.G==0 && p2.B==0) //is black
						cout<<atan2(j2-j,i2-i)<<"\t"<<(1.0*(j2-j)*(j2-j)+(i2-i)*(i2-i))<<endl;
			}	
		}
	}
}

void Image::setCoord(Coord& c, int i, int j, bool b)
{
	c.x=b?i:-1;
	c.y=b?j:-1;
}

void Image::drawline(int x1, int y1, int x2, int y2, int R, int G, int B)
//ONLY for diagonals and straight lines!
{
	if(x1==x2 && y1==y2)
		setpixel(x1,y1,R,G,B);
	else
	{
		int length=0;
		if(x1!=x2) length=abs(x2-x1);
		else if(y1!=y2) length=abs(y2-y1);

		for(int i=0; i<length/2+1; i++)
			setpixel(x1+(x2-x1)*i/length,y1+(y2-y1)*i/length,R,G,B);
	}
}

#endif
