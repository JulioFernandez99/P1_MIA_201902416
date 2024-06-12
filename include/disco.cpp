#include "../lib/disco.h"
#include "../lib/structs.h"
#include "../lib/scanner.h"

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <algorithm>

using namespace std;

scanner scan;
int startValue;

Disk::Disk(){
}

void Disk::mkdisk(vector<string> tokens){
    //! tokens recibe los parametros del comando
    string size = "";
    string u = "";
    string path = "";
    string f = "";
    bool error = false;
    for(string token:tokens){
        string tk = token.substr(0, token.find("=")); // separa el token por el igual
        
        token.erase(0,tk.length()+1); //obtengo el valor del token
        
        if(scan.compare(tk, "f")){
            if(f.empty()){
                f = token; //asigno el valor del token a la variable f
            }else{
                scan.errores("MKDISK", "El parametro F ya fue ingresado en el comando"+tk);
            }
        }else if(scan.compare(tk, "s")){    //comparo si el token es igual a s
            if (size.empty())
            {
                size = token;   //asigno el valor del token a la variable size
            }else{
                scan.errores("MKDISK","parametro SIZE repetido en el comando"+tk);
            }
        }else if (scan.compare(tk, "u"))    //comparo si el token es igual a u
        {
            if (u.empty())
            {
                u = token;  //asigno el valor del token a la variable u
            }else{
                scan.errores("MKDISK","parametro U repetido en el comando"+tk);
            }
        }else if (scan.compare(tk, "path")) //comparo si el token es igual a path
        {
            
            if (path.empty())
            {

                
                path = token;   // asigno el valor del token a la variable path
            }else{
                scan.errores("MKDISK","parametro PATH repetido en el comando"+tk);
            }    
        }else{
            scan.errores("MKDISK","no se esperaba el parametro "+tk);
            error = true;
            break;
        }
    }
    if (error){ //verifico que se haya ingresado los parametros correctamente
        return;
    }

    if (f.empty()) //verifico que el parametro f no este vacio
    {
        f = "BF";
    }
    if (u.empty()) //verifico que el parametro u no este vacio
    {
        u = "M";
    }

    if (path.empty() && size.empty()) //verifico que los parametros path y size no esten vacios
    {
        scan.errores("MKDISK", "se requiere parametro Path y Size para este comando");
    }else if(path.empty()){
        scan.errores("MKDISK","se requiere parametro Path para este comando");
    }else if (size.empty())
    {
        scan.errores("MKDISK","se requiere parametro Size para este comando");
    }else if (!scan.compare(f,"BF") && !scan.compare(f,"FF") && !scan.compare(f,"WF"))
    {
        scan.errores("MKDISK","valores de parametro F no esperados");
    }else if (!scan.compare(u,"k") && !scan.compare(u,"m"))
    {
        scan.errores("MKDISK","valores de parametro U no esperados");
    }else{
        makeDisk(size,f,u,path);//sino hay errores se crea el disco en makeDisk
    }  
}

void Disk::rmdisk(vector<string> context){
    //! context recibe los parametros del comando
    for (string token:context){ //recorro los parametros
        string tk = token.substr(0, token.find("=")); //separo el token por el igual
        token.erase(0,tk.length()+1);   //obtengo el valor del token
        if (scan.compare(tk, "path")) //comparo si el token es igual a path
        {
            if (scan.confirmar("Desea eliminar el disco en la ruta: "+token+"\n"))
            {
                if (remove(token.c_str())==0) //elimino el disco (funcion remove de la libreria stdlib.h)
                {
                    scan.respuesta("RMDISK","Disco eliminado exitosamente");
                }else{
                    scan.errores("RMDISK","Error al eliminar el disco,posiblemente no existe");
                }
            }
        }
    }

}

void Disk::grafoMbr(vector<string> context){
    Structs::MBR mbr;
    for (string token:context){
        string tk = token.substr(0, token.find("="));
        token.erase(0,tk.length()+1);
        if (scan.compare(tk, "path"))
        {
            FILE *file = fopen(token.c_str(), "rb");
            if (file == NULL)
            {
                scan.errores("GETMBR","El disco no existe");
                return;
            }
            
            rewind(file);
            fread(&mbr, sizeof(Structs::MBR), 1, file);
            fclose(file);

        }
    }


    //convertir la fecha a un formato legible
    struct tm *tm;
    tm = localtime(&mbr.mbr_fecha_creacion);
    char fecha [20];
    strftime(fecha, 20, "%Y/%m/%d %H:%M:%S", tm);
    std::string fechaStr(fecha);

    // Nombre del archivo DOT
    string nombreArchivo = "/home/julio_fernandez/Escritorio/P1_MIA_201902416/Reportes/Codigo/reporteMbr.dot";

    // Contenido del grafo en formato DOT
    string contenidoDOT = "digraph G {\n";
    contenidoDOT += "  a0 [shape=none label=<<TABLE cellspacing=\"10\" cellpadding=\"10\" style=\"rounded\" bgcolor=\"red\">\n";
    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"yellow\">REPORTE MBR</TD>\n";
    contenidoDOT += "  </TR>\n";
    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"yellow\">mbr_tamano</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"yellow\">"+std::to_string(mbr.mbr_tamano)+"</TD>\n";
    contenidoDOT += "  </TR>\n";
    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"yellow\">mbr_fecha_creacion</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"yellow\">"+(fechaStr)+"</TD>\n";
    contenidoDOT += "  </TR>\n";
    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"yellow\">mbr_disk_signature</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"yellow\">"+std::to_string(mbr.mbr_disk_signature)+"</TD>\n";
    contenidoDOT += "  </TR>\n";
    
    contenidoDOT += "</TABLE>>];\n";
    contenidoDOT += "}\n";

    // Escribir el contenido en el archivo
    ofstream archivo(nombreArchivo);
    if (archivo.is_open()) {
        archivo << contenidoDOT;
        archivo.close();
        cout << "Se ha escrito el archivo " << nombreArchivo << " exitosamente." << endl;
    } else {
        cout << "No se pudo abrir el archivo " << nombreArchivo << " para escribir." << endl;
    }

    system("dot -Tpng /home/julio_fernandez/Escritorio/P1_MIA_201902416/Reportes/Codigo/reporteMbr.dot -o /home/julio_fernandez/Escritorio/P1_MIA_201902416/Reportes/Images/reporteMbr.png");
}

