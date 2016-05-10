#ifndef __GRAPHCC
#define __GRAPHCC
#include "Graph.h"
#include <map>
using namespace std;

double absv(double v)
{
	return v<0?-v:v;
};
double minv(double v1,double v2)
{
	return (v1<v2)?v1:v2;
};
double maxv(double v1,double v2)
{
	return -minv(-v1,-v2);
};
double squarev(double x)
{
	return x*x;
};
double modv(double a, double b)
{
	return a-b*(int)(a/b);
};
double powv(double x, int y)
{
	if(y<0)
		return 1/powv(x,-y);
	if(y>1)
		return x*pow(x,y-1);
	return x;
};
double angle(double ax, double ay, double bx, double by)
{
	return (by-ay==0 && bx-ax==0)?0:modv(atan2(by-ay,bx-ax)/3.1415926*180+360,360);
};
double anglediff(Vertex* A1, Vertex* B1, Vertex* A2, Vertex* B2)
{
	return angle(A1->x,A1->y,B1->x,B1->y)-
	       angle(A2->x,A2->y,B2->x,B2->y);
};
double anglescore(Edge* A, bool A_forward, Edge* B, bool B_forward)
{
	double diff=0;
	if(A_forward)
	{
		if(B_forward)
			diff=anglediff(A->from,A->to,B->from,B->to);
		else
			diff=anglediff(A->from,A->to,B->to,B->from);
	}
	else
	{
		if(B_forward)
			diff=anglediff(A->to,A->from,B->from,B->to);
		else
			diff=anglediff(A->to,A->from,B->to,B->from);
	}
	return powv(0.5+0.5*cos(diff/180*3.1415926),6);
};

char* Graph::tochar(double x, int precision)
//x = value to convert
//precision = number of places right of decimal to display
{
	static char* buf=new char[256];

	int pos=0;

	if(x<0)
	{
		buf[pos++]='-';
		x*=-1;
	}

	int counter=0;
	while(x>=10)
	{
		counter++;
		x=x/10;
	}

	do
	{
		buf[pos++]=(0x30 + (int)(x));
		if(counter==0 && precision!=0)
			buf[pos++]='.';
		counter--;
		x=(x-(int)(x))*10;
		if(counter<0)
		{
			if(precision==0)
				break;
			else
				precision--;
		}
	}while(1);
	buf[pos++]=0;
	return buf;
};

Graph::Graph()
{
	e_front=0;
	e_back=0;
	v_front=0;
	v_back=0;
	rows=0;
	cols=0;
}

Graph::Graph(Image& img)
{
	e_front=0;
	e_back=0;
	v_front=0;
	v_back=0;
	rows=img.getrowcount();
	cols=img.getcolcount();
	for(int i=0; i<rows; i++)
	for(int j=0; j<cols; j++)
	{
		if(img.pixel_is_black(i,j))
			addVertex(i,j,1);
	}
	for(int i=0; i<rows; i++)
	for(int j=0; j<cols; j++)
	{
		if(img.pixel_is_black(i,j))
		{
			Pixel p=img.getrawpixel(i,j);
			Vertex* from=findVertex(i,j);
			if(p.N .x!=-1) addEdge(from,p.N .x,p.N .y);
			if(p.NE.x!=-1) addEdge(from,p.NE.x,p.NE.y);
			if(p. E.x!=-1) addEdge(from,p. E.x,p. E.y);
			if(p.SE.x!=-1) addEdge(from,p.SE.x,p.SE.y);
			//only do these four since every path is two-way already
		}
	}
}

Vertex* Graph::addVertex(double i, double j, int w)
{
	Vertex* v=new Vertex;
	v->x=i;
	v->y=j;
	v->weight=w;
	v->next=NULL;
	if(v_front==NULL)
	{
		v_front=v;
		v_back=v;
	}
	else
	{
		v_back->next=v;
		v_back=v_back->next;
	}
	return v;
}

