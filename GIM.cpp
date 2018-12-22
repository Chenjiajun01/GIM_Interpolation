//
//  Process.cpp
//  DCB
//
//  Created by Jiajun Chen on 31/05/2018.
//  Copyright © 2018 Jiajun Chen. All rights reserved.
//

#include "GIM.hpp"

bool ReadGIMMap(string ionexfile, Ionex& GIMmap)
{
    Ionex_header header;
    vector<GIM_tec_epoch> vtec;
    
    ifstream infile;
    infile.open(ionexfile.c_str());
    
    //infile.open(v_ObsFileName.at(obsnum).c_str());
    if (!infile.is_open())
    {
        cout<<"Fail to open "<<ionexfile<<endl;
        return false;
    }
    string Line;
    //header
    int nLat = 0; int nLon = 0;
    double Iono_shell_height = 0.0;
    double exponent = 0.1;
    GIMmap.header = header;
    
    //Satellite DCB
    int PRN = 0;
    double sdcb = 0.0;
    
    while (getline(infile, Line))
    {
        if (Line.find("INTERVAL") != string::npos)
        {
            header.Interval = atof(Line.substr(2,4).c_str());
            continue;
        }
        if (Line.find("# OF MAPS IN FILE") != string::npos)
        {
            header.MAP_NUM = atoi(Line.substr(4,3).c_str());
            continue;
        }
        if (Line.find("LAT1 / LAT2 / DLAT") != string::npos)
        {
            header.LAT.push_back(atof(Line.substr(3,5).c_str())); //LAT1
            header.LAT.push_back(atof(Line.substr(9,5).c_str())); //LAT2
            header.LAT.push_back(atof(Line.substr(15,5).c_str())); //dLAT
            nLat = floor((header.LAT[1]-header.LAT[0])/header.LAT[2] +0.5)+1;
            header.nLat = nLat;
            continue;
        }
        if (Line.find("LON1 / LON2 / DLON") != string::npos)
        {
            header.LON.push_back(atof(Line.substr(2,6).c_str())); //LON1
            header.LON.push_back(atof(Line.substr(8,6).c_str())); //LON2
            header.LON.push_back(atof(Line.substr(15,5).c_str())); //dLON
            nLon = floor((header.LON[1]-header.LON[0])/header.LON[2] +0.5)+1;
            header.nLon = nLon;
            continue;
        }
        if (Line.find("HGT1 / HGT2 / DHGT") != string::npos) {
            Iono_shell_height = atof(Line.substr(0,8).c_str());
        }
        if (Line.find("EXPONENT") != string::npos)
        {
            header.Exponent = atoi(Line.substr(4,2).c_str());
            exponent = pow(10, header.Exponent );
            continue;
        }
        if (Line.find("DIFFERENTIAL CODE BIASES")!= string::npos)
        {
            while(getline(infile,Line))
            {
                if (Line.find("PRN / BIAS / RMS")!= string::npos && Line.substr(3,1) == "G")
                { 
                    PRN = atoi(Line.substr(4,2).c_str());
                    sdcb = atof(Line.substr(9,7).c_str());
                    continue;
                }
                if (Line.find("END OF HEADER")!= string::npos)
                {
                    break;
                }
            }
        }
        if (Line.find("END OF HEADER") != string::npos)
        {
            break;
        }
    }
    //TEC MAPS
    GIM_tec_epoch temp_epoch;
    GIM_tec_lat temp_lat;
    
    while (getline(infile,Line))
    {
        if (Line.find("START OF TEC MAP") != string::npos)
        {
            continue;
        }
        if (Line.find("START OF RMS MAP") != string::npos)
        {
            break;
        }
        //each epoch
        if (Line.find("EPOCH OF CURRENT MAP") != string::npos)
        {
            temp_epoch.ctime.year = atoi(Line.substr(2,4).c_str());
            temp_epoch.ctime.month = atoi(Line.substr(10,2).c_str());
            temp_epoch.ctime.day = atoi(Line.substr(16,2).c_str());
            temp_epoch.ctime.hour = atoi(Line.substr(22,2).c_str());
            temp_epoch.ctime.minute = atoi(Line.substr(28,2).c_str());
            temp_epoch.ctime.second = atof(Line.substr(33,7).c_str());
            temp_epoch.TimeNO = CalToNO(temp_epoch.ctime);
            continue;
        }
        //each lat
        for (int i = 0; i < header.nLat; i++)
        {
            temp_lat.vtec.resize(nLon);
            if (Line.find("LAT/LON1/LON2/DLON/H") != string::npos)
            {
                temp_lat.Lat = atof(Line.substr(3,5).c_str());
                if (nLon <= 16)
                {
                    for (int j = 0; j < nLon; j++)
                    {
                        infile >> temp_lat.vtec[j];
                        temp_lat.vtec[j] *= exponent;
                    }
                    getline(infile, Line);
                }
                if (nLon > 26 && nLon <= 32)
                {
                    for (int j = 0; j < 16; j++)
                    {
                        infile >> temp_lat.vtec[j];
                        temp_lat.vtec[j] *= exponent;
                    }
                    getline(infile,Line);
                    for (int j = 16; j < nLon; j++)
                    {
                        infile >> temp_lat.vtec[j];
                        temp_lat.vtec[j] *= exponent;
                    }
                    getline(infile,Line);
                }
                if (nLon > 32 && nLon <= 48)
                {
                    for (int j = 0; j < 16; j++)
                    {
                        infile >> temp_lat.vtec[j];
                        temp_lat.vtec[j] *= exponent;
                    }
                    getline(infile,Line);
                    for (int j = 16; j < 32; j++)
                    {
                        infile >> temp_lat.vtec[j];
                        temp_lat.vtec[j] *= exponent;
                    }
                    getline(infile,Line);
                    for (int j = 32; j < nLon; j++)
                    {
                        infile >> temp_lat.vtec[j];
                        temp_lat.vtec[j] *= exponent;
                    }
                    getline(infile,Line);
                }
                
                //////
                if (nLon > 64 && nLon <= 80)
                {
                    for (int m = 0; m < 4; m++)
                    {
                        for (int j = 16*m; j < 16*(m+1); j++)
                        {
                            infile >> temp_lat.vtec[j];
                            temp_lat.vtec[j] *= exponent;
                        }
                        getline(infile,Line);
                    }
                    
                    for (int j = 64; j < nLon; j++)
                    {
                        infile >> temp_lat.vtec[j];
                        temp_lat.vtec[j] *= exponent;
                    }
                    getline(infile,Line);
                }
                if (temp_lat.vtec.size() == 0)
                {
                    cout << "Error in reading this epoch!" << endl;
                    continue;
                }
                temp_epoch.vtec.push_back(temp_lat);
                temp_lat.vtec.clear();
                vector<double>().swap(temp_lat.vtec);
            }
            getline(infile,Line);
            
            //for zhangrui GIM
            if (Line.size() < 5)
            {
                getline(infile,Line);
            }
    
        }
        vtec.push_back(temp_epoch);
        vector<GIM_tec_lat>().swap(temp_epoch.vtec);
    }
    
    infile.close();
    
    vtec[header.MAP_NUM-1].TimeNO = vtec[header.MAP_NUM-2].TimeNO + int(header.Interval/30.0); //only when the last arc is the first of next day
    GIMmap.header = header;
    GIMmap.vtec = vtec;
    
    //cout << "Ionex file read successfully!" << endl;
    return true;
}

