#include<iostream>
#include "Logica/analizador.cpp"

using namespace std;
//! =================Comandos de prueba=================
//mkdisk -path=/home/julio_fernandez/Escritorio/MIA-VACACIONES/P1_MIA_201902416 -u=M -s=21
//mkdisk -path=/home/julio_fernandez/Escritorio/MIA-VACACIONES/P1_MIA_201902416 -u=k -s=300


int main() {
    Analizador analizador;
    analizador.Run();  // Llamada a la función miembro estática
    return 0;
}
 3