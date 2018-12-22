#ifndef TIME_H
#define TIME_H

const int dayofmonth[12]={31,28,31,30,31,30,31,31,30,31,30,31};//平年12个月对应的天数

//历法时类
class TimeCalendar
{
public:
	int year;		//年
	int month;  	//月
	int day;		//日 
	int hour;		//时
	int minute;	    //分
	double second;	//秒

	//构造函数为历法时对象初始化
	TimeCalendar();

	//构造函数，赋值构造对象
	TimeCalendar(int nyear /*年*/, int nmonth  /*月*/, int nday  /*日*/,
		int nhour /*时*/, int nminute /*分*/, double nsecond  /*秒*/);

	//构造函数，年、年积日转化为历法时
	TimeCalendar(int nyear/*年份*/, int doy/*年积日*/);

	//构造函数,GPS周、周秒转化为历法时
	TimeCalendar(int week /*GPS周*/, double weeksecond /*周秒*/);
};
//DOY类
class TimeDoy
{
public:
	int year;
	int doy;

	//构造函数，初始化年积日对象
	TimeDoy();
	
	//构造函数，赋值构造年积日对象
	TimeDoy(int nyear,int ndoy);
	
	//构造函数，历法时转成DOY
	TimeDoy(TimeCalendar timeCalendar);
};

//GPS类
class TimeGPS
{
public:
	int week;           //从1980年1月6日0时开始起算的周数
	double weeksecond;  //周内时间，从每周日0时起算的秒数
	int day;			//周内的时间，周日为0，一直到周六为6，该数据用于igs文件名的构建	
	double allsecond;   //全秒,从1980年1月1日0时开始起算

	//构造函数,初始化GPS时对象
	TimeGPS();	

	//构造函数，赋值构造GPS时对象
	TimeGPS(int nweek, double nweelsecond);

	//构造函数，赋值构造GPS时对象
	TimeGPS(double nallsecond/*全秒*/);   

	//历法时转化为GPS时
	TimeGPS(TimeCalendar Calendar /*历法时*/); 

	//年、年积日转换为GPS时
	TimeGPS(int year /*年*/, int doy /*年积日*/); 
};

//公用转换函数
//历法时--DOY
TimeDoy ti_CalToDoy(TimeCalendar timeCalendar);
//历法时--GPS
TimeGPS ti_CalToGps(TimeCalendar timeCalendar);
//全秒转历法时
TimeCalendar ti_AllToCal(double allsecond);
//GPS时(周秒,年积日)转历法时(年月日时分秒)
TimeCalendar ti_gpsToCal(TimeGPS gpsTime /*GPS时对象*/);

//历法时类(年月日时分秒)对象转换总秒数
double ti_getAllSecond(TimeCalendar timeCalendar /*历法时对象*/);

//epoch NO within one day from calendar time
int CalToNO(TimeCalendar timeCalendar);

#endif
