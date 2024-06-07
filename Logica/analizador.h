
#ifndef ANALIZADOR_H
#define ANALIZADOR_H

#include <iostream>
using namespace std;

class Analizador {
public:
    void Run();  // Declarar el método estático
    string ReadLine();
    void ProcessInput(const string &);
    void ProcessMkDisk(const smatch & );

    //! =================Variables mkdisk=================
    int sizeDisk=0;
    string fitDisk= "ff";
    string unitDisk= "M";
    string pathDisk="";
};

#endif // ARRAY_H
