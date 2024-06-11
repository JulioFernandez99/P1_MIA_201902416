#include "../lib/mount.h"

#include <iostream>
#include <stdlib.h>
#include "string"
#include <locale>

using namespace std;

Mount::Mount(){}

void Mount::mount(vector<string> command){
    if(command.empty()){
        listmount();
        return;
    }
    vector<string> required = { "path", "name" };
    string path;
    string name;

    for (auto current : command){ // path=/home
        string id = shared.lower(current.substr(0, current.find("=")));
        current.erase(0, id.length() + 1);
        if(current.substr(0,1) == "\""){
            current.substr(1,current.length()-2);
        };

        if(shared.compare(id, "name")){
            if(count(required.begin(), required.end(), id)){
                auto itr = find(required.begin(), required.end(), id);
                required.erase(itr);
                name = current;
            }
        } else if (shared.compare(id, "path")) {
            if (count(required.begin(), required.end(), id)) {
                auto itr = find(required.begin(), required.end(), id);
                required.erase(itr);
                path = current;
            }
        }
    };
    if (required.size() != 0) {
        shared.handler("MOUNT", "requiere ciertos parámetros obligatorios");
        return;
    };
    mount(path, name);
}

void Mount::mount(string p, string n){
    try{
        FILE *validate = fopen(p.c_str(), "r");
        if (validate == NULL) {
            shared.handler("MOUNT", "Disco no existe");
        }
        Structs::MBR disk;
        rewind(validate);
        fread(&disk, sizeof(Structs::MBR), 1, validate);
        fclose(validate);
        
        Structs::Partition partition = dsk.findby(disk, n, p);
        if (partition.part_type == 'E') {
            vector<Structs::EBR> ebrs = dsk.getlogics(partition, p);
            if (!ebrs.empty()) {
                Structs::EBR ebr = ebrs.at(0);
                n = ebr.part_name;
                //shared.handler("", "se montará una partición lógica");
            } else {
                throw runtime_error("No se puede montar una extendida");
            }
        }

        for (int i = 0; i < 99; i++) {
            if (mounted[i].path == p) {
                for (int j = 0; j < 26; j++) {
                    if (Mount::mounted[i].mpartitions[j].status == '0') {
                        mounted[i].mpartitions[j].status = '1';
                        mounted[i].mpartitions[j].letter = alfabeto.at(j);
                        strcpy(mounted[i].mpartitions[j].name, n.c_str());
                        string re = to_string(i + 1) + alfabeto.at(j);
                        shared.response("MOUNT", "se ha realizado correctamente el mount -id=16" + re);
                        return;
                    }
                }
            }
        }

        for (int i = 0; i < 99; i++) {
            if (mounted[i].status == '0') {
                mounted[i].status = '1';
                strcpy(mounted[i].path, p.c_str());
                for (int j = 0; j < 26; j++) {
                    if (Mount::mounted[i].mpartitions[j].status == '0') {
                        mounted[i].mpartitions[j].status = '1';
                        mounted[i].mpartitions[j].letter = alfabeto.at(j);
                        strcpy(mounted[i].mpartitions[j].name, n.c_str());
                        string re = to_string(i + 1) + alfabeto.at(j);
                        shared.response("MOUNT", "se ha realizado correctamente el mount -id=16" + re);
                        return;
                    }
                }
            }
        }

    }catch(exception &e){
        shared.handler("MOUNT", e.what());
        return;
    }

}

void Mount::listmount() {
    cout << "\n<-------------------------- MOUNTS -------------------------->"
        << endl;
    for (int i = 0; i < 99; i++) {
        for (int j = 0; j < 26; j++) {
            if (mounted[i].mpartitions[j].status == '1') {
                cout << "> 16" << i + 1 << alfabeto.at(j) << ", " << mounted[i].mpartitions[j].name << endl;
            }
        }
    }
}

void Mount::unmount(vector<string> command){
    if(command.empty()){
        shared.handler("UNMOUNT", "requiere ciertos parámetros obligatorios");
        return;
    }
    vector<string> required = { "id" };
    string idcomand;
    for (auto current : command){
        string id = shared.lower(current.substr(0, current.find("=")));
        current.erase(0, id.length() + 1);
        if(shared.compare(id, "id")){
            if(count(required.begin(), required.end(), id)){
                auto itr = find(required.begin(), required.end(), id);
                required.erase(itr);
                idcomand = current;
                
            }
        }
    }
    if (required.size() != 0) {
        shared.handler("UNMOUNT", "requiere ciertos parámetros obligatorios");
        return;
    }
    
    unmount(idcomand);
}

void Mount::unmount(string id){

   
    //extraer el tercer caracter del id
    char idd = id.at(2);
    //convertir el caracter a entero
    int particion = idd - 48;

    //extraer del cuarto caracter en adelante
    string disco = "/home/julio_fernandez/Escritorio/P1_MIA_201902416/src/"+ id.substr(4)+".dsk";
    
    cout << particion << endl;
    cout << disco << endl;

    
    //obtener el mbr del disco
    Structs::MBR disk;
    FILE *validate = fopen(disco.c_str(), "r");
    if (validate == NULL) {
        shared.handler("UNMOUNT", "Disco no existe");
    }
    rewind(validate);
    fread(&disk, sizeof(Structs::MBR), 1, validate);
    fclose(validate);
    
    
    if (particion == 1) {
        disk.mbr_Partition_1.part_status = '0';
    } else if (particion == 2) {
        disk.mbr_Partition_2.part_status = '0';
    } else if (particion == 3) {
        disk.mbr_Partition_3.part_status = '0';
    } else if (particion == 4) {
        disk.mbr_Partition_4.part_status = '0';
    }else{
        shared.handler("UNMOUNT", "partición no existe");
    }
    


    FILE *discoo = fopen(disco.c_str(), "r+");
    if (discoo == NULL) {
        shared.handler("UNMOUNT", "Disco no existe");
    }
    rewind(discoo);
    fwrite(&disk, sizeof(Structs::MBR), 1, discoo);
    fclose(discoo);
    

    


}


Structs::Partition Mount::getmount(string id, string *p) {

    if (!(id[0] == '6' && id[1] == '5')) {
        throw runtime_error("el primer identificador no es válido");
    }
    string past = id;
    char letter = id[id.length() - 1];
    id.erase(0, 2);
    id.pop_back();
    int i = stoi(id) - 1;
    if (i < 0) {
        throw runtime_error("identificador de disco inválido");
    }

    for (int j = 0; j < 26; j++) {
        if (mounted[i].mpartitions[j].status == '1') {
            if (mounted[i].mpartitions[j].letter == letter) {

                FILE *validate = fopen(mounted[i].path, "r");
                if (validate == NULL) {
                    throw runtime_error("disco no existente");
                }

                Structs::MBR disk;
                rewind(validate);
                fread(&disk, sizeof(Structs::MBR), 1, validate);
                fclose(validate);
                *p = mounted[i].path;
                return dsk.findby(disk, mounted[i].mpartitions[j].name, mounted[i].path);
            }
        }
    }
    throw runtime_error("partición no existente");
}