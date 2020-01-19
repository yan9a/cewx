#include <stdio.h>
#include "cePPort.h"
using namespace ce;

int main(void)
{
    printf("Parallel Port Interface Test\n");
    cePPort p;
    p.WrD(0,0);
    p.WrD(0,1);
    p.WrD(0,2);
    p.WrD(0,3);

    unsigned char r=p.RdS(7)?0:1;//this busy pin is inverted    
    printf("In1 is %d \n",r);

    r=p.RdS(6)?1:0;
    printf("In2 is %d \n",r);

    r=p.RdS(5)?1:0; 
    printf("In3 is %d \n",r);

    r=p.RdS(4)?1:0;
    printf("In4 is %d \n",r);

    return 0;
}