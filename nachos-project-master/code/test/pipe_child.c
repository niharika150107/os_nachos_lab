#include "syscall.h"

int main() {
    /* ALL declarations at top */
    int myDes;
    char buf[256];

    myDes = GetPD();   /* get the pipe descriptor passed from parent */
    pipeRead(myDes, buf, 18);
    
    /* do something with buf */
    
    Halt();
    return 0;
}
