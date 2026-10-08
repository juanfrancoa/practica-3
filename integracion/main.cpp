#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Estructura para el diccionario dinámico de LZ78
struct EntradaDiccionario {
    int prefijo;
    char caracter;
};


// PROTOTIPOS DE FUNCIONES

// Auxiliares para RLE
string enteroAString(int numero);
int stringAEntero(const string& texto);
string comprimirRLE(const string& texto);
string descomprimirRLE(const string& comprimido);

// Auxiliares para LZ78
void redimensionarDiccionario(EntradaDiccionario*& dic, int& capacidad);
int buscarEnDiccionario(EntradaDiccionario* dic, int tam, int prefijoActual, char c);
void comprimirLZ78(const char* archivoEntrada, const char* archivoSalida);
void descomprimirLZ78(const char* archivoEntrada, const char* archivoSalida);

// Auxiliares para Encriptacion
unsigned char rotarIzquierda(unsigned char b, int n);
unsigned char rotarDerecha(unsigned char b, int n);
unsigned char encriptarByte(unsigned char b, int n, unsigned char K);
unsigned char desencriptarByte(unsigned char b, int n, unsigned char K);
void encriptarArchivo(const char* archivoEntrada, const char* archivoSalida, int n, unsigned char K);
void desencriptarArchivo(const char* archivoEntrada, const char* archivoSalida, int n, unsigned char K);

// Control de flujo e integracion
void procesarRLE(const char* entrada, const char* comprimido, const char* encriptado,
                 const char* desencriptado, const char* finalOut, int n, unsigned char K);
void procesarLZ78(const char* entrada, const char* comprimido, const char* encriptado,
                  const char* desencriptado, const char* finalOut, int n, unsigned char K);
bool verificarCoincidenciaArchivos(const char* archivo1, const char* archivo2);




int main() {
    const char* fEntrada = "entrada.txt";
    const char* fComprimido = "comprimido.txt";
    const char* fEncriptado = "encriptado.txt";
    const char* fDesencriptado = "desencriptado.txt";
    const char* fResultadoFinal = "resultado_final.txt";

    int opcion = 0;
    int n = 0;
    char K_char;

    try {
        cout << "=============================================\n";
        cout << "   SISTEMA INTEGRADO DE COMPRESION Y ENCRIPTACION\n";
        cout << "=============================================\n";
        cout << "Seleccione el metodo de compresion:\n";
        cout << "1. Run-Length Encoding (RLE)\n";
        cout << "2. LZ78 (Diccionario Dinamico)\n";
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion != 1 && opcion != 2) {
            throw "Error: Opcion de compresion invalida.";
        }

        cout << "\n--- PARAMETROS DE ENCRIPTACION ---\n";
        cout << "Ingrese el numero de posiciones a rotar (0 < n < 8): ";
        cin >> n;

        if (n <= 0 || n >= 8) {
            throw "Error: El desplazamiento n debe ser estrictamente mayor a 0 y menor a 8.";
        }

        cout << "Ingrese el caracter clave (K): ";
        cin >> K_char;

        unsigned char K = static_cast<unsigned char>(K_char);

        cout << "\nIniciando proceso...\n";

        if (opcion == 1) {
            procesarRLE(fEntrada, fComprimido, fEncriptado, fDesencriptado, fResultadoFinal, n, K);
        } else {
            procesarLZ78(fEntrada, fComprimido, fEncriptado, fDesencriptado, fResultadoFinal, n, K);
        }

        // Verificación de integridad
        cout << "\n--- VERIFICACION DE INTEGRIDAD ---\n";
        if (verificarCoincidenciaArchivos(fEntrada, fResultadoFinal)) {
            cout << "[EXITO] El archivo final coincide exactamente con el archivo original." << endl;
        } else {
            cout << "[ERROR] El archivo recuperado no coincide con el original." << endl;
        }

    } catch (const char* mensajeError) {
        cerr << "\nExcepcion capturada: " << mensajeError << endl;
        return 1;
    }

    return 0;
}




// FUNCIONES RLE
string enteroAString(int numero) {
    if (numero == 0) return "0";
    string resultado = "";
    while (numero > 0) {
        resultado = static_cast<char>('0' + (numero % 10)) + resultado;
        numero /= 10;
    }
    return resultado;
}

int stringAEntero(const string& texto) {
    int numero = 0;
    for (size_t i = 0; i < texto.length(); ++i) {
        numero = numero * 10 + (texto[i] - '0');
    }
    return numero;
}

