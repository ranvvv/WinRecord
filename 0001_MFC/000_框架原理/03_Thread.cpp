#if 0
#include<windows.h>
#include<stdio.h>
#include"06_Thread.h"

UINT MyFunc(LPVOID lpParam)
{
	printf(" Thread Identify: %d \n", AfxGetThread()->m_nThreadID);
	return 0;
}
int main()
{
	for (int i = 0; i < 10; i++)
	{
		AfxBeginThread(MyFunc, NULL);
	}
	system("pause");
}
#endif
