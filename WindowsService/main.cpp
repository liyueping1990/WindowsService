#include <iostream>
#include "CNTService.h"
#include "MyService.h"

void main(int argc, char** argv)
{
	MyService cntService;
	cntService.RegisterService(argc, argv);
}