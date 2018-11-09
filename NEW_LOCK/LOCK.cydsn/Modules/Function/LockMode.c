#include "Function.h"

uint8_t LockMode(void)
{
    Function_status()->workmode = NORMAL_MODE;
	return 0;
}
