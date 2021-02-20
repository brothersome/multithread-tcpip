#ifndef __HEADER__timemanip_
#define __HEADER_timemanip_

struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf);
unsigned int get_time_unsigned();
unsigned int get_time_signed();
int get_hours_minutes_seconds_from_client(char *si,int *hours,int *minutes,int *seconds);
unsigned int get_date_from_string_from_client(char *si);
unsigned int get_first_week_of_year_on_monday(int year);
int get_week(time_t now);
void get_string_time_database(char *so,struct tm *time_p);
void get_time_string(char *so,time_t now);

#endif