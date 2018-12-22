//
//  main.cpp
//  GIM_Interpolation
//
//  Created by Jiajun Chen on 12/06/2018.
//  Copyright © 2018 Jiajun Chen. All rights reserved.
//

#include <iostream>
#include "GIM.hpp"

int main(int argc, const char * argv[]) {
    
    string GIMfile = "";
    string outfile = "";
    string outformat = "isce"; //default
    string intptime = "00:00:ss";
    vector<float> spatialRange;
    
    //for single point
    CoorGeo pointGcrd;
    
    //for two modes: 0 area range; 1 single point
    int processFlag = 0;
    
    vector<vector<TEC_point>> TECinterpolate;
    Ionex GIM;
    TimeCalendar intpCtime;
    
    if(argc==1)  //parameter should be: GIMfile IPPfile
    {
        cout<<"Function: Interpolate GIM VTEC" << endl;
        cout<<"Usage:    GIM_Interpolation  <GIMfile> -t -b/-p [-o] [--of: isce/ascii]" << endl;
        cout<<"Example:  GIM_Interpolation  CODG3290.15I -t 03:44:00 -b 22.62 0.01 23.40 120.06 0.001 120.93 -o 20151126_gim.vtec --of isce" << endl;
        cout<<"Example:  GIM_Interpolation  CODG3290.15I -t 03:44:00 -p 22.62 120.06 -o 20151126_gim_22.62_120.6.vtec.txt" << endl;
        cout <<"           -t: hh:mm:ss" << endl;
        cout <<"           -b: minB dB maxB minL dL maxL" << endl;
        cout <<"           -p: single point coordinate Lat Lon" << endl;
        cout <<"           -o: outfile name" << endl;
        cout <<"            --of: outfile format: isce or ascii" << endl;
        cout<<"**************************" << endl;
        return 1;
     }
     else{
        GIMfile = argv[1];
      }
    
    
    for (int i=2;i<argc;i++)
    {
        string temp=argv[i];
        if (temp=="-t")
        {
            temp=argv[i+1];
            intptime = temp;
            i++;
        }
        else if (temp=="-o"){
            temp=argv[i+1];
            outfile=temp;
            i++;
        }
        else if (temp=="-p"){
            temp=argv[i+1];
            pointGcrd.B=atof(temp.c_str());
            temp=argv[i+2];
            pointGcrd.L=atof(temp.c_str());
            pointGcrd.H = 450000;
            processFlag = 1;
            i++;
        }
        else if (temp=="--of"){
            temp=argv[i+1];
            outformat=temp;
            i++;
        }
        else if (temp=="-b")
        {
            for (int j = 0; j < 6; ++j) {
                temp=argv[i+1];
                spatialRange.push_back(atof(temp.c_str()));
                i++;
            }
        }
    }
    //////////////////////////wrap paramter
    
    if (outfile.size()==0) {
        string GIMbasename = GIMfile.substr(GIMfile.size() - 12);
        outfile = GIMbasename + "_" + outformat + "." + argv[2];
    }
    
    // read GIM file
    if (false == ReadGIMMap(GIMfile, GIM)){
        return 1;
    }
    // get interpolation time
    intpCtime = GIM.vtec[0].ctime;
    intpCtime.hour = 0;
    intpCtime.minute = 0;
    intpCtime.second = 0.0;
    vector<string> intptime_v;
    split(intptime, ':', intptime_v);
    //cout << "time: " << intptime << endl;
    if (intptime_v.size() > 1) {
        intpCtime.hour = atoi(intptime_v[0].c_str());
    }
    if (intptime_v.size() > 2) {
        intpCtime.minute = atoi(intptime_v[1].c_str());
    }
    if (intptime_v.size() == 3) {
        intpCtime.second = atof(intptime_v[2].c_str());
    }
    if (intptime_v.size() > 3) {
        cout << "Unrecoganised time: intptime" << endl;
        return 1;
    }
    //processing mode option
    switch (processFlag) {
         case 1: //single point
            double vtec_ij;
            if (false ==GIMInterpolation(GIM, pointGcrd, intpCtime, vtec_ij)){
                cout << "Fail to interpolate at " << intptime
                <<" "<< pointGcrd.B <<" " << pointGcrd.L << endl;
                return 1;
            }
            //print to screen
            cout<<intptime<<"  "<< pointGcrd.B <<"  " << pointGcrd.L <<"  "<< vtec_ij << endl;
            break;
            
        default: //default mode, area range
            // interpolation
            if (false == GIMInterpolationGrid(GIM, intpCtime, spatialRange, TECinterpolate)){
                return 1;
            }
            //save to ROIPAC format file
            if (outformat == "isce") {
                if (false == WriteToIsceFile(TECinterpolate, outfile)) {
                    return 1;
                }
            }
            //save to ascii format
            else if (outformat == "ascii"){
                if (false == WriteToFile(TECinterpolate, outfile)){
                    return 1;
                }
            }
            else{
                cout << "Unrecogonised output format" << endl;
                return 1;
            }
            break;
    }
    
    //string GIMfile = "/Users/jchen/Documents/Projects/Ionosphere/calif_gps/222/CODG2220.06I";
    
    //string outfile = "/Users/jchen/Documents/Projects/Ionosphere/calif_gps/222/wwmt2220.06IPP";
    //Interpolate(GIMfile, hourtime, spatialRange, TECinterpolate);
    //WriteToFile(vector<float> TEC, outfile);
    return 0;
}
