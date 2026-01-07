// This header provides abstraction interface to plotting BER.
//
// Author: Raymond Su, raymondsu0110@gmail.com

#ifndef PLOT_BER_H
#define PLOT_BER_H

#include<iostream>
#include<vector>
#include"gnuplot.h"

using namespace std;

/* ----- declarations ----- */
struct Curve
{
	// data linked list

	int * data;
	Data * next;
	Data * prev;
};

/* ----- class ----- */
class BERPlot
{
private:
	Curve * curve;

public:
	BERPlot();
	~BERPlot();
	void addData();
	void plot();
};

/* ----- member functions ----- */
BERPlot::BERPlot()
{
	// constructor
	
	curve = nullptr;
}

BERPlot::~BERPlot()
{
	// destructor
}

void BERPlot::addData()
{
}

void BERPlot::plot()
{
}


#endif
