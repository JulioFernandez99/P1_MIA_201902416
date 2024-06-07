
#ifndef ANALIZADOR_H
#define ANALIZADOR_H

#include <iostream>
#include "structs.cpp"
using namespace std;

class Analizador {
public:
    void Run();  // Declarar el método estático
    string ReadLine();
    void ProcessInput(const string &);
    bool InsertarMBR(const string &, const Mbr &);
    void ProcessMkDisk(const smatch &);

    //! =================Variables mkdisk=================
    int sizeDisk=0;
    char fitDisk= 'f';
    string unitDisk= "M";
    string pathDisk="";
};

#endif // ARRAY_H
