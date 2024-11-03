#include <stdio.h>
#include <time.h>
   
typedef struct {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
}Date;

void SaveDate(Date *date);
void PrintDate(Date date);

void SaveDate(Date *date)
{
  struct tm *t;
  time_t clock;
  clock = time(NULL);
  t = localtime(&clock);

  date->year = t->tm_year + 1900;
  date->month = t->tm_mon;
  date->day = t->tm_mday;
  date->hour = t->tm_hour;
  date->minute = t->tm_min;
  date->second = t->tm_sec;
}

void PrintDate(Date date)
{
  printf("%4d/%02d/%02d %02d:%02d:%02d", date.year, date.month, date.day, date.hour, date.minute, date.second);
}
