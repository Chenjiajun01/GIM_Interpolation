#include "TimeSystem.h"
#include <cmath>

///////////////////历法时类函数
//初始化构造函数
TimeCalendar::TimeCalendar()
{
	year = 0;
	month = 0;
	day = 0;
	hour = 0;
	minute = 0;
	second = 0.0;
}

//赋值构造函数
TimeCalendar::TimeCalendar(int nyear /*年*/, int nmonth  /*月*/, int nday  /*日*/,
	int nhour /*时*/, int nminute /*分*/, double nsecond  /*秒*/)
{

	year = nyear;
	month = nmonth;
	day = nday;
	hour = nhour;
	minute = nminute;
	second = nsecond;
}

//年、年积转化成历法时
TimeCalendar::TimeCalendar( int nyear/*年份*/, int doy/*年积日*/)
{
	year = nyear;
	hour = 0;
	minute = 0;
	second = 0.0;
	for (int j=1; ; j++)
	{
		day = doy - dayofmonth[j-1];
		if (j == 2)
		{
			if ((year%4==0 && year%100!=0) || year%400==0)
			{
				day--;
			}
		}
		if (day <= 0)
		{
			month = j;
			day = doy;
			break;
		}
		else
		{
			doy = day;
		}
	}
}

//GPS周、周秒转化为历法时
TimeCalendar::TimeCalendar(int week /*GPS周*/, double weeksecond /*周秒*/)
{
	TimeGPS tg(week, weeksecond);
	TimeCalendar tc;
	tc = ti_gpsToCal(tg);
	year = tc.year;
	month = tc.month;
	day = tc.day;
	hour = tc.hour;
	minute = tc.minute;
	second = tc.second;

}

////////////////////GPS时类函数
//初始化构造函数
TimeGPS::TimeGPS()
{
	week = 0;
	weeksecond = 0.0;
	day = 0;
	allsecond = 0.0;
}

//赋值构造对象
//构造函数，赋值构造GPS时对象
TimeGPS::TimeGPS(int nweek, double nweelsecond)
{
	week = nweek;
	weeksecond = nweelsecond;
}


TimeGPS::TimeGPS(double nallsecond)
{
	week = int(nallsecond / 604800);
	weeksecond = nallsecond - week * 604800;
	allsecond = nallsecond;
}


//历法时--GPS
TimeGPS ti_CalToGps(TimeCalendar timeCalendar)
{
	TimeGPS Gpstime;
	//TimeDoy Doytime;

	//int doy = 0;
	int day1 = 0;
	int i = 0;

	//计算GPS周
	TimeDoy Doytime (timeCalendar);
	//Doytime = TimeDoy(timeCalendar);

	//doy = Doytime.doy;
	for(day1 = Doytime.doy, i = 1980; i < timeCalendar.year; i++)
	{
		day1 += 365;
		if ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0)
		{
			day1++;
		}
	}
	day1 -= 6;
	Gpstime.week = (int) (day1 / 7);

	//计算GPS周秒
	Gpstime.allsecond = ((day1 * 24 + timeCalendar.hour) * 60
		+ timeCalendar.minute) * 60 + timeCalendar.second;
	Gpstime.weeksecond = Gpstime.allsecond - (Gpstime.week * 604800);
	Gpstime.day = (int)(Gpstime.weeksecond / 86400);
	return(Gpstime);
}
//历法时转换到GPS时(周，周秒)
TimeGPS::TimeGPS(TimeCalendar timeCalendar)
{
	TimeGPS gt = ti_CalToGps(timeCalendar);
	week = gt.week;
	day = gt.day;
	weeksecond = gt.weeksecond;
	allsecond = gt.allsecond;
}

//年、年积日转换为GPS时
TimeGPS::TimeGPS(int year /*年*/, int doy /*年积日*/)
{
	int j = 0;
//	double Allsecond = 0.0;
	for (j=1980; j<year; j++)
	{
		doy += 365;
		if ((j%4==0 && j%100!=0) || j%400==0)
		{
			doy++;
		}
	}
	doy -= 6 ;
	allsecond	= doy * 86400;            //86400一天24小时对应的秒数
	week		= (int)(allsecond/604800);  //604800为一周对应的秒数
	weeksecond	= allsecond - week * 604800;
	day			= (int) (weeksecond / 86400);
}

