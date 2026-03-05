/* abs.c
 *      Simple program to test whether the systemcall interface works.
 *
 *
 */

#include "syscall.h"

int main() {
    int result;
    int a=-256;
    result = Abs(a);
    PrintNum(result);
    Halt();
    /* not reached */
}