void Disk::grafoParticiones(vector<string> context){
    Structs::MBR mbr;
    for (string token:context){
        string tk = token.substr(0, token.find("="));
        token.erase(0,tk.length()+1);
        if (scan.compare(tk, "path"))
        {
            FILE *file = fopen(token.c_str(), "rb");
            if (file == NULL)
            {
                scan.errores("GETMBR","El disco no existe");
                return;
            }
            
            rewind(file);
            fread(&mbr, sizeof(Structs::MBR), 1, file);
            fclose(file);

        }
    }

    
     //convertir la fecha a un formato legible
    struct tm *tm;
    tm = localtime(&mbr.mbr_fecha_creacion);
    char fecha [20];
    strftime(fecha, 20, "%Y/%m/%d %H:%M:%S", tm);
    std::string fechaStr(fecha);

    // Nombre del archivo DOT
    string nombreArchivo = "/home/julio_fernandez/Escritorio/P1_MIA_201902416/Reportes/Codigo/reporteParts.dot";

    // Contenido del grafo en formato DOT
    string contenidoDOT = "digraph G {\n";
    contenidoDOT += "  a0 [shape=none label=<<TABLE cellspacing=\"10\" cellpadding=\"10\" style=\"rounded\" bgcolor=\"cornflowerblue\">\n";
    
    //particion 1
    contenidoDOT += "  <TR>\n";
    if (mbr.mbr_Partition_1.part_type == 'P')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion Primaria</TD>\n";
    }else if (mbr.mbr_Partition_1.part_type == 'E')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion Extendida</TD>\n";
    }else if (mbr.mbr_Partition_1.part_type == 'L')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion Logica</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion</TD>\n";
    }
    contenidoDOT += "  </TR>\n";
    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_status</TD>\n";

    contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::string(1,mbr.mbr_Partition_1.part_status)+"</TD>\n";
    contenidoDOT += "  </TR>\n";

    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_type</TD>\n";
    //imprimir el tipo de particion
    if (mbr.mbr_Partition_1.part_type == 'P')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">P</TD>\n";
    }else if (mbr.mbr_Partition_1.part_type == 'E')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">E</TD>\n";
    }else if (mbr.mbr_Partition_1.part_type == 'L')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">L</TD>\n";
    }
    contenidoDOT += "  </TR>\n";

    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_fit</TD>\n";

    if (mbr.mbr_Partition_1.part_fit == 'B')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">B</TD>\n";
    }else if (mbr.mbr_Partition_1.part_fit == 'F')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">F</TD>\n";    
    }else if (mbr.mbr_Partition_1.part_fit == 'W')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">W</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">----</TD>\n";
    }
    contenidoDOT += "  </TR>\n";
    
    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_start</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::to_string(mbr.mbr_Partition_1.part_start)+"</TD>\n";
    contenidoDOT += "  </TR>\n";

    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_size</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::to_string(mbr.mbr_Partition_1.part_size)+"</TD>\n";
    contenidoDOT += "  </TR>\n";
    
    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_name</TD>\n";
    if (mbr.mbr_Partition_1.part_name[0] == '\0')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">----</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::string(mbr.mbr_Partition_1.part_name)+"</TD>\n";
    }
    contenidoDOT += "  </TR>\n";

    //particion 2
    contenidoDOT += "  <TR>\n";
    if (mbr.mbr_Partition_2.part_type == 'P')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion Primaria</TD>\n";
    }else if (mbr.mbr_Partition_2.part_type == 'E')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion Extendida</TD>\n";
    }else if (mbr.mbr_Partition_2.part_type == 'L')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion Logica</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion</TD>\n";
    }
    contenidoDOT += "  </TR>\n";
    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_status</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::string(1,mbr.mbr_Partition_2.part_status)+"</TD>\n";
    contenidoDOT += "  </TR>\n";

    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_type</TD>\n";
    //imprimir el tipo de particion
    if (mbr.mbr_Partition_2.part_type == 'P')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">P</TD>\n";
    }else if (mbr.mbr_Partition_2.part_type == 'E')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">E</TD>\n";
    }else if (mbr.mbr_Partition_2.part_type == 'L')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">L</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">----</TD>\n";
    }
    contenidoDOT += "  </TR>\n";

    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_fit</TD>\n";
    if (mbr.mbr_Partition_2.part_fit == 'B')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">B</TD>\n";
    }else if (mbr.mbr_Partition_2.part_fit == 'F')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">F</TD>\n";    
    }else if (mbr.mbr_Partition_2.part_fit == 'W')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">W</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">----</TD>\n";
    }
    contenidoDOT += "  </TR>\n";
    
    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_start</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::to_string(mbr.mbr_Partition_2.part_start)+"</TD>\n";
    contenidoDOT += "  </TR>\n";

    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_size</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::to_string(mbr.mbr_Partition_2.part_size)+"</TD>\n";
    contenidoDOT += "  </TR>\n";
    
     contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_name</TD>\n";
    if (mbr.mbr_Partition_2.part_name[0] == '\0')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">----</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::string(mbr.mbr_Partition_2.part_name)+"</TD>\n";
    }
    contenidoDOT += "  </TR>\n";




    //particion 3
    contenidoDOT += "  <TR>\n";
    if (mbr.mbr_Partition_3.part_type == 'P')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion Primaria</TD>\n";
    }else if (mbr.mbr_Partition_3.part_type == 'E')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion Extendida</TD>\n";
    }else if (mbr.mbr_Partition_3.part_type == 'L')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion Logica</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion</TD>\n";
    }
    contenidoDOT += "  </TR>\n";
    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_status</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::string(1,mbr.mbr_Partition_3.part_status)+"</TD>\n";
    contenidoDOT += "  </TR>\n";

    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_type</TD>\n";
    //imprimir el tipo de particion
    if (mbr.mbr_Partition_3.part_type == 'P')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">P</TD>\n";
    }else if (mbr.mbr_Partition_3.part_type == 'E')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">E</TD>\n";
    }else if (mbr.mbr_Partition_3.part_type == 'L')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">L</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">----</TD>\n";
    }
    contenidoDOT += "  </TR>\n";

    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_fit</TD>\n";
    if (mbr.mbr_Partition_3.part_fit == 'B')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">B</TD>\n";
    }else if (mbr.mbr_Partition_3.part_fit == 'F')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">F</TD>\n";    
    }else if (mbr.mbr_Partition_3.part_fit == 'W')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">W</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">----</TD>\n";
    }
    contenidoDOT += "  </TR>\n";
    
    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_start</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::to_string(mbr.mbr_Partition_3.part_start)+"</TD>\n";
    contenidoDOT += "  </TR>\n";

    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_size</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::to_string(mbr.mbr_Partition_3.part_size)+"</TD>\n";
    contenidoDOT += "  </TR>\n";
    
     contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_name</TD>\n";
    if (mbr.mbr_Partition_3.part_name[0] == '\0')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">----</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::string(mbr.mbr_Partition_3.part_name)+"</TD>\n";
    }
    contenidoDOT += "  </TR>\n";


    //particion 4
    contenidoDOT += "  <TR>\n";
    if (mbr.mbr_Partition_4.part_type == 'P')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion Primaria</TD>\n";
    }else if (mbr.mbr_Partition_4.part_type == 'E')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion Extendida</TD>\n";
    }else if (mbr.mbr_Partition_4.part_type == 'L')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion Logica</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">Particion</TD>\n";
    }
    contenidoDOT += "  </TR>\n";
    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_status</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::to_string(mbr.mbr_Partition_4.part_status)+"</TD>\n";
    contenidoDOT += "  </TR>\n";

    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_type</TD>\n";
    //imprimir el tipo de particion
    if (mbr.mbr_Partition_4.part_type == 'P')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">P</TD>\n";
    }else if (mbr.mbr_Partition_4.part_type == 'E')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">E</TD>\n";
    }else if (mbr.mbr_Partition_4.part_type == 'L')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">L</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">----</TD>\n";
    }
    contenidoDOT += "  </TR>\n";

    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_fit</TD>\n";
    if (mbr.mbr_Partition_4.part_fit == 'B')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">B</TD>\n";
    }else if (mbr.mbr_Partition_4.part_fit == 'F')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">F</TD>\n";    
    }else if (mbr.mbr_Partition_4.part_fit == 'W')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">W</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">----</TD>\n";
    }
    contenidoDOT += "  </TR>\n";
    
    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_start</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::to_string(mbr.mbr_Partition_4.part_start)+"</TD>\n";
    contenidoDOT += "  </TR>\n";

    contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_size</TD>\n";
    contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::to_string(mbr.mbr_Partition_4.part_size)+"</TD>\n";
    contenidoDOT += "  </TR>\n";
    
     contenidoDOT += "  <TR>\n";
    contenidoDOT += "  <TD bgcolor=\"darkslategray3\">part_name</TD>\n";
    if (mbr.mbr_Partition_4.part_name[0] == '\0')
    {
        contenidoDOT += "  <TD bgcolor=\"coral1\">----</TD>\n";
    }else{
        contenidoDOT += "  <TD bgcolor=\"coral1\">"+std::string(mbr.mbr_Partition_4.part_name)+"</TD>\n";
    }
    contenidoDOT += "  </TR>\n";


    
    contenidoDOT += "</TABLE>>];\n";
    contenidoDOT += "}\n";


    






    ofstream archivo(nombreArchivo);
    if (archivo.is_open()) {
        archivo << contenidoDOT;
        archivo.close();
        cout << "Se ha escrito el archivo " << nombreArchivo << " exitosamente." << endl;
    } else {
        cout << "No se pudo abrir el archivo " << nombreArchivo << " para escribir." << endl;
    }

    system("dot -Tpng /home/julio_fernandez/Escritorio/P1_MIA_201902416/Reportes/Codigo/reporteParts.dot -o /home/julio_fernandez/Escritorio/P1_MIA_201902416/Reportes/Images/reporteParts.png");

}

