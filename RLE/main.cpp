#include <iostream>
#include <fstream>
#include <string>

using namespace std;


// PROTOTIPOS DE FUNCIONES

string enteroAString(int numero);
int stringAEntero(const string& texto);
string comprimirRLE(const string& texto);
string descomprimirRLE(const string& comprimido);
bool verificarCoincidencia(const string& original, const string& recuperado);




int main() {
    const char* nombreArchivoEntrada = "entrada.txt";
    const char* nombreArchivoComprimido = "comprimido.txt";
    const char* nombreArchivoDescomprimido = "descomprimido.txt";

    try {
        // 1. LECTURA DEL ARCHIVO DE ENTRADA
        ifstream archivoLectura(nombreArchivoEntrada);
        if (!archivoLectura.is_open()) {
            throw "Error: No se pudo abrir el archivo de entrada (entrada.txt).";
        }

        string textoOriginal = "";
        string linea;
        while (getline(archivoLectura, linea)) {
            if (!textoOriginal.empty()) {
                textoOriginal += "\n";
            }
            textoOriginal += linea;
        }
        archivoLectura.close();

        cout << "--- CASO 5.1: COMPRESION Y DESCOMPRESION RLE CON ARCHIVOS ---\n";
        cout << "Texto original leido desde " << nombreArchivoEntrada << ":\n" << textoOriginal << endl;

        // 2. COMPRESION RLE Y ESCRITURA EN ARCHIVO
        string textoComprimido = comprimirRLE(textoOriginal);

        ofstream archivoEscrituraComp(nombreArchivoComprimido);
        if (!archivoEscrituraComp.is_open()) {
            throw "Error: No se pudo crear el archivo de salida comprimido.";
        }
        archivoEscrituraComp << textoComprimido;
        archivoEscrituraComp.close();

        cout << "\nTexto comprimido guardado en " << nombreArchivoComprimido << ":\n" << textoComprimido << endl;

        // 3. DESCOMPRESION RLE
        string textoDescomprimido = descomprimirRLE(textoComprimido);

        ofstream archivoEscrituraDescomp(nombreArchivoDescomprimido);
        if (!archivoEscrituraDescomp.is_open()) {
            throw "Error: No se pudo crear el archivo de salida descomprimido.";
        }
        archivoEscrituraDescomp << textoDescomprimido;
        archivoEscrituraDescomp.close();

        cout << "\nTexto descomprimido guardado en " << nombreArchivoDescomprimido << ":\n" << textoDescomprimido << endl;

        // 4. VERIFICACION DE COINCIDENCIA EXACTA
        if (verificarCoincidencia(textoOriginal, textoDescomprimido)) {
            cout << "\n[EXITO] La descompresion coincide exactamente con el texto original." << endl;
        } else {
            cout << "\n[ERROR] El texto reconstruido NO coincide con el original." << endl;
        }

    } catch (const char* mensajeError) {
        cerr << "\nExcepcion capturada: " << mensajeError << endl;
        return 1;
    }

    return 0;
}




// Convierte un numero entero a string sin librerias externas
string enteroAString(int numero) {
    if (numero == 0) return "0";
    string resultado = "";
    while (numero > 0) {
        resultado = static_cast<char>('0' + (numero % 10)) + resultado;
        numero /= 10;
    }
    return resultado;
}

// Convierte un string numerico a entero sin librerias externas
int stringAEntero(const string& texto) {
    int numero = 0;
    for (size_t i = 0; i < texto.length(); ++i) {
        numero = numero * 10 + (texto[i] - '0');
    }
    return numero;
}

// Comprime una cadena de texto usando RLE
string comprimirRLE(const string& texto) {
    if (texto.empty()) {
        throw "Error de compresion: El texto de entrada esta vacio.";
    }

    string comprimido = "";
    size_t n = texto.length();

    for (size_t i = 0; i < n; ++i) {
        int contador = 1;
        while (i + 1 < n && texto[i] == texto[i + 1]) {
            contador++;
            i++;
        }
        comprimido += enteroAString(contador) + texto[i];
    }

    return comprimido;
}

// Descomprime una cadena en formato RLE
string descomprimirRLE(const string& comprimido) {
    if (comprimido.empty()) {
        throw "Error de descompresion: La cadena comprimida esta vacia.";
    }

    string descomprimido = "";
    size_t n = comprimido.length();
    size_t i = 0;

    while (i < n) {
        string numStr = "";

        // Comparacion manual de rango ASCII para sustituir isdigit
        while (i < n && (comprimido[i] >= '0' && comprimido[i] <= '9')) {
            numStr += comprimido[i];
            i++;
        }

        if (numStr.empty() || i >= n) {
            throw "Error de descompresion: Formato RLE invalido.";
        }

        int repeticiones = stringAEntero(numStr);
        char caracter = comprimido[i];

        descomprimido.append(repeticiones, caracter);
        i++;
    }

    return descomprimido;
}

// Compara si la version reconstruida es exactamente igual a la original
bool verificarCoincidencia(const string& original, const string& recuperado) {
    return original == recuperado;
}