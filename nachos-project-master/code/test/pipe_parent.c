#include "syscall.h"

int main() {
    /* 1. ALL declarations at the top */
    int readDes;
    int writeDes;
    int childPid;
    char *msg;

    /* 2. Then statements */
    msg = "Hello from parent!";
    
    Pipe(&readDes, &writeDes);
    pipeWrite(writeDes, msg, 18);
    
    childPid = ExecP("pipe_child", readDes);
    Join(childPid);   /* 3. No ... , use the actual variable */
    
    Halt();
    return 0;
}
