#include <iostream>
#include "../lib/scanner.h"
int main() {
    scanner scan;
    scan.start();
}

//! mkdik
//? mkdisk -s=3000 -u=K -path=/home/julio_fernandez/Escritorio/P1_MIA_201902416/src/Disco1.dsk

//! rmdisk
//? rmdisk -path=/home/julio_fernandez/Escritorio/P1_MIA_201902416/src/Disco1.dsk

//! fdisk
//? fdisk -s=100 -path=/home/julio_fernandez/Escritorio/P1_MIA_201902416/src/Disco1.dsk -name=Particion4 #id=062Disco1

//! mount
//? mount -path=/home/julio_fernandez/Escritorio/P1_MIA_201902416/src/Disco1.dsk -name=Particion4

//! unmount
//? unmount -id=161aDisco1

//! mbr
//? mbr -path=/home/julio_fernandez/Escritorio/P1_MIA_201902416/src/Disco1.dsk


