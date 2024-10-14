#include "GameChannel.h"

int main()
{
	ZinxKernel::ZinxKernelInit();
	/*Ìí¼Ó¼àÌýÍ¨µÀ*/
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(8899, new GameConnFact())));

	ZinxKernel::Zinx_Run();


	ZinxKernel::ZinxKernelFini();
}