Vertex* Graph::findVertex(double i, double j)
{
	for(Vertex* v=v_front;v!=NULL;v=v->next)
		if(v->x==i && v->y==j)
			return v;
	return NULL;
}

bool Graph::addEdge(Vertex* from, double i, double j)
{
	Vertex* to=findVertex(i,j);
	return addEdge(from,to);
}
bool Graph::addEdge(Vertex* _from, Vertex* _to)
{
	for(Edge* iter=e_front; iter!=NULL; iter=iter->next)
		if((iter->from==_from && iter-> to ==_to) ||
		   (iter-> to ==_from && iter->from==_to))
		{
			return false; //edge already exists
		}

	Edge* e = new Edge;
	e->from=_from;
	e->to=_to;
	e->next=0;

	if(e_front==NULL)
	{
		e_front=e;
		e_back=e;
	}
	else
	{
		e_back->next=e;
		e_back=e_back->next;
	}
	return true;
}

void Graph::removeEdge(Edge* e)
{
	if(e_front==e)
	{
		if(e_front==e_back)
			e_back=NULL;
		e_front=e_front->next;
	}
	else
	{
		Edge* iter;
		for(iter=e_front; iter!=NULL && iter->next!=e; iter=iter->next)
			;
		if(e_back==e)
			e_back=iter;
		if(iter!=NULL)
			iter->next=iter->next->next;
	}
	delete e;
}

void Graph::removeVertex(Vertex* v)
{
	if(v_front==v)
	{
		if(v_front==v_back)
			v_back=NULL;
		v_front=v_front->next;
	}
	else if(v_back==v)
	{
		Vertex* iter;
		for(iter=v_front; iter!=NULL && iter->next!=v; iter=iter->next);
		if(iter!=NULL)
		{
			v_back=iter;
			iter->next=NULL;
		}
	}
	else
	{
		Vertex* iter;
		for(iter=v_front; iter!=NULL && iter->next!=v; iter=iter->next);
		if(iter!=NULL)
			iter->next=iter->next->next;
	}
	delete v;
}

void Graph::condense()
{
	Edge* min_edge=0;
	double min_dist;
	const double max_min_dist=squarev(minv(rows,cols)*0.005);
	do
	{
		min_dist=max_min_dist;
		for(Edge* iter=e_front; iter!=NULL; iter=iter->next)
		{
			double this_dist=(iter->from->x - iter->to->x) *
			                 (iter->from->x - iter->to->x) +
			                 (iter->from->y - iter->to->y) *
			                 (iter->from->y - iter->to->y);
			if(this_dist<min_dist)
			{
				min_dist=this_dist;
				min_edge=iter;
			}
		}

		if(min_dist<max_min_dist)
		{
			Vertex* new_v=addVertex(
			        (min_edge->from->x * min_edge->from->weight + min_edge->to->x * min_edge->to->weight)
			        / (min_edge->from->weight + min_edge->to->weight),

			        (min_edge->from->y * min_edge->from->weight + min_edge->to->y * min_edge->to->weight)
			        / (min_edge->from->weight + min_edge->to->weight),

			        min_edge->from->weight + min_edge->to->weight);

			//replace all edges referencing min_edge->to or min_edge->from
			Edge* iternext=NULL;
			for(Edge* iter=e_front; iter!=NULL; iter=iternext)
			{
				iternext=iter->next;
				if(iter==min_edge)
				{
					continue;
				}
				else if(iter->from==min_edge->from || iter->from==min_edge->to)
				{
					addEdge(new_v,iter->to);
					removeEdge(iter);
				}
				else if(iter->to==min_edge->to || iter->to==min_edge->from)
				{
					addEdge(iter->from,new_v);
					removeEdge(iter);
				}
			}
			removeVertex(min_edge->from);
			removeVertex(min_edge->to);
			removeEdge(min_edge);
		}
	}while(min_dist<max_min_dist);
}

