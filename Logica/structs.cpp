#include <iostream>
#include <string>

using namespace std;

struct Particion{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_s;//size
    char part_name[16];
};

struct Mbr{
    int mbr_tamano;
    string mbr_fecha_creacion;
    int mbr_disk_signature;
    char mbr_fit;
    Particion mbr_partition_1;
    Particion mbr_partition_2;
    Particion mbr_partition_3;
    Particion mbr_partition_4;
};