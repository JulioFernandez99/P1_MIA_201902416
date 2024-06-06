#include <iostream>
#include <regex>
#include <string>
#include <set>
#include "analizador.h"
using namespace std;

void Analizador::ProcessMkDisk(const smatch &match) {
   cout << "Procesando comando mkdisk:" << endl;

    // Expresión regular para capturar los parámetros y sus valores
    regex reParam(R"((-\w+)=(("[^"]*")|(\S+)))");

    // Set para almacenar los nombres de los parámetros que ya hemos encontrado
    set<string> foundParams;

    // Itera sobre todas las coincidencias en la cadena
    for (size_t i = 0; i < match.size(); ++i) {
        string command = match[i].str();

        // Itera sobre todas las coincidencias de parámetros y valores en el comando
        sregex_iterator paramIterator(command.begin(), command.end(), reParam);
        sregex_iterator end;

        while (paramIterator != end) {
            string paramName = (*paramIterator)[1].str(); // Captura el nombre del parámetro
            paramName = paramName.substr(1); // Elimina el primer carácter "-"
            string paramValue;
            if ((*paramIterator)[3].matched) {  // Si hay un valor entre comillas dobles
                paramValue = (*paramIterator)[3].str(); // Captura el valor entre comillas dobles
                // Eliminar las comillas dobles del valor capturado
                paramValue = paramValue.substr(1, paramValue.size() - 2);
            } else {  // Si no hay valor entre comillas dobles
                paramValue = (*paramIterator)[4].str(); // Captura el valor normal
            }

            // Verificar si ya hemos encontrado este parámetro antes
            if (foundParams.find(paramName) == foundParams.end()) {
                cout << "Nombre del parámetro: " << paramName << ", Valor del parámetro: " << paramValue << endl;
                foundParams.insert(paramName); // Agregar el nombre del parámetro al conjunto
            }

            ++paramIterator;
        }
    }
}




void Analizador::ProcessInput(const string &input) {
   
    //! mkdisk
    regex reDisk(R"(mkdisk\s+(-\w+=("[^"]+"|\S+)\s*)+)");
    smatch matchMkDisk;
    regex_search(input, matchMkDisk, reDisk);

    //! rmdisk
    regex reRmdisk(R"(rmdisk\s+(-\w+=("[^"]+"|\S+)\s*)+)");
    smatch matchRmDisk;
    regex_search(input, matchRmDisk, reRmdisk);

    //! fdisk
    regex reFdisk(R"(fdisk\s+(-\w+=("[^"]+"|\S+)\s*)+)");
    smatch matchFdisk;
    regex_search(input, matchFdisk, reFdisk);

    //! mount
    regex reMount(R"(^mount\s+((-|#)\w+=("[^"]+"|\S+)\s*)+$)");
    smatch matchMount;
    regex_search(input, matchMount, reMount);

    //! unmount
    regex reUnmount(R"(^unmount\s+(-\w+=("[^"]+"|\S+)\s*)+$)");
    smatch matchUnmount;
    regex_search(input, matchUnmount, reUnmount);

    //! mkfs
    regex reKfs(R"(mkfs\s+(-\w+=("[^"]+"|\S+)\s*)+)");
    smatch matchKfs;
    regex_search(input, matchKfs, reKfs);


    // Verificar si la coincidencia no está vacía y el comando es "mkdisk"
    if (!matchMkDisk.empty() && matchMkDisk[0].str().find("mkdisk") == 0) {
        ProcessMkDisk(matchMkDisk);
    }else if (!matchRmDisk.empty() && matchRmDisk[0].str().find("rmdisk") == 0) {
        cout << "Comando rmdisk" << endl;
    }else if (!matchFdisk.empty() && matchFdisk[0].str().find("fdisk") == 0) {
        cout << "Comando fdisk" << endl;
    }else if (!matchMount.empty() && matchMount[0].str().find("mount") == 0) {
        cout << "Comando mount" << endl;
    }else if (!matchUnmount.empty() && matchUnmount[0].str().find("unmount") == 0) {
        cout << "Comando unmount" << endl;
    }else if (!matchKfs.empty() && matchKfs[0].str().find("mkfs") == 0) {
        cout << "Comando mkfs" << endl;
    }
    else {
        cout << "Comando no reconocido" << endl;
    }
    


    
    
    
    

    // smatch match;
    // if (regex_search(input, match, reDisk)){
    //     ProcessMkDisk(match);
    // }else if(regex_search(input, match, reRdisk)){
    //     cout << "Comando rmdisk" << endl;
    // }else if (regex_search(input, match, reFdisk)){
    //     cout << "Comando fdisk" << endl;  
    // }else if (regex_search(input, match, reMount)){
    //     cout << "Comando mount" << endl;
    // }else if (regex_search(input, match, reUnmount)){
    //     cout << "Comando unmount" << endl;
    // }else if (regex_search(input, match, reKfs)){
    //     cout << "Comando mkfs" << endl;
    // }
    // else {
    //     cout << "Comando no reconocido" << endl;
    // }
}


string Analizador::ReadLine() {
    string line;
    cout << ">>";
    getline(cin, line);
    return line;
}

void Analizador::Run() {
    while (true){
        string input = ReadLine();
        ProcessInput(input);
    }
    
}


