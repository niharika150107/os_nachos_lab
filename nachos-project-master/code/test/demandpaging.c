#include "syscall.h"
int bigArray[3500];
int main()
{
   int i;
   for(i=0;i<3500;i++)
      bigArray[i]=i;
   for(i=0;i<3500;i++) {
      PrintNum(bigArray[i]);
      PrintChar(' ');
   }
   PrintString("\n\nTotal Page Faults: ");
   PrintNum(GetPageFaults());
   PrintChar('\n');
   Exit(0);
}
