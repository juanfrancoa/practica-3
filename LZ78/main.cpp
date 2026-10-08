#include <iostream>
#include <fstream>

using namespace std;

// Estructura para representar un par (indice/prefijo, caracter)
struct ParLZ78 {
    int indice;
    char caracter;
};

// Estructura para una entrada del diccionario dinámico
struct EntradaDiccionario {
    int prefijo;
    char caracter;
};


// PROTOTIPOS DE FUNCIONES

void redimensionarDiccionario(EntradaDiccionario*& dic, int& capacidad);
int buscarEnDiccionario(EntradaDiccionario* dic, int tam, int prefijoActual, char c);
void comprimirLZ78(const char* archivoEntrada, const char* archivoSalida);
void descomprimirLZ78(const char* archivoEntrada, const char* archivoSalida);
bool verificarCoincidenciaArchivos(const char* archivoOriginal, const char* archivoDescomprimido);



int main() {
    const char* nombreEntrada = "entrada.txt";
    const char* nombreComprimido = "comprimido_lz78.txt";
    const char* nombreDescomprimido = "descomprimido_lz78.txt";

    try {
        cout << "--- CASO 5.2: COMPRESION Y DESCOMPRESION LZ78 CON ARCHIVOS ---\n";

        // 1. Compresion LZ78
        comprimirLZ78(nombreEntrada, nombreComprimido);
        cout << "Compresion completada. Resultados guardados en " << nombreComprimido << endl;

        // 2. Descompresion LZ78
        descomprimirLZ78(nombreComprimido, nombreDescomprimido);
        cout << "Descompresion completada. Resultados guardados en " << nombreDescomprimido << endl;

        // 3. Verificacion
        if (verificarCoincidenciaArchivos(nombreEntrada, nombreDescomprimido)) {
            cout << "\n[EXITO] El texto reconstruido coincide exactamente con el original." << endl;
        } else {
            cout << "\n[ERROR] El texto reconstruido NO coincide con el original." << endl;
        }

    } catch (const char* mensajeError) {
        cerr << "\nExcepcion capturada: " << mensajeError << endl;
        return 1;
    }

    return 0;
}




// Redimensiona dinamicamente la capacidad del diccionario multiplicandola por 2
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

// Busca si la combinacion (prefijoActual + c) ya existe en el diccionario
int buscarEnDiccionario(EntradaDiccionario* dic, int tam, int prefijoActual, char c) {
    for (int i = 1; i <= tam; ++i) {
        if (dic[i].prefijo == prefijoActual && dic[i].caracter == c) {
            return i; // Retorna el indice del elemento encontrado
        }
    }
    return 0; // No encontrado
}

// Proceso de compresion LZ78 leyendo caracteres directamente del archivo
void comprimirLZ78(const char* archivoEntrada, const char* archivoSalida) {
    ifstream fEntrada(archivoEntrada, ios::binary);
    if (!fEntrada.is_open()) {
        throw "Error: No se pudo abrir el archivo de entrada para compresión.";
    }

    ofstream fSalida(archivoSalida, ios::binary);
    if (!fSalida.is_open()) {
        fEntrada.close();
        throw "Error: No se pudo crear el archivo comprimido de salida.";
    }

    // Inicializacion del diccionario dinamico
    int capacidad = 10;
    int tamDiccionario = 0;
    EntradaDiccionario* diccionario = new EntradaDiccionario[capacidad];

    int prefijoActual = 0;
    char c;

    while (fEntrada.get(c)) {
        int indiceEncontrado = buscarEnDiccionario(diccionario, tamDiccionario, prefijoActual, c);

        if (indiceEncontrado != 0) {
            // La subcadena ya existe en el diccionario
            prefijoActual = indiceEncontrado;
        } else {
            // Emite el par (prefijoActual, c)
            fSalida << prefijoActual << " " << c << " ";

            // Agrega la nueva entrada al diccionario
            tamDiccionario++;
            if (tamDiccionario >= capacidad) {
                redimensionarDiccionario(diccionario, capacidad);
            }
            diccionario[tamDiccionario].prefijo = prefijoActual;
            diccionario[tamDiccionario].caracter = c;

            // Reinicia el prefijo
            prefijoActual = 0;
        }
    }

    // Si al finalizar el texto queda un prefijo pendiente por emitir
    if (prefijoActual != 0) {
        fSalida << prefijoActual << " " << '\0' << " ";
    }

    // Liberacion de recursos y memoria dinamica
    delete[] diccionario;
    fEntrada.close();
    fSalida.close();
}

// Proceso de descompresion LZ78 reconstruyendo el diccionario desde los pares
void descomprimirLZ78(const char* archivoEntrada, const char* archivoSalida) {
    ifstream fEntrada(archivoEntrada, ios::binary);
    if (!fEntrada.is_open()) {
        throw "Error: No se pudo abrir el archivo comprimido para descompresión.";
    }

    ofstream fSalida(archivoSalida, ios::binary);
    if (!fSalida.is_open()) {
        fEntrada.close();
        throw "Error: No se pudo crear el archivo descomprimido de salida.";
    }

    // Inicializacion del diccionario dinamico para descompresion
    int capacidad = 10;
    int tamDiccionario = 0;
    EntradaDiccionario* diccionario = new EntradaDiccionario[capacidad];

    int i;
    char c;

    while (fEntrada >> i) {
        fEntrada.get(); // Consumir espacio en blanco
        fEntrada.get(c); // Leer el caracter emitido

        // Agregar la nueva entrada reconstruida al diccionario
        tamDiccionario++;
        if (tamDiccionario >= capacidad) {
            redimensionarDiccionario(diccionario, capacidad);
        }
        diccionario[tamDiccionario].prefijo = i;
        diccionario[tamDiccionario].caracter = c;

        // Reconstruccion de la frase usando arreglos dinamicos
        int tamFrase = 0;
        int curr = tamDiccionario;

        // Calcular longitud de la frase
        while (curr > 0) {
            tamFrase++;
            curr = diccionario[curr].prefijo;
        }

        char* frase = new char[tamFrase];
        curr = tamDiccionario;
        int pos = tamFrase - 1;

        // Llenar el arreglo de atras hacia adelante
        while (curr > 0) {
            frase[pos--] = diccionario[curr].caracter;
            curr = diccionario[curr].prefijo;
        }

        // Escribir la frase reconstruida en el archivo final
        for (int k = 0; k < tamFrase; ++k) {
            if (frase[k] != '\0') {
                fSalida.put(frase[k]);
            }
        }

        delete[] frase; // Liberar arreglo temporal de la frase
    }

    // Liberacion de memoria dinamica y cierre de archivos
    delete[] diccionario;
    fEntrada.close();
    fSalida.close();
}

// Compara caracter a caracter si el archivo original y el descomprimido son identicos
// Compara caracter a caracter si el archivo original y el descomprimido son identicos
bool verificarCoincidenciaArchivos(const char* archivoOriginal, const char* archivoDescomprimido) {
    ifstream f1(archivoOriginal, ios::binary);
    ifstream f2(archivoDescomprimido, ios::binary);

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

        // Si ambos llegaron al final del archivo simultaneamente, son iguales
        if (!b1 && !b2) {
            f1.close();
            f2.close();
            return true;
        }

        // Si uno finalizo antes que el otro o los caracteres no coinciden
        if (b1 != b2 || c1 != c2) {
            f1.close();
            f2.close();
            return false;
        }
    }
}