bool GIMInterpolation(Ionex& GIM, CoorGeo& point, TimeCalendar& ctime, double& intpVtec){
    //interpolation time
    double second = 0.0;
    second = ti_getAllSecond(ctime);
   
    //Get the 8 GIM points
    TimeCalendar startCtime, endCtime;
    double startSecond = 0.0;
    double endSecond = 0.0;
    startCtime = GIM.vtec[0].ctime;
    endCtime = GIM.vtec.back().ctime;
    startSecond = ti_getAllSecond(startCtime);
    endSecond = ti_getAllSecond(endCtime);
    
    //check if out of range
    if (second < startSecond || second > endSecond) {
        cout << "Interpolation time out of range!" << endl;
        return false;
    }
    
    double T_interval = GIM.header.Interval;
    int ti = 0;
    
    ti = floor((second - startSecond) / T_interval + 0.00001);
    double dt0 = second - (ti * T_interval + startSecond); // t - Ti
    double dt1 = (ti + 1) * T_interval + startSecond - second; //Ti+1 - t
    double Lr = point.L + dt0 / 3600; //Lrotate = L + (t - Ti)
    double Lrp1 = point.L - dt1 / 3600;
    
    double Bmin = 0.0;
    double Bmax = 0.0;
    double dB = 0.0;
    double q = 0.0;
    int Bi = 0;
    Bmin = GIM.header.LAT[0];
    Bmax = GIM.header.LAT[1];
    dB = GIM.header.LAT[2];
    //check the range
    if (point.B < Bmax || point.B > Bmin) {
        cout << "Interpolation LAT out of range!" << endl;
        return false;
    }
    Bi = floor((point.B - Bmin) / dB + 0.00001);
    q = (point.B - ((Bi+1) * dB + Bmin)) / fabs(dB);
    
    double Lmin = 0.0;
    double Lmax = 0.0;
    double dL = 0.0;
    double p = 0.0;
    int Li = 0;
    Lmin = GIM.header.LON[0];
    Lmax = GIM.header.LON[1];
    dL = GIM.header.LON[2];
    if (point.L < Lmin || point.L > Lmax) {
        cout << "Interpolation LON out of range!" << endl;
        return false;
    }
    Li = floor((Lr - Lmin) / dL + 0.00001);
    p = (Lr - (Li * dL + Lmin)) / dL;
    //Li = floor((point.L - Lmin) / dL + 0.00001);
    //p = (point.L - (Li * dL + Lmin)) / dL;
    
    //spatial interpolation
    interpolate vtec_GIM_i;
    double intpVtec_i = 0.0;
    vtec_GIM_i.E00 = GIM.vtec[ti].vtec[Bi+1].vtec[Li];
    vtec_GIM_i.E01 = GIM.vtec[ti].vtec[Bi].vtec[Li];
    vtec_GIM_i.E10 = GIM.vtec[ti].vtec[Bi+1].vtec[Li+1];
    vtec_GIM_i.E11 = GIM.vtec[ti].vtec[Bi].vtec[Li+1];
    intpVtec_i = (1 - p) * (1 - q) * vtec_GIM_i.E00 + p * (1 - q) * vtec_GIM_i.E10 + q * (1-p) * vtec_GIM_i.E01 + p * q * vtec_GIM_i.E11;
    
    Li = floor((Lrp1 - Lmin) / dL + 0.00001);
    p = (Lrp1 - (Li * dL + Lmin)) / dL;
    interpolate vtec_GIM_ip1;
    double intpVtec_ip1 = 0.0;
    vtec_GIM_ip1.E00 = GIM.vtec[ti+1].vtec[Bi+1].vtec[Li];
    vtec_GIM_ip1.E01 = GIM.vtec[ti+1].vtec[Bi].vtec[Li];
    vtec_GIM_ip1.E10 = GIM.vtec[ti+1].vtec[Bi+1].vtec[Li+1];
    vtec_GIM_ip1.E11 = GIM.vtec[ti+1].vtec[Bi].vtec[Li+1];
    intpVtec_ip1 = (1 - p) * (1 - q) * vtec_GIM_ip1.E00 + p * (1 - q) * vtec_GIM_ip1.E10 + q * (1-p) * vtec_GIM_ip1.E01 + p * q * vtec_GIM_ip1.E11;
    
    //temporal interpolation
    //double dt0 = second - (ti * T_interval + startSecond); // t - Ti
    //double dt1 = (ti + 1) * T_interval + startSecond - second; //Ti+1 - t
    intpVtec = (dt1 / T_interval) * intpVtec_i + (dt0 / T_interval) * intpVtec_ip1;
    
    return true;
}