void Disk::rep(vector<string> context){
    string id = "";
    string path = "";
    string name = "";


    bool error = false;
    for (string token:context){
        string tk = token.substr(0, token.find("="));
        token.erase(0,tk.length()+1);

    


        if (scan.compare(tk, "path"))
        {
            if (path.empty())
            {
                path = token;
            }else{
                scan.errores("REP","parametro PATH repetido en el comando"+tk);
                error = true;
                break;
            }
        }else if (scan.compare(tk, "name"))
        {
            if (name.empty())
            {
                name = token;
            }else{
                scan.errores("REP","parametro NAME repetido en el comando"+tk);
                error = true;
                break;
            }
        }else{
            scan.errores("REP","no se esperaba el parametro "+tk);
            error = true;
            break;
        }
    }

    cout << "ID: " << id << endl;
    cout << "Path: " << path << endl;
    cout << "Name: " << name << endl;

}

void Disk::getmbr(vector<string> context){
    //! context recibe los parametros del comando
    cout << "=======getmbr=======" << endl;
    Structs::MBR mbr;
    for (string token:context){
        string tk = token.substr(0, token.find("=")); //separo el token por el igual
        token.erase(0,tk.length()+1);
        if (scan.compare(tk, "path"))
        {
            FILE *file = fopen(token.c_str(), "rb");
            if (file == NULL)
            {
                scan.errores("GETMBR","El disco no existe");
                return;
            }
            
            rewind(file);
            fread(&mbr, sizeof(Structs::MBR), 1, file);
            fclose(file);

            cout << "Size: " << mbr.mbr_tamano << endl;

            //convertir la fecha a un formato legible
            struct tm *tm;
            tm = localtime(&mbr.mbr_fecha_creacion);
            char fecha [20];
            strftime(fecha, 20, "%Y/%m/%d %H:%M:%S", tm);

            cout << "Fecha: " << fecha << endl;
            
            
            cout << "Fit: " << mbr.disk_fit << endl;
            cout << "Disk Signature: " << mbr.mbr_disk_signature << endl;
            cout << "Bits del MBR: " << sizeof(Structs::MBR) << endl;
            cout << "Path: " << token << endl;
            cout <<"\n";
            cout << "=============Particiones=============" << endl;
            cout << "*Particion 1: " << endl;
            cout << "   Status: " << mbr.mbr_Partition_1.part_status << endl;
            cout << "   Type: " << mbr.mbr_Partition_1.part_type << endl;
            cout << "   Fit: " << mbr.mbr_Partition_1.part_fit << endl;
            cout << "   Start: " << mbr.mbr_Partition_1.part_start << endl;
            cout << "   Size: " << mbr.mbr_Partition_1.part_size << endl;
            cout << "   Name: " << mbr.mbr_Partition_1.part_name << endl;
            cout <<"\n";
            cout << "*Particion 2: " << endl;
            cout << "   Status: " << mbr.mbr_Partition_2.part_status << endl;
            cout << "   Type: " << mbr.mbr_Partition_2.part_type << endl;
            cout << "   Fit: " << mbr.mbr_Partition_2.part_fit << endl;
            cout << "   Start: " << mbr.mbr_Partition_2.part_start << endl;
            cout << "   Size: " << mbr.mbr_Partition_2.part_size << endl;
            cout << "   Name: " << mbr.mbr_Partition_2.part_name << endl;
            cout <<"\n";
            cout << "*Particion 3: " << endl;
            cout << "   Status: " << mbr.mbr_Partition_3.part_status << endl;
            cout << "   Type: " << mbr.mbr_Partition_3.part_type << endl;
            cout << "   Fit: " << mbr.mbr_Partition_3.part_fit << endl;
            cout << "   Start: " << mbr.mbr_Partition_3.part_start << endl;
            cout << "   Size: " << mbr.mbr_Partition_3.part_size << endl;
            cout << "   Name: " << mbr.mbr_Partition_3.part_name << endl;
            cout <<"\n";
            cout << "*Particion 4: " << endl;
            cout << "   Status: " << mbr.mbr_Partition_4.part_status << endl;
            cout << "   Type: " << mbr.mbr_Partition_4.part_type << endl;
            cout << "   Fit: " << mbr.mbr_Partition_4.part_fit << endl;
            cout << "   Start: " << mbr.mbr_Partition_4.part_start << endl;
            cout << "   Size: " << mbr.mbr_Partition_4.part_size << endl;
            cout << "   Name: " << mbr.mbr_Partition_4.part_name << endl;

        }
    }

    grafoMbr(context);
    grafoParticiones(context);
}