string comprimirRLE(const string& texto) {
    if (texto.empty()) throw "Error de compresion RLE: El texto esta vacio.";
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

string descomprimirRLE(const string& comprimido) {
    if (comprimido.empty()) throw "Error de descompresion RLE: El texto esta vacio.";
    string descomprimido = "";
    size_t n = comprimido.length();
    size_t i = 0;
    while (i < n) {
        string numStr = "";
        while (i < n && (comprimido[i] >= '0' && comprimido[i] <= '9')) {
            numStr += comprimido[i];
            i++;
        }
        if (numStr.empty() || i >= n) throw "Error de descompresion RLE: Formato invalido.";
        int repeticiones = stringAEntero(numStr);
        char caracter = comprimido[i];
        descomprimido.append(repeticiones, caracter);
        i++;
    }
    return descomprimido;
}

//  FUNCIONES LZ78
void redimensionarDiccionario(EntradaDiccionario*& dic, int& capacidad) {
    int nuevaCapacidad = capacidad * 2;
    EntradaDiccionario* nuevoDic = new EntradaDiccionario[nuevaCapacidad];
    for (int i = 0; i < capacidad; ++i) {
        nuevoDic[i] = dic[i];
    }
    delete[] dic;
    dic = nuevoDic;
    capacidad = nuevaCapacidad;
}

int buscarEnDiccionario(EntradaDiccionario* dic, int tam, int prefijoActual, char c) {
    for (int i = 1; i <= tam; ++i) {
        if (dic[i].prefijo == prefijoActual && dic[i].caracter == c) return i;
    }
    return 0;
}

void comprimirLZ78(const char* archivoEntrada, const char* archivoSalida) {
    ifstream fEntrada(archivoEntrada, ios::binary);
    if (!fEntrada.is_open()) throw "Error: No se pudo abrir el archivo para compresion LZ78.";
    ofstream fSalida(archivoSalida, ios::binary);
    if (!fSalida.is_open()) {
        fEntrada.close();
        throw "Error: No se pudo crear archivo comprimido LZ78.";
    }

    int capacidad = 10, tamDiccionario = 0, prefijoActual = 0;
    EntradaDiccionario* diccionario = new EntradaDiccionario[capacidad];
    char c;

    while (fEntrada.get(c)) {
        int idx = buscarEnDiccionario(diccionario, tamDiccionario, prefijoActual, c);
        if (idx != 0) {
            prefijoActual = idx;
        } else {
            fSalida << prefijoActual << " " << c << " ";
            tamDiccionario++;
            if (tamDiccionario >= capacidad) redimensionarDiccionario(diccionario, capacidad);
            diccionario[tamDiccionario].prefijo = prefijoActual;
            diccionario[tamDiccionario].caracter = c;
            prefijoActual = 0;
        }
    }
    if (prefijoActual != 0) fSalida << prefijoActual << " " << '\0' << " ";

    delete[] diccionario;
    fEntrada.close();
    fSalida.close();
}

void descomprimirLZ78(const char* archivoEntrada, const char* archivoSalida) {
    ifstream fEntrada(archivoEntrada, ios::binary);
    if (!fEntrada.is_open()) throw "Error: No se pudo abrir el archivo para descompresion LZ78.";
    ofstream fSalida(archivoSalida, ios::binary);
    if (!fSalida.is_open()) {
        fEntrada.close();
        throw "Error: No se pudo crear archivo descomprimido LZ78.";
    }

    int capacidad = 10, tamDiccionario = 0, i;
    EntradaDiccionario* diccionario = new EntradaDiccionario[capacidad];
    char c;

    while (fEntrada >> i) {
        fEntrada.get();
        fEntrada.get(c);

        tamDiccionario++;
        if (tamDiccionario >= capacidad) redimensionarDiccionario(diccionario, capacidad);
        diccionario[tamDiccionario].prefijo = i;
        diccionario[tamDiccionario].caracter = c;

        int tamFrase = 0, curr = tamDiccionario;
        while (curr > 0) {
            tamFrase++;
            curr = diccionario[curr].prefijo;
        }

        char* frase = new char[tamFrase];
        curr = tamDiccionario;
        int pos = tamFrase - 1;
        while (curr > 0) {
            frase[pos--] = diccionario[curr].caracter;
            curr = diccionario[curr].prefijo;
        }

        for (int k = 0; k < tamFrase; ++k) {
            if (frase[k] != '\0') fSalida.put(frase[k]);
        }
        delete[] frase;
    }

    delete[] diccionario;
    fEntrada.close();
    fSalida.close();
}

// --- FUNCIONES DE ENCRIPTACION ---
unsigned char rotarIzquierda(unsigned char b, int n) {
    return static_cast<unsigned char>((b << n) | (b >> (8 - n)));
}

unsigned char rotarDerecha(unsigned char b, int n) {
    return static_cast<unsigned char>((b >> n) | (b << (8 - n)));
}

unsigned char encriptarByte(unsigned char b, int n, unsigned char K) {
    return rotarIzquierda(b, n) ^ K;
}

unsigned char desencriptarByte(unsigned char b, int n, unsigned char K) {
    return rotarDerecha(b ^ K, n);
}

void encriptarArchivo(const char* archivoEntrada, const char* archivoSalida, int n, unsigned char K) {
    ifstream fEntrada(archivoEntrada, ios::binary);
    if (!fEntrada.is_open()) throw "Error: No se pudo abrir el archivo para encriptar.";
    ofstream fSalida(archivoSalida, ios::binary);
    if (!fSalida.is_open()) {
        fEntrada.close();
        throw "Error: No se pudo crear el archivo encriptado.";
    }

    char byteLeido;
    while (fEntrada.get(byteLeido)) {
        unsigned char enc = encriptarByte(static_cast<unsigned char>(byteLeido), n, K);
        fSalida.put(static_cast<char>(enc));
    }
    fEntrada.close();
    fSalida.close();
}

void desencriptarArchivo(const char* archivoEntrada, const char* archivoSalida, int n, unsigned char K) {
    ifstream fEntrada(archivoEntrada, ios::binary);
    if (!fEntrada.is_open()) throw "Error: No se pudo abrir el archivo para desencriptar.";
    ofstream fSalida(archivoSalida, ios::binary);
    if (!fSalida.is_open()) {
        fEntrada.close();
        throw "Error: No se pudo crear el archivo desencriptado.";
    }

    char byteLeido;
    while (fEntrada.get(byteLeido)) {
        unsigned char desenc = desencriptarByte(static_cast<unsigned char>(byteLeido), n, K);
        fSalida.put(static_cast<char>(desenc));
    }
    fEntrada.close();
    fSalida.close();
}

// --- CONTROL DE FLUJO E INTEGRACION ---
void procesarRLE(const char* entrada, const char* comprimido, const char* encriptado,
                 const char* desencriptado, const char* finalOut, int n, unsigned char K) {
    // 1. Leer original
    ifstream fIn(entrada);
    if (!fIn.is_open()) throw "Error: No se pudo abrir el archivo de entrada.";
    string contenido = "", linea;
    while (getline(fIn, linea)) {
        if (!contenido.empty()) contenido += "\n";
        contenido += linea;
    }
    fIn.close();

    // 2. Comprimir RLE y guardar
    string compStr = comprimirRLE(contenido);
    ofstream fComp(comprimido, ios::binary);
    fComp << compStr;
    fComp.close();
    cout << "1. Compresion RLE completada -> " << comprimido << endl;

    // 3. Encriptar resultado
    encriptarArchivo(comprimido, encriptado, n, K);
    cout << "2. Encriptacion completada -> " << encriptado << endl;

    // 4. Desencriptar resultado
    desencriptarArchivo(encriptado, desencriptado, n, K);
    cout << "3. Desencriptacion completada -> " << desencriptado << endl;

    // 5. Descomprimir RLE
    ifstream fDes(desencriptado, ios::binary);
    string compLeido = "", l;
    while (getline(fDes, l)) {
        if (!compLeido.empty()) compLeido += "\n";
        compLeido += l;
    }
    fDes.close();

    string descompStr = descomprimirRLE(compLeido);
    ofstream fFinal(finalOut);
    fFinal << descompStr;
    fFinal.close();
    cout << "4. Descompresion RLE completada -> " << finalOut << endl;
}

void procesarLZ78(const char* entrada, const char* comprimido, const char* encriptado,
                  const char* desencriptado, const char* finalOut, int n, unsigned char K) {
    // 1. Comprimir con LZ78
    comprimirLZ78(entrada, comprimido);
    cout << "1. Compresion LZ78 completada -> " << comprimido << endl;

    // 2. Encriptar datos comprimidos
    encriptarArchivo(comprimido, encriptado, n, K);
    cout << "2. Encriptacion completada -> " << encriptado << endl;

    // 3. Desencriptar datos
    desencriptarArchivo(encriptado, desencriptado, n, K);
    cout << "3. Desencriptacion completada -> " << desencriptado << endl;

    // 4. Descomprimir LZ78 para obtener texto original
    descomprimirLZ78(desencriptado, finalOut);
    cout << "4. Descompresion LZ78 completada -> " << finalOut << endl;
}

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