void Graph::dump()
{
	for(Edge* iter=e_front; iter!=NULL; iter=iter->next)
	{
		cout<<iter->from->x<<"\t"<<iter->from->y<<"\t"<<iter->from->weight<<"\t -> \t";
		cout<<iter->to->x<<"\t"<<iter->to->y<<"\t"<<iter->to->weight<<'\t';

		cout<<iter<<'\t';
		cout<<iter->next<<'\t';

		cout<<endl;
		double delta=0.2;
		for(double q=delta; q<1; q+=delta)
			cout<<"\t\t\t\t\t\t\t\t\t"
			    <<((iter->from->x)*(1-q)+(iter->to->x)*q)<<"\t"
			    <<((iter->from->y)*(1-q)+(iter->to->y)*q)<<endl;
	}
}

void Graph::dump(char* file)
{
	ofstream ouf(file);
	for(map<Vertex*,bool>::iterator iter=vertex_list.begin(); iter!=vertex_list.end(); iter++)
	{
		ouf<<iter->first->x<<"\t"<<iter->first->y<<endl;
	}
	ouf.close();
}

Graph::Graph(char* file)
{
	ifstream inf(file,ios::in);
	double X, Y;
	vertex_list.clear();
	while(inf>>X)
	{
		inf>>Y;
		Vertex* v = new Vertex;
		v->x=X;
		v->y=Y;
		vertex_list[v]=true;
	}
	inf.close();
}

void Graph::smooth()
{
	//This function is to eliminate pairs of adjacent edges that form a nearly
	//straight angle, and replace them with one straight edge.

	//1. Find a Vertex that's shared by exactly two distinct Edges
	//2. Determine the angle formed by the Edges (atan2)
	//3. Add an edge between the mutually exclusive Vertexes
	//4. Remove the two original Edges
	//5. Repeat until no candidate Edge pairs are found

	const double TOLERANCE=20;
	bool repeat_loop;
	int timer=-1;
	do
	{
		repeat_loop=false;
		Edge* iternext;
		for(Edge* iter=e_front; iter!=NULL; iter=iternext)
		{
			iternext=iter->next;
			Edge* pair=findEdgePair(iter->from,iter->next);
			if(pair!=NULL)
			{
				if(getAngle(pair,iter)<TOLERANCE)
				{
					if(pair==iternext) iternext=iternext->next;
					removeEdgePair(pair,iter);
					repeat_loop=true;
					timer--;
					if(timer==0) break;
					continue;
				}
			}

			pair=findEdgePair(iter->to,iter->next);

			if(pair!=NULL)
			{

				if(getAngle(pair,iter)<TOLERANCE)
				{
					if(pair==iternext) iternext=iternext->next;
					removeEdgePair(pair,iter);
					repeat_loop=true;
					timer--;
					if(timer==0) break;
					continue;
				}
			}
		}
	}while(repeat_loop && false);
}

Edge* Graph::findEdgePair(Vertex* v, Edge* beginhere)
{
	Edge* e=NULL;
	for(Edge* iter=beginhere; iter!=NULL; iter=iter->next)
	{
		if(iter->from==v || iter->to==v)
		{
			if(e==NULL)
				e=iter;
			else
				return NULL; //found 2 more edges that share point, quit.
		}
	}
	return e;
}

double Graph::getAngle(Edge* a, Edge* b)
{
	const double PI=3.1415926;
	double heading_a = atan2(a->to->y - a->from->y, a->to->x - a->from->x) *180.0/PI;
	double heading_b = atan2(b->to->y - b->from->y, b->to->x - b->from->x) *180.0/PI;

	if(a->from==b->from || a->to==b->to)
		return absv(modv(heading_b-heading_a+720,360)-180);

	return absv(modv(heading_b-heading_a+540,360)-180);
}

double Graph::absv(double x)
{
	return (x<0)?-x:x;
}

double Graph::modv(double x, double y)
{
	if(y==0) return 0;
	return x-y*(int)(x/y);
}

