#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef IOBASE_IO
#define IOBASE_BRIDGE_NUMBER    0
#define IOBASE_MEMORY           1
#define IOBASE_IO               2
#define IOBASE_ISA_IO           3
#define IOBASE_ISA_MEM          4
#endif

#ifndef __NR_pciconfig_iobase
#define __NR_pciconfig_iobase 200
#endif

int main( void )
{
    long    iobase;
    char    *p;

//    man pages say pciconfig_iobase() exists, linker says it doesn't
//    iobase = pciconfig_iobase(IOBASE_MEMORY, 0, 0);
    iobase = syscall(__NR_pciconfig_iobase, IOBASE_MEMORY, 0, 0);
    printf("PCI memory base address: %p\n", (void*)iobase);
    iobase = syscall(__NR_pciconfig_iobase, IOBASE_IO, 0, 0);
    printf("PCI I/O base address:    %p\n", (void*)iobase);
    iobase = syscall(__NR_pciconfig_iobase, IOBASE_ISA_MEM, 0, 0);
    if ((p = getenv("SNAP_ISA_MEMBASE")) != NULL) 
        iobase = strtoul(p,NULL,16);
    printf("ISA memory base address: %p\n", (void*)iobase);
    iobase = syscall(__NR_pciconfig_iobase, IOBASE_ISA_IO, 0, 0);
    if ((p = getenv("SNAP_ISA_IOBASE")) != NULL) 
        iobase = strtoul(p,NULL,16);
    printf("ISA I/O base address:    %p\n", (void*)iobase);
    return 0;
}

