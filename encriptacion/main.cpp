#include <iostream>
#include <fstream>

using namespace std;


// PROTOTIPOS DE FUNCIONES

unsigned char rotarIzquierda(unsigned char b, int n);
unsigned char rotarDerecha(unsigned char b, int n);
unsigned char encriptarByte(unsigned char b, int n, unsigned char K);
unsigned char desencriptarByte(unsigned char b, int n, unsigned char K);
void encriptarArchivo(const char* archivoEntrada, const char* archivoSalida, int n, unsigned char K);
void desencriptarArchivo(const char* archivoEntrada, const char* archivoSalida, int n, unsigned char K);
bool verificarCoincidenciaArchivos(const char* archivo1, const char* archivo2);




int main() {
    const char* nombreEntrada = "entrada.txt";
    const char* nombreEncriptado = "encriptado.bin";
    const char* nombreDesencriptado = "desencriptado.txt";

    int n = 3;                  // Desplazamiento de bits (0 < n < 8)
    unsigned char K = 'X';      // Clave de encriptacion (1 byte)

    try {
        cout << "--- CASO 5.3: ENCRIPTACION Y DESENCRIPTACION DE DATOS ---\n";

        // Validación del parámetro de rotación
        if (n <= 0 || n >= 8) {
            throw "Error: El valor de rotacion n debe cumplir (0 < n < 8).";
        }

        // 1. Encriptacion
        encriptarArchivo(nombreEntrada, nombreEncriptado, n, K);
        cout << "Encriptacion completada. Archivo generado: " << nombreEncriptado << endl;

        // 2. Desencriptacion
        desencriptarArchivo(nombreEncriptado, nombreDesencriptado, n, K);
        cout << "Desencriptacion completada. Archivo generado: " << nombreDesencriptado << endl;

        // 3. Verificacion
        if (verificarCoincidenciaArchivos(nombreEntrada, nombreDesencriptado)) {
            cout << "\n[EXITO] El texto desencriptado coincide exactamente con el archivo original." << endl;
        } else {
            cout << "\n[ERROR] El texto desencriptado NO coincide con el original." << endl;
        }

    } catch (const char* mensajeError) {
        cerr << "\nExcepcion capturada: " << mensajeError << endl;
        return 1;
    }

    return 0;
}





// Rotacion circular a la izquierda sobre un byte (8 bits)
unsigned char rotarIzquierda(unsigned char b, int n) {
    return static_cast<unsigned char>((b << n) | (b >> (8 - n)));
}

// Rotacion circular a la derecha sobre un byte (8 bits)
unsigned char rotarDerecha(unsigned char b, int n) {
    return static_cast<unsigned char>((b >> n) | (b << (8 - n)));
}

// Proceso de encriptacion de un byte: 1. Rotacion Izq -> 2. Operacion XOR con clave K
unsigned char encriptarByte(unsigned char b, int n, unsigned char K) {
    unsigned char rotado = rotarIzquierda(b, n);
    return rotado ^ K;
}

// Proceso de desencriptacion de un byte: 1. Operacion XOR con clave K -> 2. Rotacion Der
unsigned char desencriptarByte(unsigned char b, int n, unsigned char K) {
    unsigned char xorAplicado = b ^ K;
    return rotarDerecha(xorAplicado, n);
}

// Lee un archivo byte a byte, encripta y guarda el resultado
void encriptarArchivo(const char* archivoEntrada, const char* archivoSalida, int n, unsigned char K) {
    ifstream fEntrada(archivoEntrada, ios::binary);
    if (!fEntrada.is_open()) {
        throw "Error: No se pudo abrir el archivo de entrada para encriptar.";
    }

    ofstream fSalida(archivoSalida, ios::binary);
    if (!fSalida.is_open()) {
        fEntrada.close();
        throw "Error: No se pudo crear el archivo de salida para guardar los datos encriptados.";
    }

    char byteLeido;
    while (fEntrada.get(byteLeido)) {
        unsigned char byteOriginal = static_cast<unsigned char>(byteLeido);
        unsigned char byteEncriptado = encriptarByte(byteOriginal, n, K);
        fSalida.put(static_cast<char>(byteEncriptado));
    }

    fEntrada.close();
    fSalida.close();
}

// Lee un archivo encriptado byte a byte, desencripta y guarda el resultado original
void desencriptarArchivo(const char* archivoEntrada, const char* archivoSalida, int n, unsigned char K) {
    ifstream fEntrada(archivoEntrada, ios::binary);
    if (!fEntrada.is_open()) {
        throw "Error: No se pudo abrir el archivo encriptado para desencriptar.";
    }

    ofstream fSalida(archivoSalida, ios::binary);
    if (!fSalida.is_open()) {
        fEntrada.close();
        throw "Error: No se pudo crear el archivo de salida para guardar los datos desencriptados.";
    }

    char byteLeido;
    while (fEntrada.get(byteLeido)) {
        unsigned char byteEncriptado = static_cast<unsigned char>(byteLeido);
        unsigned char byteDesencriptado = desencriptarByte(byteEncriptado, n, K);
        fSalida.put(static_cast<char>(byteDesencriptado));
    }

    fEntrada.close();
    fSalida.close();
}

// Compara caracter a caracter si el archivo original y el desencriptado son identicos
bool verificarCoincidenciaArchivos(const char* archivo1, const char* archivo2) {
    ifstream f1(archivo1, ios::binary);
    ifstream f2(archivo2, ios::binary);

    if (!f1.is_open() || !f2.is_open()) {
        if (f1.is_open()) f1.close();
        if (f2.is_open()) f2.close();
        return false;
    }

    char c1, c2;
    bool b1, b2;

    while (true) {
        b1 = static_cast<bool>(f1.get(c1));
        b2 = static_cast<bool>(f2.get(c2));

        if (!b1 && !b2) {
            f1.close();
            f2.close();
            return true;
        }

        if (b1 != b2 || c1 != c2) {
            f1.close();
            f2.close();
            return false;
        }
    }
}