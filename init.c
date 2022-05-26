// init: The initial user-level program

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "print_uart.h"

int main(void)
{

  for(;;){
    print_uart("hello,minios!\n");
	}
}
