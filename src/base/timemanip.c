#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#include "def.h"
#include "logging.h"

#define 	YEAR0   1900
#define 	EPOCH_YR   1970
#define 	SECS_DAY   (24L * 60L * 60L)
#define 	LEAPYEAR(year)   (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define 	YEARSIZE(year)   (LEAPYEAR(year) ? 366 : 365)
#define 	FIRSTSUNDAY(timp)   (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define 	FIRSTDAYOF(timp)   (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)
#define 	TIME_MAX   ULONG_MAX
#define 	ABB_LEN   3
const int 	_ytab [2][12] = { { 31,28,31,30,31,30,31,31,30,31,30,31 } , {31,29,31,30,31,30,31,31,30,31,30,31} };

struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf)
{
  time_t time_i = *timer;
  unsigned long dayclock, dayno;
  int year = EPOCH_YR;

  dayclock = (unsigned long) time_i % SECS_DAY;
  dayno = (unsigned long) time_i / SECS_DAY;

  tmbuf->tm_sec = dayclock % 60;
  tmbuf->tm_min = (dayclock % 3600) / 60;
  tmbuf->tm_hour = dayclock / 3600;
  tmbuf->tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
  while (dayno >= (unsigned long) YEARSIZE(year))
  {
    dayno -= YEARSIZE(year);
    year++;
  }
  tmbuf->tm_year = year - YEAR0;
  tmbuf->tm_yday = dayno;
  tmbuf->tm_mon = 0;
  while (dayno >= (unsigned long) _ytab[LEAPYEAR(year)][tmbuf->tm_mon])
  {
    dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
    tmbuf->tm_mon++;
  }
  tmbuf->tm_mday = dayno + 1;
  tmbuf->tm_isdst = 0; // WinterTime = 0

  return tmbuf;
}
unsigned int get_time_unsigned()
{
	time_t t;
	time(&t);
	return (unsigned int) t;
}
unsigned int get_time_signed()
{
	time_t t;
	time(&t);
	return t;
}
int get_hours_minutes_seconds_from_client(char *si,int *hours,int *minutes,int *seconds)
{
	char *s;
	char *sb;
	char *se;
	char kar;
	s = si;
	while (isspace((int)*s) && (*s)!='\0') s++;
	if (*s == '\0')
	{
		(*hours) = (*minutes) = (*seconds) = 0;
		return 0;
	}
	if (!isdigit((int)*s)) return -1;
	sb = s;
	while (isdigit((int)*s)) s++;
	se = s;
	if ((*s)!=':')
	{
		while (isspace((int)*s) && (*s)!='\0') s++;
		if ((*s)=='\0')
		{
			if (se-sb > 2) return -1;
			kar = (*se);
			(*se)='\0';
			(*hours) = atoi(sb);
			(*se) = kar;
			if ((*hours) >= 24) return -1;
			(*minutes) = (*seconds) = 0;
			return (*hours) * 3600;
		}
		return -1;
	}
	kar = *se;
	(*se)='\0';
	if (se-sb > 2)
	{
		(*se) = kar;
		return -1;
	}
	(*hours) = atoi(sb);
	(*se) = kar;
	sb = s = se + 1;
	if (!isdigit((int)*s)) return -1;
	while (isdigit((int)*s)) s++;
	se = s;
	if ((*s)!=':')
	{
		while (isspace((int)*s) && (*s)!='\0') s++;
		if ((*s)=='\0')
		{
			kar = (*se);
			(*se)='\0';
			if (se-sb > 2)
			{
				(*se) = kar;
				return -1;
			}
			(*minutes) = atoi(sb);
			(*se) = kar;
			if ((*minutes) >= 60) return -1;
			(*seconds) = 0;
			return (*hours) * 3600 + (*minutes) * 60;
		}
		return -1;
	}
	kar = *se;
	(*se)='\0';
	if (se-sb > 2)
	{
		(*se) = kar;
		return -1;
	}
	(*minutes) = atoi(sb);
	(*se) = kar;
	sb = s = se+1;
	if (!isdigit((int)*s)) return -1;
	while (isdigit((int)*s)) s++;
	kar = *s;
	(*s) = '\0';
	if (s-sb > 2)
	{
		(*s) = kar;
		return -1;
	}
	(*seconds) = atoi(sb);
	(*s) = kar;
	if ((*seconds) >= 60) return -1;
	return (*hours) * 3600 + (*minutes) * 60 + (*seconds);
}