void Graph::removeEdgePair(Edge* A, Edge* B)
{
	if(A->from==B->from)    addEdge(A->to,B->to);
	else if(A->to==B->from) addEdge(A->from,B->to);
	else if(A->from==B->to) addEdge(A->to,B->from);
	else if(A->to==B->to)   addEdge(A->from,B->from);

	removeEdge(A);
	removeEdge(B);
}

double Vdist(Vertex* O, Vertex* A) { double dy=A->y-O->y; double dx=A->x-O->x; return sqrt(dy*dy+dx*dx); } ;
double Vdist(Vertex* O, Vertex* A,double S,double VX, double VY) { double dy=A->y-(O->y*S+VY); double dx=A->x-(O->x*S+VX); return sqrt(dy*dy+dx*dx); } ;

double calc_line_dist(Edge* A, Edge* B, double S, double VX, double VY)
{
	double type1=(Vdist(A->from,B->from,S,VX,VY)+Vdist(A->to,B->to,S,VX,VY))/2;
	double type2=(Vdist(A->from,B->to,S,VX,VY)+Vdist(A->to,B->from,S,VX,VY))/2;
	return minv(type1,type2);
}

double grid_compare(Graph& scene, Graph& target,
                    double resolution_scene_x, double resolution_scene_y,
                    double resolution_target_x, double resolution_target_y,
                    double offset_x, double offset_y, double& hit_count, double& miss_count, bool verbose);