// Crear funcion makeDisk
void Disk::makeDisk(string s, string f, string u, string path){\
    Structs::MBR disco; //hago una instancia de la estructura MBR
    try{
        int size = stoi(s); // stoi = string to int, esto convierte el string a un entero
        if (size <=0){
            scan.errores("MKDISK","Size debe ser mayor a 0");
        }
        if(scan.compare(u,"M")){//si u es igual a M (esto es el tamaño del disco en MB)
            size = size * 1024 * 1024;
        }
        if(scan.compare(u,"K")){//si u es igual a K (esto es el tamaño del disco en KB)
            size = size * 1024;
        }
        f = f.substr(0,1); // BF -> B, aca tomo la primera letra de f
        disco.mbr_tamano = size;    //asigno el tamaño del disco
        disco.mbr_fecha_creacion = time(nullptr);   //asigno la fecha de creacion del disco
        disco.disk_fit = toupper(f[0]); //asigno el ajuste del disco
        disco.mbr_disk_signature = rand() % 9999 + 100; //asigno un numero aleatorio de 4 digitos

        FILE *file = fopen(path.c_str(),"r"); // c_str() = convertir string a char, esto es para abrir el archivo
        if(file != NULL){
            scan.errores("MKDISK","El disco ya existe");//si el archivo ya existe, mando un error
            fclose(file);//cierro el archivo
            return;
        }

        //inicializo las particiones
        disco.mbr_Partition_1 = Structs::Partition(); 
        disco.mbr_Partition_2 = Structs::Partition();
        disco.mbr_Partition_3 = Structs::Partition();
        disco.mbr_Partition_4 = Structs::Partition();

        string path2 = path; //creo una variable para guardar el path
        if(path.substr(0,1) == "\""){
            path2 = path.substr(1,path.length()-2); //quito las comillas del path
        };

        if(!scan.compare(path.substr(path.find_last_of(".") + 1), "dsk")){ 
            //verifico que el disco sea de tipo .dsk
            scan.errores("MKDISK","El disco debe ser de tipo .dsk");//sino mando un error
            return;
        }

        try{
            FILE *file = fopen(path.c_str(), "w+b");//creo el archivo
            if(file!=NULL){ //si el archivo se creo correctamente

                //                    inicio                       final
                // archivo de n-bytes=[ 0                            0 ]
                // se hace de esta forma para que el archivo tenga el tamaño especificado

                fwrite("\0", 1, 1, file);   //escribo un byte en el archivo
                fseek(file, size-1, SEEK_SET);  //me muevo al final del archivo
                fwrite("\0", 1, 1, file);   //escribo un byte en el archivo
                rewind(file);   //esto es para que el puntero del archivo vuelva al inicio
                fwrite(&disco, sizeof(Structs::MBR), 1, file);  //escribo el MBR en el archivo al inicio
                fclose(file);   //cierro el archivo
            }else{ //si no se pudo crear el archivo
                string comando1 = "mkdir -p \""+ path + "\""; //esta variable es para crear el directorio
                string comando2 = "rmdir \""+ path + "\""; //esta elimina el directorio
                system(comando1.c_str());   //ejecuto el comando para crear el directorio
                system(comando2.c_str());   //ejecuto el comando para eliminar el directorio
                FILE *file = fopen(path.c_str(), "w+b"); //vuelvo a intentar crear el archivo
                fwrite("\0",1,1,file);  //escribo un byte en el archivo
                fseek(file, size - 1, SEEK_SET);    //me muevo al final del archivo
                fwrite("\0", 1, 1, file);   //escribo un byte en el archivo
                rewind(file);   //esto es para que el puntero del archivo vuelva al inicio
                fwrite(&disco, sizeof(Structs::MBR),1, file);   //escribo el MBR en el archivo al inicio
                fclose(file);   //cierro el archivo
            }

            // esto es para imprimir el disco creado
            FILE *imprimir = fopen(path.c_str(), "r"); 
            if(imprimir!=NULL){
                Structs::MBR discoI;
                fseek(imprimir, 0, SEEK_SET);
                fread(&discoI,sizeof(Structs::MBR), 1,imprimir); //aca leo el MBR del disco y lo guardo en discoI
                struct tm *tm; //esto es para convertir la fecha a un formato legible
                tm = localtime(&discoI.mbr_fecha_creacion);
                char mostrar_fecha [20];
                strftime(mostrar_fecha, 20, "%Y/%m/%d %H:%M:%S", tm);                
                scan.respuesta("MKDISK","   Disco creado exitosamente");
                std::cout << "********Nuevo Disco********" << std::endl;
                std::cout << "Size:  "<< discoI.mbr_tamano << std::endl;
                std::cout << "Fecha:  "<< mostrar_fecha << std::endl;
                std::cout << "Fit:  "<< discoI.disk_fit << std::endl;
                std::cout << "Disk_Signature:  "<< discoI.mbr_disk_signature << std::endl;
                cout << "Bits del MBR:  " << sizeof(Structs::MBR) << endl;
                std::cout << "Path:  "<< path2 << std::endl;
            }
            fclose(imprimir);

        }catch(const exception& e){
            scan.errores("MKDISK","Error al crear el disco");
        }
    }catch(invalid_argument &e){
        scan.errores("MKDISK","Size debe ser un entero");
    }

}

