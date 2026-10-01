// This header provides abstraction interface to plotting BER.
//
// Author: Raymond Su, raymondsu0110@gmail.com

#ifndef PLOT_BER_H
#define PLOT_BER_H

#include <iostream>
#include <string>
#include <sstream>

#include "gnuplot.h"

using namespace std;


/* -------- declarations -------- */
struct CurveSettings
{
    // general settings for each curve
    string name = "default_name";   // name of the curve (shown as legends)
    double LineWidth = 1.5;         // line width of the curve
    string LineColor = "blue";      // color of the curve
    int MarkerType = 7;             // shape of the markers
    double MarkerSize = 1.0;        // size of the markers
};

struct Curve
{
	// data linked list
	double * data = nullptr;// data of the current curve (e.g. BER curve)
    Curve * next = nullptr; // link to next curve (e.g. another scheme)
	Curve * prev = nullptr; // link to previous curve
	
    int length = 0;         // length of the curve
    int id = -1;            // ID of the curve
    CurveSettings cs;       // settings for the curve
};

string cs_to_string(CurveSettings cs);

/* ----------- class ------------ */
class BERPlot
{
private:
	Curve * curves_head = nullptr;
    Curve * curves_tail = nullptr;
    int curves_count = 0;
    double * snr_array = nullptr;
    GnuplotPipe gp;

public:
	BERPlot();
	~BERPlot();
	void addData(double * data_array, double * snr, int N, \
                 int curve_id, CurveSettings cs);
	void updateData(double * data_array, int N, int curve_id);
    void plot(string title);
};

/* ------ member functions ------ */
BERPlot::BERPlot() : gp(true)
{
	// constructor
	
	curves_head = new Curve;
	curves_tail = new Curve;
    curves_tail = curves_head; // curves_head contains no data
}

BERPlot::~BERPlot()
{
	// destructor

    Curve * current = curves_tail;
    while(current != nullptr)
    {
        delete [] current -> data;
        Curve * temp = current -> prev;
        delete current;
        current = temp;
    }

    curves_tail = nullptr;
    curves_head = nullptr;
}

void BERPlot::addData(double * data_array, double * snr, int N, \
                      int curve_id, CurveSettings cs)
{
    // data_array -> the array of BER values
    // N          -> length of the data array
    // curve_id   -> id of this BER data array

    // create new curve & stuff
    Curve * new_element = new Curve;
    new_element -> id = curve_id;
    new_element -> length = N;
    new_element -> cs = cs;
    new_element -> data = new double [N];
    if(snr_array == nullptr)
    {
        snr_array = new double [N];
        for(int i = 0; i < N; i++) snr_array[i] = snr[i];
    }
    for(int i = 0; i < N; i++) new_element -> data[i] = data_array[i];

    ++curves_count;

    // add to linked list
    curves_tail -> next = new_element;
    new_element -> prev = curves_tail;
    curves_tail = new_element;
}

void BERPlot::updateData(double * data_array, int N, int curve_id)
{
    // update curve data
    Curve * current = curves_head -> next;
    while(current -> id != curve_id && current != nullptr)
    {
        current = current -> next;
    }
    if(current == nullptr)
    {
        cerr << "BER curve with this ID does Not exist!" << endl;
        return;
    }

    for(int i = 0; i < N; i++) current -> data[i] = data_array[i];
}

void BERPlot::plot(string title)
{
    // plot updated BER curves in each iteration

	// synthesize the title of the plot
	string plot_title_set = "set title '"  + title + "' font 'Arial,24'";

    // some general settings
    gp.sendLine("set logscale y");
    gp.sendLine(plot_title_set);
    gp.sendLine("set xlabel 'SNR (dB)' font 'Arial,14'");
    gp.sendLine("set ylabel 'Bit Error Rate' font 'Arial,14'");
    gp.sendLine("set format y '10^{%L}'");
    gp.sendLine("set tics font 'Arial,10'");
    gp.sendLine("set key box width 5 height 2 font 'Arial,12'"); // legends
    gp.sendLine("set grid");

    // plot
    string chimera = "plot ";
    string settings_str;
    Curve * current = curves_head -> next;
    while(current != nullptr)
    {
        settings_str = cs_to_string(current -> cs);
        chimera += "'-' with linespoints " + settings_str;
        if(current -> next) chimera += ", ";
        current = current -> next;
    }
    gp.sendLine(chimera); // the plot command

    ostringstream ss;
    current = curves_head -> next;
    while(current != nullptr)
    {
        for(int j = 0; j < (current -> length); j++)
        {
            ss << std::scientific << current -> data[j];
            string d = to_string(snr_array[j]) + " " + ss.str();
            gp.sendLine(d, true);

            ss.str("");
            ss.clear();
        }
        gp.sendEndOfData();
        current = current -> next;
    }
}

/* ------- other functions ------ */
string cs_to_string(CurveSettings cs)
{
    string s;
    s += "title '" + cs.name + "' ";
    s += "lw " + to_string(cs.LineWidth) + " ";
    s += "lc rgb '" + cs.LineColor + "' ";
    s += "pt " + to_string(cs.MarkerType) + " ";
    s += "ps " + to_string(cs.MarkerSize);

    return s;
}


#endif