double Graph::compare(Graph& target)
{

/*
cout<<"rs="<<'\t';
cout<<"rt="<<'\t';
cout<<"oxy="<<'\t';
cout<<"mstep="<<'\t';
cout<<"sA="<<'\t';
cout<<"sB="<<'\t';
cout<<"sC="<<'\t';
cout<<endl;
*/

	double max_rsx, max_rsy,
	       max_rtx, max_rty,
	       max_ox, max_oy,
	       max_h, max_m,
	       max_score=-1;	

	double hit_count, miss_count;
	double resolution_scene_x=2, resolution_scene_y=2,
	       resolution_target_x=2, resolution_target_y=2,
	       offset_x=0, offset_y=20;

	for(resolution_scene_x=.4; max_score<90 && resolution_scene_x<=3; resolution_scene_x*=1.3)
	for(resolution_scene_y=resolution_scene_x*1; max_score<90 && resolution_scene_y<=resolution_scene_x*1; resolution_scene_y+=resolution_scene_x/5.0)
	for(resolution_target_x=.4; max_score<90 && resolution_target_x<=3; resolution_target_x*=1.3)
	for(resolution_target_y=resolution_target_x*1; max_score<90 && resolution_target_y<=resolution_target_x*1; resolution_target_y+=resolution_target_x/5.0)
	for(offset_x=-20; max_score<90 && offset_x<=-20; offset_x+=10)
	for(offset_y=-70; max_score<90 && offset_y<=-70; offset_y+=10)
	{
		double score=grid_compare(*this,target,
	                    resolution_scene_x,resolution_scene_y,
	                    resolution_target_x, resolution_target_y,
	                    offset_x,offset_y,
	                    hit_count,miss_count,false);
		if(score>max_score)
		{
			max_rsx=resolution_scene_x;
			max_rsy=resolution_scene_y;
			max_rtx=resolution_target_x;
			max_rty=resolution_target_y;
			max_ox=offset_x;
			max_oy=offset_y;
			max_score=score;

cout<<resolution_scene_x<<'/';
cout<<resolution_scene_y<<'\t';
cout<<resolution_target_x<<'/';
cout<<resolution_target_y<<'\t';
cout<<offset_x<<'/';
cout<<offset_y<<'\t';
cout<<score<<'\t';
cout<<endl;

		}
	}

	resolution_scene_x=max_rsx;
	resolution_scene_y=max_rsy;
	resolution_target_x=max_rtx;
	resolution_target_y=max_rty;
	offset_x=max_ox;
	offset_y=max_oy;

	double* maximizer;
	double m_step;
	double scoreA, scoreB, scoreC;
	double recalcA=true, recalcC=true;
	scoreB=grid_compare(*this,target,
	                    resolution_scene_x,resolution_scene_y,
	                    resolution_target_x, resolution_target_y,
	                    offset_x,offset_y,
	                    hit_count,miss_count,false);
	maximizer=&offset_x;
	int laps=1;

	while(maximizer!=NULL)
	{
		m_step=10;
		do
		{
			if(recalcA)
			{
				(*maximizer)-=m_step;
				scoreA=grid_compare(*this,target,
			                          resolution_scene_x,resolution_scene_y,
			                          resolution_target_x, resolution_target_y,
			                          offset_x,offset_y,
			                          hit_count,miss_count,false);
				(*maximizer)+=m_step;
			}
			if(recalcC)
			{
				(*maximizer)+=m_step;
				scoreC=grid_compare(*this,target,
			                          resolution_scene_x,resolution_scene_y,
			                          resolution_target_x, resolution_target_y,
			                          offset_x,offset_y,
			                          hit_count,miss_count,false);
				(*maximizer)-=m_step;
			}

			if(scoreA>scoreB)
			{
				(*maximizer)-=m_step;
				scoreC=scoreB;
				scoreB=scoreA;
				recalcA=true;
				recalcC=false;
			}
			else if(scoreC>scoreB)
			{
				(*maximizer)+=m_step;
				scoreA=scoreB;
				scoreB=scoreC;
				recalcA=false;
				recalcC=true;
			}
			else
			{
				m_step/=2;
				recalcA=true;
				recalcC=true;
			}
		}while(m_step>0.001);
/*
cout<<resolution_scene_x<<'/';
cout<<resolution_scene_y<<'\t';
cout<<resolution_target_x<<'/';
cout<<resolution_target_y<<'\t';
cout<<offset_x<<'/';
cout<<offset_y<<'\t';
cout<<scoreB<<'\t';
cout<<endl;
*/
		if(maximizer==&offset_x) maximizer=&offset_y;
		else if(maximizer==&offset_y) maximizer=&resolution_target_x;
		else if(maximizer==&resolution_target_x) maximizer=&resolution_target_y;
		else if(maximizer==&resolution_target_y) maximizer=&resolution_scene_x;
		else if(maximizer==&resolution_scene_x) maximizer=&resolution_scene_y;
		else if(maximizer==&resolution_scene_y) maximizer=NULL;

		if(maximizer==NULL && laps>1)
		{
			maximizer=&offset_x;
			laps--;
		}
	}
/*
	grid_compare(*this,target,
	             resolution_scene_x,resolution_scene_y,
	             resolution_target_x, resolution_target_y,
	             offset_x,offset_y,
	             hit_count,miss_count,true);
*/
cout<<resolution_scene_x<<'/';
cout<<resolution_scene_y<<'\t';
cout<<resolution_target_x<<'/';
cout<<resolution_target_y<<'\t';
cout<<offset_x<<'/';
cout<<offset_y<<'\t';
cout<<scoreB<<'\t';
cout<<endl;

	return scoreB;
}
double grid_compare(Graph& scene, Graph& target, double resolution_scene_x, double resolution_scene_y,
                    double resolution_target_x, double resolution_target_y,
                    double offset_x, double offset_y, double& hit_count, double& miss_count, bool verbose)
{
	hit_count=0;
	miss_count=0;

	scene.generate_hash_points(resolution_scene_x,resolution_scene_y,0,0);
	target.generate_hash_points(resolution_target_x,resolution_target_y,offset_x,offset_y);

	map <int, map <int, bool> > scene_hash_points=scene.get_hash_points();
	map <int, map <int, bool> > target_hash_points=target.get_hash_points();

	int min_x= 1e8,
	    max_x=-1e8,
            min_y= 1e8,
            max_y=-1e8;

if(verbose) cout<<"Target points:\n";
	for(map <int, map <int, bool> >::iterator iterX=target_hash_points.begin(); iterX!=target_hash_points.end(); iterX++)
	{
		map <int,bool> row=iterX->second;
		for(map <int, bool>::iterator iterY=row.begin(); iterY!=row.end(); iterY++)
		{
			min_x=(int)minv(min_x,iterX->first);
			max_x=(int)maxv(max_x,iterX->first);
			min_y=(int)minv(min_y,iterY->first);
			max_y=(int)maxv(max_y,iterY->first);
if(verbose) cout<<iterX->first<<'\t'<<iterY->first<<endl;
			double bestIJ=9999;
			for(int I=-5; I<=5; I++)
				for(int J=-5; J<=5; J++)
					if(scene_hash_points.find(iterX->first+I)!=
					   scene_hash_points.end() &&
					   scene_hash_points[iterX->first+I].find(iterY->first+J)!=
					   scene_hash_points[iterX->first+I].end())
						bestIJ=minv(bestIJ,sqrt(I*I+J*J));
			hit_count +=  1/(1+bestIJ);
			miss_count+=1-1/(1+bestIJ);
		}
	}

if(verbose) cout<<"\nScene points:\n";
	for(map <int, map <int, bool> >::iterator iterX=scene_hash_points.begin(); iterX!=scene_hash_points.end(); iterX++)
	{
		map <int,bool> row=iterX->second;
		for(map <int, bool>::iterator iterY=row.begin(); iterY!=row.end(); iterY++)
		{
if(verbose) cout<<iterX->first<<'\t'<<iterY->first<<endl;
			if(false &&
			   min_x<=iterX->first && iterX->first<=max_x &&
			   min_y<=iterY->first && iterY->first<=max_y)
			{
				double bestIJ=1e20;
				for(int I=-5; I<=5; I++)
					for(int J=-5; J<=5; J++)
						if(target_hash_points.find(iterX->first+I)!=
						   target_hash_points.end() &&
						   target_hash_points[iterX->first+I].find(iterY->first+J)!=
						   target_hash_points[iterX->first+I].end())
							bestIJ=minv(bestIJ,sqrt(I*I+J*J));
				miss_count+=1-1/(1+bestIJ);
			}
		}
	}
	return 100*(1.0*hit_count/(hit_count+miss_count));
}