void Disk::fdisk(vector<string> context){
    bool dlt = false;
    for (string current : context) {
        string id = shared.lower(current.substr(0, current.find('=')));
        current.erase(0, id.length() + 1);
        if (current.substr(0, 1) == "\"") {
            current = current.substr(1, current.length() - 2);
        }
        if (shared.compare(id, "delete")) {
            dlt = true;
        }
    }
    if (!dlt) {
        vector<string> required = {"s", "path", "name"};
        string size;
        string u = "k";
        string path;
        string type = "P";
        string f = "WF";
        string name;
        string add;

        for (auto current : context) {
            string id = shared.lower(current.substr(0, current.find('=')));
            current.erase(0, id.length() + 1);
            if (current.substr(0, 1) == "\"") {
                current = current.substr(1, current.length() - 2);
            }

            if (shared.compare(id, "s")) {
                if (count(required.begin(), required.end(), id)) {
                    auto itr = find(required.begin(), required.end(), id);
                    required.erase(itr);
                    size = current;
                }
            } else if (shared.compare(id, "u")) {
                u = current;
            } else if (shared.compare(id, "path")) {
                if (count(required.begin(), required.end(), id)) {
                    auto itr = find(required.begin(), required.end(), id);
                    required.erase(itr);
                    path = current;
                }
            } else if (shared.compare(id, "t")) {
                type = current;
            } else if (shared.compare(id, "f")) {
                f = current;
            } else if (shared.compare(id, "name")) {
                if (count(required.begin(), required.end(), id)) {
                    auto itr = find(required.begin(), required.end(), id);
                    required.erase(itr);
                    name = current;
                }
            } else if (shared.compare(id, "add")) {
                add = current;
                if (count(required.begin(), required.end(), "s")) {
                    auto itr = find(required.begin(), required.end(), "s");
                    required.erase(itr);
                    size = current;
                }
            }
        }
        if (!required.empty()) {
            shared.handler("FDISK", "create requiere parámetros obligatorios");
            return;
        }
        if (add.empty()) {
            generatepartition(size, u, path, type, f, name, add);
        } else {
            addpartition(add, u, name, path);
        }

    } else {
        vector<string> required = {"p", "n", "delete"};
        string _delete;
        string path;
        string name;

        for (auto current : context) {
            string id = shared.lower(current.substr(0, current.find('=')));
            current.erase(0, id.length() + 1);
            if (current.substr(0, 1) == "\"") {
                current = current.substr(1, current.length() - 2);
            }

            if (shared.compare(id, "p")) {
                if (count(required.begin(), required.end(), id)) {
                    auto itr = find(required.begin(), required.end(), id);
                    required.erase(itr);
                    path = current;
                }
            } else if (shared.compare(id, "n")) {
                if (count(required.begin(), required.end(), id)) {

                    auto itr = find(required.begin(), required.end(), id);
                    required.erase(itr);
                    name = current;
                }
            } else if (shared.compare(id, "delete")) {
                if (count(required.begin(), required.end(), id)) {

                    auto itr = find(required.begin(), required.end(), id);
                    required.erase(itr);
                    _delete = current;
                }
            }
        }
        if (required.size() != 0) {
            shared.handler("FDISK", "delete requiere parámetros obligatorios");
            return;
        }
        //deletepartition(_delete, path, name);
    }
}

