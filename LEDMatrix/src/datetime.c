#include "datetime.h"

DIGIT date_config[8] = {//digitChar, position, changeDigit;
    { 0,    20, 0},
    { 0,    25, 0},
    { 0,    30, 0},
    { 0,    35, 0},
    { 0,    45, 0},
    { 0,    50, 0},
    { 0,    60, 0},
    { 0,    65, 0}
};

DIGIT time_config[6] = {//digitChar, position, changeDigit;
    { 0,    20, 0},
    { 0,    25, 0},
    { 0,    35, 0},
    { 0,    40, 0},
    { 0,    50, 0},
    { 0,    55, 0}
};

unsigned char monthlen(unsigned char isleapyear, unsigned char month) {
    if (month == 1) {
        return (28 + isleapyear);
    }
    if (month > 6) {
        month--;
    }
    if (month % 2 == 1) {
        return (30);
    }
    return (31);
}

void formatDateTime(unsigned long time) {
    unsigned short year = 1970;
    unsigned char mon = 0;
    unsigned short day = 0;
    unsigned char hour = 0;
    unsigned char min = 0;
    unsigned char sec = 0;
    unsigned long daytime = time % SECS_DAY;

    day = time / SECS_DAY;
    hour = daytime / 3600UL;
    min = (daytime % 3600) / 60;
    sec = daytime % 60;

    while (day >= YEARSIZE(year)) {
        day -= YEARSIZE(year);
        year++;
    }
    mon = 0;
    while (day >= monthlen(LEAPYEAR(year), mon)) {
        day -= monthlen(LEAPYEAR(year), mon);
        mon++;
    }
    mon++;
    day++;

    date_config[0].digitChar = year / 1000 + 16;
    date_config[1].digitChar = year % 1000 / 100 + 16;
    date_config[2].digitChar = year % 100 / 10 + 16;
    date_config[3].digitChar = year % 10 + 16;
    date_config[4].digitChar = mon / 10 + 16;
    date_config[5].digitChar = mon % 10 + 16;
    date_config[6].digitChar = day / 10 + 16;
    date_config[7].digitChar = day % 10 + 16;
    time_config[0].digitChar = hour / 10 + 16;
    time_config[1].digitChar = hour % 10 + 16;
    time_config[2].digitChar = min / 10 + 16;
    time_config[3].digitChar = min % 10 + 16;
    time_config[4].digitChar = sec / 10 + 16;
    time_config[5].digitChar = sec % 10 + 16;

}

void changeTime(unsigned long *time, unsigned char add, unsigned char timePosition, unsigned char datePosition){
    unsigned short year = 1970;
    unsigned char mon = 0;
    unsigned short day = *time / SECS_DAY;

    while (day >= YEARSIZE(year)) {
        day -= YEARSIZE(year);
        year++;
    }
    while (day >= monthlen(LEAPYEAR(year), mon)) {
        day -= monthlen(LEAPYEAR(year), mon);
        mon++;
    }
    //mon++;

    switch (timePosition) {
        case 1:
            if (add) *time += 1;
            else *time -= 1;
            break;
        case 2:
            if (add) *time += 10;
            else *time -= 10;
            break;
        case 3:
            if (add) *time += 60;
            else *time -= 60;
            break;
        case 4:
            if (add) *time += 600;
            else *time -= 600;
            break;
        case 5:
            if (add) *time += 3600;
            else *time -= 3600;
            break;
//        case 6:
//            if (add) *time += 36000;
//            else *time -= 36000;
//            break;
    }

    switch (datePosition) {
            case 1:
                if (add) *time += SECS_DAY;
                else *time -= SECS_DAY;
                break;
            case 2:
                if (add) *time += monthlen(LEAPYEAR(year), mon) * SECS_DAY;
                else *time -= monthlen(LEAPYEAR(year), mon-1) * SECS_DAY;
                break;
            case 3:
                if (add) *time += YEARSIZE(year+1) * SECS_DAY;
                else *time -= YEARSIZE(year-1) * SECS_DAY;
                break;
//            case 3:
//                if (add) *time += 316224000;
//                else *time -= 316224000;
//                break;
        }
}

void calculateBlinking(unsigned char timePosition, unsigned char datePosition) {
    for (unsigned char i = 0; i < 6; ++i)
        time_config[i].blink = 0;

    switch (timePosition) {
        case 1:
            time_config[5].blink = 1;
            break;
        case 2:
            time_config[4].blink = 1;
            break;
        case 3:
            time_config[3].blink = 1;
            break;
        case 4:
            time_config[2].blink = 1;
            break;
        case 5:
            time_config[1].blink = 1;
            time_config[0].blink = 1;
            break;
    }

    for (unsigned char i = 0; i < 8; ++i)
        date_config[i].blink = 0;

    switch (datePosition) {
        case 1:
            date_config[7].blink = 1;
            date_config[6].blink = 1;
            break;
        case 2:
            date_config[5].blink = 1;
            date_config[4].blink = 1;
            break;
        case 3:
            date_config[3].blink = 1;
            date_config[2].blink = 1;
            date_config[1].blink = 1;
            date_config[0].blink = 1;
            break;
    }
}

