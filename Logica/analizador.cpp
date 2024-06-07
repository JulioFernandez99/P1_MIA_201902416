#include <iostream>
#include <regex>
#include <string>
#include <set>
#include <sstream> // Para stringstream
#include <fstream> // Para ofstream
#include <ctime>
#include <iomanip> // Para std::put_time
#include "analizador.h"

using namespace std;

bool Analizador::InsertarMBR(const string &filePath, const Mbr &mbr) {
    // Abrir el archivo en modo binario para escritura
    std::ofstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Error al abrir el archivo: " << filePath << std::endl;
        return false;
    }

    // Establecer la posición de escritura al principio del archivo
    file.seekp(0, std::ios::beg);

    // Escribir el MBR en la posición actual del archivo
    file.write(reinterpret_cast<const char*>(&mbr), sizeof(Mbr));
    if (!file) {
        std::cerr << "Error al escribir en el archivo: " << filePath << std::endl;
        return false;
    }

    file.close();
    return true;
}



bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

void Analizador::ProcessMkDisk(const smatch &match) {
    //! =================Variables mkdisk=================
    sizeDisk=0;
    fitDisk= 'f';
    unitDisk= "M";
    pathDisk="";

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
                if (paramName == "s") {
                    sizeDisk = stoi(paramValue);
                } else if (paramName == "f"){
                    transform(paramValue.begin(), paramValue.end(), paramValue.begin(), ::tolower);
                    fitDisk = paramValue[0];
                }else if (paramName == "u"){
                    transform(paramValue.begin(), paramValue.end(), paramValue.begin(), ::tolower);
                    unitDisk = paramValue;
                }else if (paramName == "path"){
                    pathDisk = paramValue;
                }
                foundParams.insert(paramName); // Agregar el nombre del parámetro al conjunto

            }

            ++paramIterator;
        }
    }

    if (unitDisk == "k" ){
        sizeDisk = sizeDisk * 1024;
    }else if (unitDisk == "m"){
        sizeDisk = sizeDisk * 1024 * 1024;
    }
    cout << "========Parámetros encontrados========" << endl;
    cout << "Tamaño del disco: " << sizeDisk << endl;
    cout << "Ajuste del disco: " << fitDisk << endl;
    cout << "Unidad del disco: " << unitDisk << endl;
    cout << "Ruta del disco: " << pathDisk << endl;



    //? =============================Creando disco=============================
    

    int counter = 1; // Contador para nombres de archivo

    while (true) {
        // Construir el nombre del archivo único
        stringstream filename;
        filename << pathDisk << "/Disco" << counter << ".dsk";
        // Verificar si el archivo existe
        if (!fileExists(filename.str())) {
            // El archivo no existe, se puede usar este nombre
            ofstream outfile(filename.str(), ios::binary);
            if (outfile) {
                // Escribir el arreglo en el archivo
                vector<char> buffer(sizeDisk, 0); // Inicializa un vector de chars con el tamaño deseado
                outfile.write(buffer.data(), buffer.size()); // Escribe los datos en el archivo
                cout << "Archivo binario creado con un arreglo de tamaño " << sizeDisk << ": " << filename.str() << endl;
                break; // Salir del bucle una vez que se haya creado el archivo
            } else {
                cerr << "Error al crear el archivo: " << filename.str() << endl;
                
            }
        }

        // Si el archivo existe, intentar con el siguiente nombre
        ++counter;
    }

    pathDisk +="/Disco"+to_string(counter)+".dsk";
    // // Obtener la fecha y hora actual del sistema
    time_t tiempo_actual = time(nullptr);
    tm* tiempo_descompuesto = localtime(&tiempo_actual);

    // Formatear la fecha y hora como una cadena
    ostringstream oss;
    oss << put_time(tiempo_descompuesto, "%Y-%m-%d %H:%M:%S");
    string fecha_creacion = oss.str();


    Mbr mbr;
    
    mbr.mbr_tamano = sizeDisk;
    mbr.mbr_fecha_creacion = fecha_creacion;
    mbr.mbr_disk_signature = rand() % 1000 + 1;
    mbr.mbr_fit = fitDisk;
    mbr.mbr_partition_1.part_status = '0';
    mbr.mbr_partition_2.part_status = '0';
    mbr.mbr_partition_3.part_status = '0';
    mbr.mbr_partition_4.part_status = '0';
    //InsertarMBR(pathDisk , mbr);
    

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