vector<Structs::EBR> Disk::getlogics(Structs::Partition partition, string p) {
    vector<Structs::EBR> ebrs;

    FILE *file = fopen(p.c_str(), "rb+");
    rewind(file);
    Structs::EBR tmp;
    fseek(file, partition.part_start, SEEK_SET);
    fread(&tmp, sizeof(Structs::EBR), 1, file);
    do {
        if (!(tmp.part_status == '0' && tmp.part_next == -1)) {
            if (tmp.part_status != '0') {
                ebrs.push_back(tmp);
            }
            fseek(file, tmp.part_next, SEEK_SET);
            fread(&tmp, sizeof(Structs::EBR), 1, file);
        } else {
            fclose(file);
            break;
        }
    } while (true);
    return ebrs;
}

Structs::Partition Disk::findby(Structs::MBR mbr, string name, string path) {
    Structs::Partition partitions[4];
    partitions[0] = mbr.mbr_Partition_1;
    partitions[1] = mbr.mbr_Partition_2;
    partitions[2] = mbr.mbr_Partition_3;
    partitions[3] = mbr.mbr_Partition_4;

    bool ext = false;
    Structs::Partition extended;
    for (auto &partition : partitions) {
        if (partition.part_status == '1') {
            if (shared.compare(partition.part_name, name)) {
                return partition;
            } else if (partition.part_type == 'E') {
                ext = true;
                extended = partition;
            }
        }
    }
    if (ext) {
        vector<Structs::EBR> ebrs = getlogics(extended, path);
        for (Structs::EBR ebr : ebrs) {
            if (ebr.part_status == '1') {
                if (shared.compare(ebr.part_name, name)) {
                    Structs::Partition tmp;
                    tmp.part_status = '1';
                    tmp.part_type = 'L';
                    tmp.part_fit = ebr.part_fit;
                    tmp.part_start = ebr.part_start;
                    tmp.part_size = ebr.part_size;
                    strcpy(tmp.part_name, ebr.part_name);
                    return tmp;
                }
            }
        }
    }
    throw runtime_error("la partición no existe");
}

