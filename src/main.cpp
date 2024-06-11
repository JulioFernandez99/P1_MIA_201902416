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
//? fdisk -s=100 -path=/home/julio_fernandez/Escritorio/P1_MIA_201902416/src/Disco1.dsk -name=Particion1 #id=062Disco3

//! mount
//? mount -path=/home/julio_fernandez/Escritorio/P1_MIA_201902416/src/Disco1.dsk -name=Particion3

//! unmount
//? unmount -id=651aDisco1

//! mbr
//? mbr -path=/home/julio_fernandez/Escritorio/P1_MIA_201902416/src/Disco1.dsk

//! rep
//? rep –id=061Disco1 -Path=/home/user/reports/reporte1.jpg -name=mbr