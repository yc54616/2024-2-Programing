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

Date date[128];
void SaveDate(int inode_num);
void PrintDate(int inode_num);

void SaveDate(int inode_num)
{
  struct tm *t;
  time_t clock;
  clock = time(NULL);
  t = localtime(&clock);

  date[inode_num].year = t->tm_year + 1900;
  date[inode_num].month = t->tm_mon;
  date[inode_num].day = t->tm_mday;
  date[inode_num].hour = t->tm_hour;
  date[inode_num].minute = t->tm_min;
  date[inode_num].second = t->tm_sec;
}

void PrintDate(int inode_num)
{
  printf("%4d/%02d/%02d %02d:%02d:%02d", date[inode_num].year, date[inode_num].month, date[inode_num].day, date[inode_num].hour, date[inode_num].minute, date[inode_num].second);
}
