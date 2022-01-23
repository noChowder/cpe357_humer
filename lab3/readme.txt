1. Using sizeof() properly get the sizes of certain types without manually inputting numbers or doing arithmetic of any kind. This is important for files being used on different machines. Sizeof() will get the same size on a 32-bit or 64-bit machine.

2. Getchar() will read numbers from stdin as char and not int or float. It only returns an int on failure, so you can check for success if the number it reads is of type char. On failure, you can check if the return is an int.

3. 