bool GIMInterpolationGrid(Ionex& GIM, TimeCalendar& ctime, vector<float>& spatialRange, vector<vector<TEC_point>>& intpVtec){
    //calculate grid point
    float minLat = spatialRange[0];
    float dLat = spatialRange[1];
    float maxLat = spatialRange[2];
    float minLon = spatialRange[3];
    float dLon = spatialRange[4];
    float maxLon = spatialRange[5];
    int numLat = int((maxLat - minLat) / dLat + 0.001) + 1;
    int numLon = int((maxLon - minLon) / dLon + 0.001) + 1;
    
    //double Lat_i = 0.0;
    CoorGeo point_ij;
    double vtec_ij = 0.0;
    TEC_point intptec_ij;
    for (int i = 0; i < numLat; ++i) {
        point_ij.B = minLat + i * dLat;
        vector<TEC_point> intptec_i;
        for (int j = 0; j < numLon; ++j) {
            point_ij.L = minLon + j * dLon;      
            if (false == GIMInterpolation(GIM, point_ij, ctime, vtec_ij)) {
                cout << "Fail to interpolate at: " << point_ij.B << " " << point_ij.L << endl;
                return false;
            }
            intptec_ij.pos = point_ij;
            intptec_ij.tec = vtec_ij;
            intptec_i.push_back(intptec_ij);
        }
        intpVtec.push_back(intptec_i);
    }
    return true;
}

