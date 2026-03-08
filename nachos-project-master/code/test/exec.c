/* exec.c
 *	Simple program to test the exec system call.
 */

#include "syscall.h"
#define stdin 0
#define stdout 1

int main() {
    int pid,pid2;
    int i,j;
    PrintString("--Starting in Parent--\n");
    pid = Exec("../test/add");
    pid2=Exec("../test/add2");
    if (pid < 0) {
        Write("Exec failed: ", 14, stdout);
        PrintNum(pid);
    } else{
	    for(i=0;i<10;i++){
		    for(j=0;j<10000;j++){
		    }
		    PrintString("In Parent\n");
	    }
        Join(pid);
	Join(pid2);
    }
}