////////////////////DOY类函数
//初始化构造函数
TimeDoy::TimeDoy()
{
	year = 0;
	doy = 0;
}

//赋值构造对象
TimeDoy::TimeDoy(int nyear,int ndoy)
{
	year = nyear;
	doy = ndoy;
}
//历法时--DOY
TimeDoy ti_CalToDoy(TimeCalendar timeCalendar)
{
	TimeDoy Doytime;
	int doy = 0;
	for(int i = 1; i<timeCalendar.month; i++)
	{
		doy += 30;
		switch (i)
		{
		case 1:  doy++;break;
		case 3:  doy++;break;
		case 5:  doy++;break;
		case 7:  doy++;break;
		case 8:  doy++;break;
		case 10: doy++;break;
//		case 12: doy++;break;
		case 2:
			{
				doy -= 2;
				if ((timeCalendar.year % 4 == 0 && timeCalendar.year % 100 != 0) || timeCalendar.year % 400 == 0)
				{
					doy++;
				}
				break;
			}
		}
	}
	Doytime.doy = doy + timeCalendar.day;
	Doytime.year = timeCalendar.year;
	return(Doytime);
}
//历法时转换到年积日
TimeDoy::TimeDoy(TimeCalendar timeCalendar)
{
	TimeDoy dy = ti_CalToDoy(timeCalendar);
	doy = dy.doy;
	year = dy.year;
}

TimeCalendar ti_AllToCal(double all)
{	
	TimeGPS gt(all);
	TimeCalendar ct(gt.week, gt.weeksecond);
	return(ct);	
}

//GPS时(周秒,年积日)转历法时(年月日时分秒)
TimeCalendar ti_gpsToCal(TimeGPS gpsTime /*GPS时对象*/)
{       
	TimeCalendar calendarTime;
	int doy = 0;
	int day = gpsTime.week * 7 +  6 + (int)(gpsTime.weeksecond/86400); //自80年1月6日起，+6后表示从1月1日起，方便计算

	for (int i = 1980; ; i++)
	{	
		doy = day - 365;
		if ((i%4==0 && i%100!=0) || i%400==0)
		{
			doy--;//闰年为366天
		}
		if (doy<0)
		{
			calendarTime.year = i;  //计算出年号
			doy = day;
			break;
		}
		day = doy;
	}

	for (int j=1; ; j++)
	{
		//如果计算得到的day==0  直接得到年月日!
		if (day == 0)
		{
			calendarTime.year = calendarTime.year-1;
			calendarTime.month = 12;
			calendarTime.day = 31;
			break;
		}
		//逐渐递减每月的天数
		doy = day - dayofmonth[j-1];
		if (j == 2)
		{
			if ((calendarTime.year%4==0 && calendarTime.year%100!=0) || calendarTime.year%400==0)  
			{
				doy--;//闰年二月为29天
			}
		}

		if (doy <=0)
		{
			calendarTime.month = j;//计算出月
			calendarTime.day = day;//计算出日
			break;
		}
		else
		{
			day = doy ; 
		}
	}

	calendarTime.hour	=	(int)(gpsTime.weeksecond/3600)%24; //计算出时

	calendarTime.minute	=	(int)(gpsTime.weeksecond/60)%60;   //计算出分

	calendarTime.second	=	gpsTime.weeksecond - (int)(gpsTime.weeksecond/60) * 60; //计算出秒

	return calendarTime;


}

//历法时转换到全秒
double ti_getAllSecond(TimeCalendar timeCalendar)
{
    TimeGPS Gpstime (timeCalendar);
    //Gpstime = TimeGPS(timeCalendar);
    double allsecond = Gpstime.week * 604800 + Gpstime.weeksecond;
    return (allsecond);
}

int CalToNO(TimeCalendar timeCalendar)
{
	int epochNO;
	double temp_seconds = 0.0;
	temp_seconds = timeCalendar.hour * 3600 + timeCalendar.minute * 60 + floor(timeCalendar.second + 0.5);
	epochNO = floor(temp_seconds / 30.0 + 0.5) + 1;
	return epochNO;
}
