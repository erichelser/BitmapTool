#ifndef __BAG__
#define __BAG__

#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <math.h>
using namespace std;

class Bag
{
	public:
		Bag() {init();} ;
		Bag(bool);
		~Bag() {nuke();};
		void nuke();
		void insert(double x);
		inline int size() {return items;};
		double remove_smallest();
		double remove_largest();
		double index(int);	//treat like a sorted array
		
		void print() {print("");};
		void print(string s)
		{
			if(bag_right!=NULL) bag_right->print(s+"    ");
			cout<<s<<"[";
			if(has_left) cout<<value_left; else cout<<"-";
			cout<<",";
			if(has_right) cout<<value_right; else cout<<"-";
			cout<<"]"<<"("<<size()<<")"<<"\n";
			if(bag_left!=NULL) bag_left->print(s+"    ");
		};
	private:
		bool has_left, has_right;
		double value_left, value_right;
		Bag* bag_left;
		Bag* bag_right;
		int items;
		void balance();
		bool unique_values_only;
		void init();
};
#endif