void Graph::generate_vertex_list() { generate_vertex_list(0); }
void Graph::generate_vertex_list(int inters)
{
	inters++; // if inters=1, we want to have 1 midpoint between A and B, so this actually needs to be =2.

	vertex_list.clear();
	for(Edge* iter=e_front; iter!=NULL; iter=iter->next)
	{
		vertex_list[iter->from]=true;
		vertex_list[iter->to]=true;

		for(int h=1; h<inters; h++)
		{
			Vertex* inter;
			inter=new Vertex;
			inter->x=iter->from->x*(1.0*h/inters) + iter->to->x*(1 - 1.0*h/inters);
			inter->y=iter->from->y*(1.0*h/inters) + iter->to->y*(1 - 1.0*h/inters);
			vertex_list[inter]=true;
		}
	}
}

void Graph::generate_hash_points(double resolution_x, double resolution_y, double offset_x, double offset_y)
{
	hash_points.clear();
	for(map<Vertex*,bool>::iterator iterA=vertex_list.begin(); iterA!=vertex_list.end(); iterA++)
	{
		int X=(int)((iterA->first->x+offset_x)/resolution_x);
		int Y=(int)((iterA->first->y+offset_y)/resolution_y);
		hash_points[X][Y]=true;
	}
}

#endif

//////////////////////////////////////////////////////////////
//APPENDIX
//////////////////////////////////////////////////////////////
/*


const double BIN_RESOLUTION=0.01;

double cscore(map<Vertex*,bool> &scene_vertex_list, map<Vertex*,bool> &target_vertex_list, double VX, double VY, double scale,
               double target_xmax, double target_ymax,
               double scene_xmax, double scene_ymax)
{
	double score1=0,   score2=0;
	double counter1=0, counter2=0;
	double target_window_size=
	sqrt(powv(minv(target_xmax,scene_xmax)-maxv(0,VX),2)+
	     powv(minv(target_ymax,scene_ymax)-maxv(0,VY),2));

	for(map<Vertex*,bool>::iterator iter1=target_vertex_list.begin(); iter1!=target_vertex_list.end(); iter1++)
	{
		double min=1e10;
		for(map<Vertex*,bool>::iterator iter2=scene_vertex_list.begin(); iter2!=scene_vertex_list.end(); iter2++)
		{
			if(maxv(VX,0) <= iter2->first->x &&
			   iter2->first->x <= minv(target_xmax,scene_xmax) &&
			   maxv(VY,0) <= iter2->first->y &&
			   iter2->first->y <= minv(target_ymax,scene_ymax))
			{
				double dist=Vdist(iter1->first,iter2->first,scale,VX,VY);
				if(dist<min)
					min=dist;
			}
		}
		if(min<1e10)
		{
			score1-=powv(1-min/target_window_size,40);
			counter1++;
		}
	}

	//this loop is disabled
	for(map<Vertex*,bool>::iterator iter2=scene_vertex_list.begin(); false && iter2!=scene_vertex_list.end(); iter2++)
	{
		if(maxv(VX,0) <= iter2->first->x && iter2->first->x <= minv(target_xmax,scene_xmax) &&
		   maxv(VY,0) <= iter2->first->y && iter2->first->y <= minv(target_ymax,scene_ymax))
		{
			double min=1e10;
			for(map<Vertex*,bool>::iterator iter1=target_vertex_list.begin(); iter1!=target_vertex_list.end(); iter1++)
			{
				double dist=Vdist(iter1->first,iter2->first,scale,VX,VY);
				if(dist<min)
					min=dist;
			}
			score2+=min;
			counter2++;
		}
	}
	return score1;
} 

void best_cscore(Graph& scene, Graph& target, double scale, double& Xoffset, double& Yoffset, double& measure);


void Graph::compare(Graph& target)
{


	double Xoffset, Yoffset;
	double measureA, measureB, measureC;
	bool recalcA, recalcB, recalcC;

	double scale=3;
	double scale_step=0.5;

	recalcA=true;
	recalcB=true;
	recalcC=true;
	do
	{
		if(recalcA)
			best_cscore(*this,target, scale-scale_step, Xoffset, Yoffset, measureA);
		if(recalcB)
			best_cscore(*this,target, scale           , Xoffset, Yoffset, measureB);
		if(recalcC)
			best_cscore(*this,target, scale+scale_step, Xoffset, Yoffset, measureC);

		cout<<((recalcA)?"(A)":"( )")<<" ";
		cout<<((recalcB)?"(B)":"( )")<<" ";
		cout<<((recalcC)?"(C)":"( )")<<" ";
		cout<<scale<<" ("<<scale_step<<") "<<measureA<<" - "<<measureB<<" - "<<measureC<<endl;

		recalcA=false;
		recalcB=false;
		recalcC=false;


		if(measureA<measureB)
		{
			measureC=measureB;
			measureB=measureA;
			recalcA=true;
			scale-=scale_step;
		}
		else if(measureC<measureB)
		{
			measureA=measureB;
			measureB=measureC;
			recalcC=true;
			scale+=scale_step;
		}
		else
		{
			recalcA=true;
			recalcC=true;
			scale_step*=0.6;
		}
	}while(absv(scale_step)>=0.0001);
}


void best_cscore(Graph& scene, Graph& target, double scale, double& Xoffset, double& Yoffset, double& measure)
{
	map<Vertex*,bool> target_vertex_list= target.get_vertex_list();
	map<Vertex*,bool> scene_vertex_list = scene.get_vertex_list();

	double VX=0;
	double VY=0;
	double Xmax=target.getrows();
	double Ymax=target.getcols();
	double resultA, resultB, resultC;

	double Xstep=Xmax/2;
	double Ystep=Ymax/2;
	bool recalcA=true, recalcB=true, recalcC=true;
	while(Xstep>0.01)
	{
		if(recalcA)
			resultA=cscore(scene_vertex_list, target_vertex_list, VX-Xstep, VY, scale,
			target.getrows()*scale+VX-Xstep, target.getcols()*scale+VY,
			scene.getrows(),scene.getcols());
		if(recalcB)
			resultB=cscore(scene_vertex_list, target_vertex_list, VX      , VY, scale,
			target.getrows()*scale+VX      , target.getcols()*scale+VY,
			scene.getrows(),scene.getcols());
		if(recalcC)
			resultC=cscore(scene_vertex_list, target_vertex_list, VX+Xstep, VY, scale,
			target.getrows()*scale+VX+Xstep, target.getcols()*scale+VY,
			scene.getrows(),scene.getcols());

		recalcA=false;
		recalcB=false;
		recalcC=false;

		if(resultA < resultB)
		{
			VX -= Xstep;
			resultC=resultB;
			resultB=resultA;
			recalcA=true;
		}
		else if(resultB > resultC)
		{
			VX += Xstep;
			resultA=resultB;
			resultB=resultC;
			recalcC=true;
		}
		else
		{
			Xstep /= 1.5;
			recalcA=true;
			recalcC=true;
		}
	}
	recalcA=true;
	recalcB=true;
	recalcC=true;
	while(Ystep>0.01)
	{
		if(recalcA)
			resultA=cscore(scene_vertex_list, target_vertex_list, VX, VY-Ystep, scale,
			target.getrows()*scale+VX, target.getcols()*scale+VY-Ystep,
			scene.getrows(),scene.getcols());
		if(recalcB)
			resultB=cscore(scene_vertex_list, target_vertex_list, VX, VY      , scale,
			target.getrows()*scale+VX, target.getcols()*scale+VY      ,
			scene.getrows(),scene.getcols());
		if(recalcC)
			resultC=cscore(scene_vertex_list, target_vertex_list, VX, VY+Ystep, scale,
			target.getrows()*scale+VX, target.getcols()*scale+VY+Ystep,
			scene.getrows(),scene.getcols());

		recalcA=false;
		recalcB=false;
		recalcC=false;

		if(resultA < resultB)
		{
			VY -= Ystep;
			resultC=resultB;
			resultB=resultA;
			recalcA=true;
		}
		else if(resultB > resultC)
		{
			VY += Ystep;
			resultA=resultB;
			resultB=resultC;
			recalcC=true;
		}
		else
		{
			Ystep /= 1.5;
			recalcA=true;
			recalcC=true;
		}
	}

	measure=resultB;
}

map<dint,bool> fuzzy_intersection (map<dint,bool> A, map<dint,bool> B, double tolerance)
{
	map<dint,bool> ret;

	for(map<dint,bool>::iterator I=B.begin(); I!=B.end(); I++)
	{
		bool found=false;
		for(map<dint,bool>::iterator J=A.begin(); !found && J!=A.end(); J++)
			if(dint_dist(I->first,J->first)<=tolerance)
			{
				found=true;
				ret[J->first]=true;
			}
	}
	return ret;
}
map<dint,bool> fuzzy_union (map<dint,bool> A, map<dint,bool> B, double tolerance)
{
	map<dint,bool> D;

	map<dint,bool> C=fuzzy_intersection(B,A,tolerance); // C = B {fuzzy-int} A

	for(map<dint,bool>::iterator I=A.begin(); I!=A.end(); I++) // D = A;
		D[I->first]=I->second;

	for(map<dint,bool>::iterator I=B.begin(); I!=B.end(); I++) // D += B - C
		if(C.find(I->first)==C.end())
			D[I->first]=I->second;

	return D;
}


*/
//
