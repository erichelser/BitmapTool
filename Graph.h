#ifndef __GRAPH__
#define __GRAPH__

#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <math.h>
#include <map>
#include "Bag.h"
#include "Image.h"

using namespace std;

struct Vertex
{
	double x, y;
	int weight;
	Vertex* next;
};
struct Edge
{
	Vertex* from;
	Vertex* to;
	Edge* next;
};

class Image;

class Graph
{
	public:
		Graph();
		Graph(Image&);
		Graph(char*);
		void condense();
		void dump();
		void dump(char*);
		void smooth();
		inline Edge* get_edge_front(){ return e_front; };
		static char* tochar(double,int);
		double compare(Graph&);
		void generate_vertex_list();
		void generate_vertex_list(int);
		void generate_hash_points(double,double,double,double);
		map <int, map <int, bool> > get_hash_points() { return hash_points; } ;
		map <Vertex*,bool> get_vertex_list() { return vertex_list; } ;
		int getrows() { return rows; } ;
		int getcols() { return cols; } ;
		int edge_count();
	private:
		int rows, cols;
		Edge* e_front;
		Edge* e_back;
		Vertex* v_front;
		Vertex* v_back;

		map <int, map <int, bool> > hash_points;
		map <Vertex*,bool> vertex_list;

		Vertex* addVertex(double i, double j, int w);
		void removeVertex(Vertex* v);
		Vertex* findVertex(double i, double j);
		bool addEdge(Vertex* from, double i, double j);
		bool addEdge(Vertex* from, Vertex* to);
		void removeEdge(Edge* e);
		Edge* findEdgePair(Vertex* v, Edge* beginhere);
		double getAngle(Edge* a, Edge* b);
		double absv(double x);
		double modv(double x, double y);
		void removeEdgePair(Edge* A, Edge* B);
};

#endif