void Disk::generatepartition(string s, string u, string p, string t, string f, string n, string a){
    try{
        startValue = 0;
        int i = stoi(s);
        if(i <= 0){
            shared.handler("FDISK", "Size debe ser mayor a 0");
            return;
        }
        if(shared.compare(u, "b") || shared.compare(u, "k") || shared.compare(u, "m")){
            if(!shared.compare(u, "b")){
                i *= (shared.compare(u, "k") ? 1024 : 1024 * 1024);
            }
        }else{
            shared.handler("FDISK", "U debe ser b, k o m");
            return;
        }
        if(p.substr(0,1) == "\""){
            p = p.substr(1,p.length()-2);
        }
        if(!(shared.compare(t, "p") || shared.compare(t, "e") || shared.compare(t, "l"))){
            shared.handler("FDISK", "El tipo debe ser p, e o l");
            return;
        }
        if(!(shared.compare(f, "bf") || shared.compare(f, "ff") || shared.compare(f, "wf"))){
            shared.handler("FDISK", "El fit debe ser bf, ff o wf");
            return;
        }
        Structs::MBR disco;
        FILE *file = fopen(p.c_str(), "rb+");
        if(file == NULL){
            shared.handler("FDISK", "El disco no existe");
            return;
        }else{
            rewind(file);
            fread(&disco, sizeof(Structs::MBR), 1, file);
        }
        fclose(file);

        vector<Structs::Partition> particiones = getPartitions(disco); // vector de particiones, a un inicio vacio
        vector<Transition> between;

        int used = 0;
        int ext = 0;
        int c = 1;
        int base = sizeof(disco);
        Structs::Partition extended;
        for(Structs::Partition p : particiones){
            if(p.part_status == '1'){
                Transition trn;
                trn.partition = c;
                trn.start = p.part_start;
                trn.end = p.part_start + p.part_size;

                trn.before = trn.start - base;
                base = trn.end;

                if(used != 0){
                    between.at(used-1).after = trn.start - between.at(used-1).end; 
                }
                between.push_back(trn);
                used++;

                if(p.part_type == 'e' || p.part_type == 'E'){
                    ext++;
                    extended = p;
                }
            }
            if(used == 4 && !(shared.compare(t, "l"))){
                shared.handler("FDISK", "No se pueden crear mas particiones primarias");
                return;
            }else if(ext==1 && !(shared.compare(t, "e"))){
                shared.handler("FDISK", "No se pueden crear mas particiones extendidas");
                return;
            }
            c++;
        }
        if(ext == 0 && shared.compare(t, "l")){
            shared.handler("FDISK", "No se puede crear una particion logica sin una extendida");
            return;
        }
        if(used != 0){
            between.at(between.size()-1).after = disco.mbr_tamano - between.at(between.size()-1).end;
        }

        try{
            findby(disco, n,p);
            shared.handler("FDISK", "Ya existe una particion con ese nombre");
            return;
        }catch(exception &e){}

        // Aqui se crea la particion que el usuario este mandando
        Structs::Partition newPartition;
        newPartition.part_status = '1';
        newPartition.part_size = i; 
        newPartition.part_type = toupper(t[0]); // P, E, L
        newPartition.part_fit = toupper(f[0]); // B, F, W
        strcpy(newPartition.part_name, n.c_str()); // Nombre de la particion

        if(shared.compare(t, "l")){
            // Aqui se crea la particion logica
        }

        disco = adjust(disco, newPartition, between, particiones, used);

        FILE *bfile = fopen(p.c_str(), "rb+");
        if(bfile != NULL){
            fseek(bfile, 0, SEEK_SET);
            fwrite(&disco, sizeof(Structs::MBR), 1, bfile);
            if(shared.compare(t,"e")){
                Structs::EBR ebr;
                ebr.part_start = startValue;
                fseek(bfile, startValue, SEEK_SET);
                fwrite(&ebr, sizeof(Structs::EBR), 1, bfile);
            }
            fclose(bfile);
            shared.response("FDISK", "Particion creada correctamente");
        }

    }catch (invalid_argument &e) {
        shared.handler("FDISK", "-s debe ser un entero");
        return;
    }
    catch (exception &e) {
        shared.handler("FDISK", e.what());
        return;
    }
};

vector<Structs::Partition> Disk::getPartitions(Structs::MBR mbr){
    vector<Structs::Partition> partitions;
    partitions.push_back(mbr.mbr_Partition_1);
    partitions.push_back(mbr.mbr_Partition_2);
    partitions.push_back(mbr.mbr_Partition_3);
    partitions.push_back(mbr.mbr_Partition_4);
    return partitions;
}

Structs::MBR

