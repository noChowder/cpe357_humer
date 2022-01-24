1. Using sizeof() properly get the sizes of certain types without manually inputting numbers or doing arithmetic of any kind. This is important for files being used on different machines. Sizeof() will get the same size on a 32-bit or 64-bit machine.

2. Getchar() will read numbers from stdin as char and not int or float. It only returns an int on failure, so you can check for success if the number it reads is of type char. On failure, you can check if the return is an int.

3. This results in a segfault because *i cannot be accessed. The pointer *i is not pointing to any location yet or it's defaulting to a location that cannot be accessed.

#include <stdio.h>
#include <stdlib.h>

int main(){
    int *i;
    *i = 0;
    return 0;
}

4. Malloc works by looking for enough free memory that can be accessed. It uses brk/sbrk to find that address space and returns null if there is not enough space in the memory. A system call is a request to the kernal to access a resource. A library function is a request to use a function defined in a library. System calls are made if some services are required from the kernal.