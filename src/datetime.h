#ifndef DATETIME_H_
#define DATETIME_H_

//(24L * 60L * 60L)
#define SECS_DAY    86400UL
#define LEAPYEAR(year)  (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)  (LEAPYEAR(year) ? 366 : 365)

typedef struct {
    unsigned char digitChar;
    unsigned char position;
    unsigned char blink;
} DIGIT;

extern DIGIT time_config[6];
extern DIGIT date_config[8];

unsigned char monthlen(unsigned char isleapyear,unsigned char month);
void formatDateTime(unsigned long time);
void changeTime(unsigned long *time, unsigned char add, unsigned char timePosition, unsigned char datePosition);
void calculateBlinking(unsigned char timePosition, unsigned char datePosition);

#endif