Disk::adjust(Structs::MBR mbr, Structs::Partition p, vector<Transition> t, vector<Structs::Partition> ps, int u){
    if (u == 0) {
        p.part_start = sizeof(mbr);
        startValue = p.part_start;
        mbr.mbr_Partition_1 = p;
        return mbr;
    } else {
        Transition toUse;
        int c = 0;
        for (Transition tr : t) {
            if (c == 0) {
                toUse = tr;
                c++;
                continue;
            }

            if (toupper(mbr.disk_fit) == 'F') {
                if (toUse.before >= p.part_size || toUse.after >= p.part_size) {
                    break;
                }
                toUse = tr;
            } else if (toupper(mbr.disk_fit) == 'B') {
                if (toUse.before < p.part_size || toUse.after < p.part_size) {
                    toUse = tr;
                } else {
                    if (tr.before >= p.part_size || tr.after >= p.part_size) {
                        int b1 = toUse.before - p.part_size;
                        int a1 = toUse.after - p.part_size;
                        int b2 = tr.before - p.part_size;
                        int a2 = tr.after - p.part_size;

                        if ((b1 < b2 && b1 < a2) || (a1 < b2 && a1 < a2)) {
                            c++;
                            continue;
                        }
                        toUse = tr;
                    }
                }
            } else if (toupper(mbr.disk_fit) == 'W') {
                if (!(toUse.before >= p.part_size) || !(toUse.after >= p.part_size)) {
                    toUse = tr;
                } else {
                    if (tr.before >= p.part_size || tr.after >= p.part_size) {
                        int b1 = toUse.before - p.part_size;
                        int a1 = toUse.after - p.part_size;
                        int b2 = tr.before - p.part_size;
                        int a2 = tr.after - p.part_size;

                        if ((b1 > b2 && b1 > a2) || (a1 > b2 && a1 > a2)) {
                            c++;
                            continue;
                        }
                        toUse = tr;
                    }
                }
            }
            c++;
        }
        if (toUse.before >= p.part_size || toUse.after >= p.part_size) {
            if (toupper(mbr.disk_fit) == 'F') {
                if (toUse.before >= p.part_size) {
                    p.part_start = (toUse.start - toUse.before);
                    startValue = p.part_start;
                } else {
                    p.part_start = toUse.end;
                    startValue = p.part_start;
                }
            } else if (toupper(mbr.disk_fit) == 'B') {
                int b1 = toUse.before - p.part_size;
                int a1 = toUse.after - p.part_size;

                if ((toUse.before >= p.part_size && b1 < a1) || !(toUse.after >= p.part_start)) {
                    p.part_start = (toUse.start - toUse.before);
                    startValue = p.part_start;
                } else {
                    p.part_start = toUse.end;
                    startValue = p.part_start;
                }
            } else if (toupper(mbr.disk_fit) == 'W') {
                int b1 = toUse.before - p.part_size;
                int a1 = toUse.after - p.part_size;

                if ((toUse.before >= p.part_size && b1 > a1) || !(toUse.after >= p.part_start)) {
                    p.part_start = (toUse.start - toUse.before);
                    startValue = p.part_start;
                } else {
                    p.part_start = toUse.end;
                    startValue = p.part_start;
                }
            }
            Structs::Partition partitions[4];
            for (int i = 0; i < ps.size(); i++) {
                partitions[i] = ps.at(i);
            }
            for (auto &partition : partitions) {
                if (partition.part_status == '0') {
                    partition = p;
                    break;
                }
            }

            Structs::Partition aux;
            for (int i = 3; i >= 0; i--) {
                for (int j = 0; j < i; j++) {
                    if ((partitions[j].part_start > partitions[j + 1].part_start)) {
                        aux = partitions[j + 1];
                        partitions[j + 1] = partitions[j];
                        partitions[j] = aux;
                    }
                }
            }

            for (int i = 3; i >= 0; i--) {
                for (int j = 0; j < i; j++) {
                    if (partitions[j].part_status == '0') {
                        aux = partitions[j];
                        partitions[j] = partitions[j + 1];
                        partitions[j + 1] = aux;
                    }
                }
            }
            mbr.mbr_Partition_1 = partitions[0];
            mbr.mbr_Partition_2 = partitions[1];
            mbr.mbr_Partition_3 = partitions[2];
            mbr.mbr_Partition_4 = partitions[3];
            return mbr;
        } else {
            throw runtime_error("No hay espacio suficiente para esta particion");
        }
    }
}

void Disk::addpartition(string add, string u, string n, string p) {
    try {
        int i = stoi(add);

        if (shared.compare(u, "b") || shared.compare(u, "k") || shared.compare(u, "m")) {

            if (!shared.compare(u, "b")) {
                i *= (shared.compare(u, "k")) ? 1024 : 1024 * 1024;
            }
        } else {
            throw runtime_error("-u necesita valores específicos");
        }


        FILE *file = fopen(p.c_str(), "rb+");
        if (file == NULL) {
            throw runtime_error("disco no existente");
        }

        Structs::MBR disk;
        rewind(file);
        fread(&disk, sizeof(Structs::MBR), 1, file);

        findby(disk, n, p);

        Structs::Partition partitions[4];
        partitions[0] = disk.mbr_Partition_1;
        partitions[1] = disk.mbr_Partition_2;
        partitions[2] = disk.mbr_Partition_3;
        partitions[3] = disk.mbr_Partition_4;


        for (int i = 0; i < 4; i++) {
            if (partitions[i].part_status == '1') {
                if (shared.compare(partitions[i].part_name, n)) {
                    if ((partitions[i].part_size + (i)) > 0) {
                        if (i != 3) {
                            if (partitions[i + 1].part_start != 0) {
                                if (((partitions[i].part_size + (i) +
                                    partitions[i].part_start) <=
                                    partitions[i + 1].part_start)) {
                                    partitions[i].part_size += i;
                                    break;
                                } else {
                                    throw runtime_error("se sobrepasa el límite");
                                }
                            }
                        }
                        if ((partitions[i].part_size + i +
                            partitions[i].part_start) <= disk.mbr_tamano) {
                            partitions[i].part_size += i;
                            break;
                        } else {
                            throw runtime_error("se sobrepasa el límite");
                        }

                    }
                }
            }
        }

        disk.mbr_Partition_1 = partitions[0];
        disk.mbr_Partition_2 = partitions[1];
        disk.mbr_Partition_3 = partitions[2];
        disk.mbr_Partition_4 = partitions[3];

        rewind(file);
        fwrite(&disk, sizeof(Structs::MBR), 1, file);
        shared.response("FDISK", "la partición se ha aumentado correctamente");
        fclose(file);
    }
    catch (exception &e) {
        shared.handler("FDISK", e.what());
        return;
    }

}

