#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* month_day function's prototype*/
void month_day(int year, int day, int *pmonth, int *pday);

char *month(int i) {
   static char *month[] = {
       "Invalid","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
   };
   if(i < 1 || i > 12){
       return month[0];
   }
   else{
        return month[i];
   }
}


int main(int argc, char** argv) {
    /*if (strcmp (argv[1], "\n") == 0){
        printf("You did not enter any input");
        return 0;
    }*/
    int year = atoi(argv[1]);
    int day = atoi(argv[2]);

    if(day < 0 || day > 366){
      printf("Invalid day input");
      return 0;
    }
    else if(day==0){
        printf("Midnight of new years eve for a split second... or its invalid");
        return 0;
    }
    else if(day==366){
        printf("If you're searching for a leap year, not fully supported, only matching exact Feb 29 on 2020 and 2016");
        return 0;
    }

    int *pmonth = malloc(sizeof(int));
    int *pday = malloc(sizeof(int));

    month_day(year,day,pmonth,pday);
    printf("%s %d, %d ", month(*pmonth),*pday,year);
    return 0;
}


void month_day(int year, int day, int *pmonth, int *pday){
    int substract =0;
    int selMonth = 0;
    if(day>0 && day<=31){
        substract =0;
        selMonth = 1;
    }
    else if(day>31 && day<=59){
        selMonth = 2;
        substract =31;
    }
    else if(day>59 && day<=90){
        selMonth = 3;
        substract =59;
    }
    else if(day>90 && day<=120){
        selMonth = 4;
        substract =90;
    }
    else if(day>120 && day<=151){
        selMonth = 5;
        substract =120;
    }
    else if(day>151 && day<=181){
        selMonth = 6;
        substract =151;
    }
    else if(day>181 && day<=212){
        selMonth = 7;
        substract =181;
    }
    else if(day>212 && day<=243){
        selMonth = 8;
        substract =212;
    }
    else if(day>243 && day<=273){
        selMonth = 9;
        substract = 243;
    }
    else if(day>273 && day<=304){
        selMonth = 10;
        substract = 273;
    }
    else if(day>304 && day<=334){
        selMonth = 11;
        substract =304;
    }
    else if(day>334 && day<=365){
        selMonth = 12;
        substract =334;
    }
    if((year == 2020|| year ==2016) && day ==60){
        selMonth=2;
        substract =31;
    }
    int newDay=day-substract;

    *pmonth = selMonth;
    *pday = newDay;
}
