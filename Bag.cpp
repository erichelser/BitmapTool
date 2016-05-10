#ifndef BAGCC
#define BAGCC

#include "Bag.h"

void Bag::init()
{
	has_left=false;
	has_right=false;
	value_left=0;
	value_right=0;
	bag_left=NULL;
	bag_right=NULL;
	items=0;
	unique_values_only=false;
}

Bag::Bag(bool unique)
{
	init();
	unique_values_only=unique;
}

void Bag::nuke()
{
	if(bag_left!=NULL)
	{
		bag_left->nuke();
		delete bag_left;
		bag_left=NULL;
	}
	if(bag_right!=NULL)
	{
		bag_right->nuke();
		delete bag_right;
		bag_right=NULL;
	}
}

void Bag::insert(double x)
{
	if(unique_values_only &&
	   ((has_left && value_left==x) ||
		(has_right && value_right==x)) )
		return;

	items++;
	if(bag_left==NULL)
		bag_left=new Bag();
	if(bag_right==NULL)
		bag_right=new Bag();

	if(!has_left)
	{
		has_left=true;
		value_left=x;
	}
	else if(!has_right)
	{
		has_right=true;
		if(x<value_left)
		{
			value_right=value_left;
			value_left=x;
		}
		else
			value_right=x;
	}
	else
	{
		if(x<value_left)
		{
			bag_left->insert(x);
			balance();
		}
		else if(value_right<x)
		{
			bag_right->insert(x);
			balance();
		}
		else
		{
			if(bag_left->size()<bag_right->size())
			{
				bag_left->insert(value_left);
				value_left=x;
			}
			else
			{
				bag_right->insert(value_right);
				value_right=x;
			}
		}
	}
}

void Bag::balance()
{
	int diff=bag_right->size() - bag_left->size();
	if(diff<-1)
	{
		bag_right->insert(value_right);
		value_right=value_left;
		value_left=bag_left->remove_largest();
		bag_left->balance();
	}
	else if(1<diff)
	{
		bag_left->insert(value_left);
		value_left=value_right;
		value_right=bag_right->remove_smallest();
		bag_right->balance();
	}
}

double Bag::remove_largest()
{
	items--;
	double ret;
	
	if(!has_right)
	{
		ret=value_left;
		has_left=false;
	}
	else if(bag_right->size()==0)
	{
		ret=value_right;
		has_right=false;
		if(bag_left->size()>0)
		{
			value_right=value_left;
			value_left=bag_left->remove_largest();
			has_right=true;
		}
	}
	else
		ret=bag_right->remove_largest();
	return ret;
}

double Bag::remove_smallest()
{
	items--;
	double ret;
	
	if(!has_right)
	{
		ret=value_left;
		has_left=false;
	}
	else if(bag_left->size()==0)
	{
		ret=value_left;
		value_left=value_right;
		has_right=false;
		if(bag_right->size()>0)
		{
			value_right=bag_right->remove_largest();
			has_right=true;
		}
	}
	else
		ret=bag_left->remove_smallest();
	return ret;
}

double Bag::index(int i)
{
	if(i<0 || size()<=i)
		return 0;
	if(i<bag_left->size())
		return bag_left->index(i);
	if(i-bag_left->size() == 0)
		return value_left;
	if(i-bag_left->size() == 1)
		return value_right;
	return bag_right->index(i-bag_left->size()-2);
}

#endif
