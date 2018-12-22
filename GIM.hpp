//
//  Process.hpp
//  DCB
//
//  Created by Jiajun Chen on 31/05/2018.
//  Copyright © 2018 Jiajun Chen. All rights reserved.
//

#ifndef Process_hpp
#define Process_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include<iomanip>
#include <sstream>

#include "TimeSystem.h"
#include "Coordinate.h"
//#include "GlobalVariable.hpp"
using namespace std;

// Define GIM related strunct and class
struct Ionex_header
{
    int MAP_NUM;
    int Exponent;
    int nLat;
    int nLon;
    float Interval;
    vector<double> LAT;
    vector<double> LON;
};

struct GIM_tec_lat
{
    double Lat;
    vector<double> vtec;
};

struct GIM_tec_epoch
{
    TimeCalendar ctime;
    int TimeNO;
    vector<GIM_tec_lat> vtec;
};

struct Ionex
{
    Ionex_header header;
    vector<GIM_tec_epoch> vtec;
};

//Preprocess IPP struct
struct IPPStruct{
    CoorGeo ipp;
    TimeCalendar calTime;
    int PRN;
    double stec;
    double azimuth;
    double elevation;
    double zenith;
public:
    IPPStruct()
    {
        PRN = 0;
        stec = 0.0;
        azimuth = 0.0;
        elevation = 0.0;
        zenith = 0.0;
    }
};
struct IPPfile
{
    TimeCalendar epoch;
    vector<IPPStruct> ippPiece;
    vector<int> PRN;
    int     epochNUM;
};

//stec of four points to interpolate
struct interpolate{
    double E01;
    double E00;
    double E11;
    double E10;
public:
    interpolate()
    {
        E01 = 0.0;
        E00 = 0.0;
        E11 = 0.0;
        E10 = 0.0;
    }
    
};

struct TEC_point{
    CoorGeo pos;
    float tec;
public:
    TEC_point()
    {tec = 0.0;}
};

//read GIM file
bool ReadGIMMap(string filename, Ionex& GIMmap);

bool GIMInterpolation(Ionex& GIM, CoorGeo& point, TimeCalendar& ctime, double& intpVtec);

bool GIMInterpolationGrid(Ionex& GIM, TimeCalendar& ctime, vector<float>& spatialRange, vector<vector<TEC_point>>& intpVtec);

bool WriteToFile(vector<vector<TEC_point>>& intpVtec, string outfile);

bool WriteToIsceFile(vector<vector<TEC_point>>& vtec, string filename);

//split string
vector<string> &split(const string &str, char delim, vector<string> &elems, bool skip_empty = true);

#endif
