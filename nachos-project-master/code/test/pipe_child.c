#include "syscall.h"

int main() {
    /* ALL declarations at top */
    int myDes;
    int i;
    char buf[256];

    myDes = GetPD();   /* get the pipe descriptor passed from parent */
    pipeRead(myDes, buf, 18);
    
    /* do something with buf */
    /* Print what we received */
    for(i = 0; i < 18; i++) {
        PrintChar(buf[i]);
    }
    Halt();
    return 0;
}