bool WriteToFile(vector<vector<TEC_point>>& vtec, string filename){
    ofstream fout(filename.c_str(),ios::out);
    fout << "IPPLAT           IPPLON        VTEC" << endl;
    //fout.setf(ios_base::fixed,ios_base::floatfield);
    fout.setf(ios_base::adjustfield,ios_base::right);
    unsigned long lines = vtec.size();
    unsigned long colums = vtec[0].size();
    for (int i = 0; i < lines; ++i) {
        for (int j = 0; j < colums; ++j) {
            fout << setw(11)<<setprecision(4)<<vtec[i][j].pos.B
            << setw(12)<<setprecision(4)<<vtec[i][j].pos.L
            << setw(12)<<setprecision(3)<<vtec[i][j].tec
            << endl;
        }
    }
    fout.close();
    return true;
}

bool WriteToIsceFile(vector<vector<TEC_point>>& vtec, string filename){
    ofstream fout(filename.c_str(),ios::out);
    unsigned long length = vtec.size();
    unsigned long width = vtec[0].size();
    for (int i = length-1; i >= 0; --i) {
        for (int j = 0; j < width; ++j) {
            fout.write( (char*) &vtec[i][j].tec, sizeof(float) );
        }
    }
    fout.close();
    //write .rsc file
    double xfirst = vtec[0][0].pos.L;
    double yfirst = vtec[length-1][0].pos.B;
    double xstep = vtec[0][1].pos.L - vtec[0][0].pos.L;
    double ystep = vtec[0][0].pos.B - vtec[1][0].pos.B;
    string rscfilename = filename + ".rsc";
    ofstream frout(rscfilename.c_str(),ios::out);
    frout.setf(ios_base::adjustfield,ios_base::left);
    frout << setw(20) << "WIDTH" << setw(12)<<setprecision(7) << width << endl;
    frout << setw(20) << "FILE_LENGTH" << setw(12)<<setprecision(7) << length << endl;
    frout << setw(20) << "XMIN" << setw(12)<<setprecision(7) << 0 << endl;
    frout << setw(20) << "XMAX" << setw(12)<<setprecision(7) << width-1 << endl;
    frout << setw(20) << "YMIN"<< setw(12)<<setprecision(7) << 0 << endl;
    frout << setw(20) << "YMAX" << setw(12)<<setprecision(7) << length-1 << endl;
    frout << setw(20) << "X_FIRST" << setw(12)<<setprecision(7) << xfirst << endl;
    frout << setw(20) << "Y_FIRST" << setw(12)<<setprecision(7) << yfirst << endl;
    frout << setw(20) << "X_STEP" << setw(12)<<setprecision(7) << xstep << endl;
    frout << setw(20) << "Y_STEP" << setw(12)<<setprecision(7) << ystep << endl;
    frout << setw(20) << "X_UNIT" << setw(12) << "TECU" << endl;
    frout << setw(20) << "Y_UNIT" << setw(12) << "TECU" << endl;
    
    frout.close();
    
    return true;
}

vector<string> &split(const string &str, char delim, vector<string> &elems, bool skip_empty) {
    istringstream iss(str);
    for (string item; getline(iss, item, delim); )
        if (skip_empty && item.empty()) continue;
        else elems.push_back(item);
    return elems;
}


