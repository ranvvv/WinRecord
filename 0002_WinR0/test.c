#include"common.h"



NTSTATUS run()
{
#ifdef _WIN64

#else
	p000_002();
#endif
	return STATUS_SUCCESS;
}

NTSTATUS cleanup()
{
#ifdef _WIN64

#else
	p000_001_cleanup();
#endif
	return STATUS_SUCCESS;
}
