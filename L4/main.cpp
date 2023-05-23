#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

// Representation marco

#define to_decimal(bcd) ((bcd / 16 * 10) + (bcd % 16))
#define to_bcd(decimal) ((decimal / 10 * 16) + (decimal % 10))

// Data structures

typedef unsigned char byte;

struct datetime {
    byte wday, mday, month, year;
    byte secs, mins, hours;
};

// Constants 

const unsigned int datetime_registers[] = {
    0x06, 0x07, 0x08, 0x09, // wday, mday, month, year
    0x00, 0x02, 0x04,       // secs, mins, hours
};

const char* const month[] = {
    "January" , "February" , "March"     ,     
    "April"   , "May"      , "June"      ,      
    "July"    , "August"   , "September" , 
    "October" , "November" , "December"  ,
};

const char* const week[] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
};

// New interrupt handlers

void interrupt new_time_int(...);
void interrupt new_alarm_int(...);

// Old interrupt handlers

void interrupt(*old_time_int)(...);
void interrupt(*old_alarm_int)(...);

// Datetime interaction functions

void set_datetime(struct datetime* dt);
void get_datetime(struct datetime* dt);

// User interface procedures

void print_datetime(struct datetime* dt);

int main()
{
    return 0;
}

void set_datetime(struct datetime* dt)
{
    byte* output = (byte*) dt;
    int i;
    byte status;
    
    _disable();
    
    do {
        outp(0x70, 0xA);
        status = inp(0x71) & 0x80;
    }
    while(status); // wait "updating" status disabled
    
    outp(0x70, 0xB);
    outp(0x71, inp(0x71) | 0x80); // disable update
    
    for(i = 0; i < 7; i++){
        outp(0x70, datetime_registers[i]);
        outp(0x71, to_bcd(output[i]));
    }
    
    outp(0x70, 0xB);
    outp(0x71, inp(0x71) & 0x7F); // enable update
    
    _enable();
}

void get_datetime(struct datetime* dt)
{
    int i;
    byte* input = (byte*) dt;
    
    for(i = 0; i < 7; i++){
        outp(0x70, datetime_registers[i]);
        input[i] = inp(0x71);
        input[i] = to_decimal(input[i]);
    }
}

void print_datetime(struct datetime* dt)
{
    printf("%s, %s %02u, 20%02u %02u:%02u:%02u\n", 
           week[dt->wday],
           month[dt->month],
           dt->mday,
           dt->year,
           dt->hours, dt->mins, dt->secs);
}