unsigned int get_date_from_string_from_client(char *si)
{
	char *s,*sb,*se;
	int day = 1;
	int month;
	int year = -1;
	int hours=0;
	int minutes=0;
	int seconds=0;
	struct tm time_p;
	time_t time_i;
	char kar;
	// DD
	if (si == NULL) return 0;
	if ((*si) == '\0') return 0;
	s = si;
	while (isspace((int)*s) && (*s)!='\0') s++;
	if ((*s)=='\0') return 0;
	sb = s;
	while (isdigit((int)*s)) s++;					// Day or Year
	if (sb == s) return 0;
	se = s;
	while (isspace((int)*s) && (*s)!='\0') s++;     // Remove trailing spaces
	if ((*s)=='\0')
	{
		if (se - sb == 4)							// It is only a year
		{
			year = atoi(sb);
			day = 1;
			month = 1;
			minutes = seconds = 0;
			hours = 0;
			time_p.tm_hour = hours;
			time_p.tm_mday = day;
			time_p.tm_min = minutes;
			time_p.tm_mon = month - 1;
			time_p.tm_sec = seconds;
			time_p.tm_year = year - 1900;
			time_p.tm_isdst = -1; // WinterTime = 0 - mktime delivers UTC time
			time_i = mktime(&time_p);
			if (time_i < 0) return 0;
			return (unsigned int)time_i;
		}
		return 0;
	}
	if ((*s)!='-') return 0;
	if ((se - sb) > 2)
	{
		if (se - sb == 4)							// It is a year
		{
			year = atoi(sb);
		}
		else return 0;
	}
	kar = (*se);
	(*se)='\0';
	if (year == -1)							// This is a day and not a year
	{
		day = atoi(sb);
		if (day > 31 || day<1)
		{
			(*se) = kar;
			return 0;
		}
	}
	(*se) = kar;
	s++;
	if ((*s)=='\0') return 0;
// Day done now month
	while (isspace((int)*s) && (*s)!='\0') s++;
	if ((*s)=='\0') return 0;
	sb = s;
	while (isdigit((int)*s)) s++;				// The months
	if (sb == s) return 0;
	se = s;
	while (isspace((int)*s) && (*s)!='\0') s++;
	if ((*s)=='\0')
	{
		if (year == -1) return 0;
		month = atoi(sb);
		day = 1;
		minutes = seconds = 0;
		hours = 0;
		time_p.tm_hour = hours;
		time_p.tm_mday = day;
		time_p.tm_min = minutes;
		time_p.tm_mon = month - 1;
		time_p.tm_sec = seconds;
		time_p.tm_year = year - 1900;
		time_p.tm_isdst = -1; // WinterTime = 0 - mktime delivers UTC time
		time_i = mktime(&time_p);
		if (time_i < 0) return 0;
		return (unsigned int)time_i;
	}
	if ((*s)!='-') return 0; // Now this is the next seperator before year    DD - MM
	if ((se - sb) > 2) return 0;					// Months maximum of 2
	kar = *se;
	(*se)='\0';
	month = atoi(sb);			// Months
	(*se) = kar;
	if (month > 12 || month<1) return 0;
	s++;
	if ((*s)=='\0') return 0;
	// The Year
	while (isspace((int)*s) && (*s)!='\0') s++;
	if ((*s)=='\0') return 0;
	sb = s;							// sb on Day or Year
	while (isdigit((int)*s)) s++;
	if (sb==s) return 0;
	se = s;
	while (isspace((int)*s) && (*s)!='\0') s++;
	if ((*s)!='\0')
	{
		if (!isdigit((int)*s)) return 0;
		if (get_hours_minutes_seconds_from_client(s,&hours,&minutes,&seconds)< 0) return 0;
	}
	kar = *se;
	(*se)='\0';
	switch(se-sb)
	{
	case 1:
		if (year == -1)
		{
			(*se) = kar;
			return -1;
		}
		day = atoi(sb);
		if (day < 1)
		{
			(*se) = kar;
			return 0;
		}
		break;
	case 2: // Add the year 2000
		if (year == -1) year = atoi(sb) + 2000;
		else {
			day = atoi(sb);
			if (day <1 || day >31)
			{
				(*se) = kar;
				return 0;
			}
		}
		break;
	case 3:
	case 4:
		if (year != -1) return 0;
		year = atoi(sb);
		if (year < 2000 || year >= 3000)
		{
			(*se) = kar;
			return 0;
		}
		break;
	default:
		(*se) = kar;
		return 0;
	}
	(*se) = kar;
	// Transform day , month and year
	time_p.tm_hour = hours;
	time_p.tm_mday = day;
	time_p.tm_min = minutes;
	time_p.tm_mon = month - 1;
	time_p.tm_sec = seconds;
	time_p.tm_year = year - 1900;
	time_p.tm_isdst = -1;
	time_i = mktime(&time_p);
	if (time_i < 0) return 0;
	return (unsigned int)time_i;
}
unsigned int get_first_week_of_year_on_monday(int year)
{
	int year_offset;
	int day,days;
	year_offset = year - 1970;
	days = year_offset * 365 + ((year_offset + 1) / 4); // 77 -> 7 + 1 -> 2
	day = (days + 4) % 7;
	switch(day)
	{
	case 0: // Sunday
		days++; // This is the first day
		break;
	case 1:
		break;
	case 2: // Tuesday
		days--;
		break;
	case 3: // Wednesday
		days -=2;
		break;
	case 4: // Thursday
		days -=3;
		break;
	case 5: // Friday
		days+=3;
		break;
	case 6: // Saturday
		days+=2;
		break;
	}
	return 3600 * 24 * days; // On TimeStamp 0:00
}
int get_week(time_t now)
{
	unsigned int firstTime;
	unsigned int firstTime2;
	int counter;
	struct tm time_p;
	if (now == 0) now = get_time_signed();
	gmtime_r(&now,&time_p);
	firstTime = (unsigned int)now;
	firstTime2 = get_first_week_of_year_on_monday(time_p.tm_year + 1900);
	for (counter = 0; counter < 52;counter++)
	{
		if (firstTime2 > firstTime) return counter-1;
		firstTime2 += 3600 * 24 * 7;
	}
	return 52;
}
void get_time_string(char *so,time_t now) {
	struct tm time_p;
	if (now == 0) now = get_time_signed();
	gmtime_r(&now,&time_p);
	sprintf(so,"%04d%02d%02d %02d%02d%02d",time_p.tm_year + 1900,time_p.tm_mon+1,time_p.tm_mday,time_p.tm_hour,time_p.tm_min,time_p.tm_sec);
	
}
void get_string_time_database(char *so,struct tm *time_p)
{
	sprintf(so,"%04d-%02d-%02d %02d:%02d:%02d",time_p->tm_year + 1900,time_p->tm_mon+1,time_p->tm_mday,time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
}

int end_timemanip()
{
#ifdef TESTMODE
	add_to_log("sds",__FILE__,__LINE__,"End");
#endif
	return __LINE__;
}
