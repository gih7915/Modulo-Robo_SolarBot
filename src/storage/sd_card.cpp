#include "sd_card.h"
#include "../config.h"
#include <SD.h>
#include <SPI.h>

static bool sdAvailable = false;

bool sd_init() {
    Serial.println("\n=== Inicializando Cartão SD ===");
    
    // Inicializa SPI com os pinos configurados
    SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    
    // Tenta montar o cartão SD
    if (!SD.begin(SD_CS)) {
        Serial.println("❌ Falha ao montar o cartão SD!");
        Serial.println("Verifique:");
        Serial.println("  - Cartão inserido corretamente");
        Serial.println("  - Conexões dos pinos (MISO, MOSI, SCK, CS)");
        Serial.println("  - Cartão formatado em FAT32");
        sdAvailable = false;
        return false;
    }
    
    Serial.println("✓ Cartão SD montado com sucesso!");
    sdAvailable = true;
    
    // Mostra informações do cartão
    sd_printCardInfo();
    
    return true;
}

bool sd_isAvailable() {
    return sdAvailable;
}

void sd_printCardInfo() {
    if (!sdAvailable) {
        Serial.println("Cartão SD não disponível");
        return;
    }
    
    uint8_t cardType = SD.cardType();
    
    Serial.println("\n--- Informações do Cartão SD ---");
    
    if (cardType == CARD_NONE) {
        Serial.println("Tipo: Nenhum cartão detectado");
        return;
    }
    
    Serial.print("Tipo: ");
    switch(cardType) {
        case CARD_MMC:
            Serial.println("MMC");
            break;
        case CARD_SD:
            Serial.println("SDSC");
            break;
        case CARD_SDHC:
            Serial.println("SDHC");
            break;
        default:
            Serial.println("Desconhecido");
    }
    
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("Tamanho: %llu MB\n", cardSize);
    
    uint64_t totalBytes = SD.totalBytes() / (1024 * 1024);
    uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);
    
    Serial.printf("Espaço Total: %llu MB\n", totalBytes);
    Serial.printf("Espaço Usado: %llu MB\n", usedBytes);
    Serial.printf("Espaço Livre: %llu MB\n", totalBytes - usedBytes);
    Serial.println("--------------------------------\n");
}

bool sd_writeFile(const char* path, const char* message) {
    if (!sdAvailable) {
        Serial.println("SD não disponível para escrita");
        return false;
    }
    
    Serial.printf("Escrevendo em: %s\n", path);
    
    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Falha ao abrir arquivo para escrita");
        return false;
    }
    
    if (file.print(message)) {
        Serial.println("✓ Arquivo escrito com sucesso");
    } else {
        Serial.println("❌ Falha na escrita");
        file.close();
        return false;
    }
    
    file.close();
    return true;
}

bool sd_appendFile(const char* path, const char* message) {
    if (!sdAvailable) {
        Serial.println("SD não disponível para append");
        return false;
    }
    
    File file = SD.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Falha ao abrir arquivo para append");
        return false;
    }
    
    if (file.print(message)) {
        // Serial.println("✓ Dados adicionados ao arquivo");
    } else {
        Serial.println("❌ Falha ao adicionar dados");
        file.close();
        return false;
    }
    
    file.close();
    return true;
}

String sd_readFile(const char* path) {
    if (!sdAvailable) {
        Serial.println("SD não disponível para leitura");
        return String("");
    }
    
    Serial.printf("Lendo arquivo: %s\n", path);
    
    File file = SD.open(path);
    if (!file) {
        Serial.println("Falha ao abrir arquivo para leitura");
        return String("");
    }
    
    String content = "";
    while (file.available()) {
        content += (char)file.read();
    }
    
    file.close();
    Serial.printf("✓ Lido %d bytes\n", content.length());
    return content;
}

void sd_listDir(const char* dirname, uint8_t levels) {
    if (!sdAvailable) {
        Serial.println("SD não disponível");
        return;
    }
    
    Serial.printf("Listando diretório: %s\n", dirname);
    
    File root = SD.open(dirname);
    if (!root) {
        Serial.println("Falha ao abrir diretório");
        return;
    }
    
    if (!root.isDirectory()) {
        Serial.println("Não é um diretório");
        return;
    }
    
    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) {
                sd_listDir(file.path(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\t\t");
            Serial.print(file.size());
            Serial.println(" bytes");
        }
        file = root.openNextFile();
    }
}

bool sd_deleteFile(const char* path) {
    if (!sdAvailable) {
        Serial.println("SD não disponível");
        return false;
    }
    
    Serial.printf("Deletando arquivo: %s\n", path);
    
    if (SD.remove(path)) {
        Serial.println("✓ Arquivo deletado");
        return true;
    } else {
        Serial.println("❌ Falha ao deletar");
        return false;
    }
}

bool sd_renameFile(const char* path1, const char* path2) {
    if (!sdAvailable) {
        Serial.println("SD não disponível");
        return false;
    }
    
    Serial.printf("Renomeando %s para %s\n", path1, path2);
    
    if (SD.rename(path1, path2)) {
        Serial.println("✓ Arquivo renomeado");
        return true;
    } else {
        Serial.println("❌ Falha ao renomear");
        return false;
    }
}

bool sd_createDir(const char* path) {
    if (!sdAvailable) {
        Serial.println("SD não disponível");
        return false;
    }
    
    Serial.printf("Criando diretório: %s\n", path);
    
    if (SD.mkdir(path)) {
        Serial.println("✓ Diretório criado");
        return true;
    } else {
        Serial.println("❌ Falha ao criar diretório");
        return false;
    }
}

bool sd_removeDir(const char* path) {
    if (!sdAvailable) {
        Serial.println("SD não disponível");
        return false;
    }
    
    Serial.printf("Removendo diretório: %s\n", path);
    
    if (SD.rmdir(path)) {
        Serial.println("✓ Diretório removido");
        return true;
    } else {
        Serial.println("❌ Falha ao remover diretório");
        return false;
    }
}

void sd_testCardFunctions() {
    if (!sdAvailable) {
        Serial.println("Cartão SD não disponível para teste");
        return;
    }
    
    Serial.println("\n=== TESTE COMPLETO DO CARTÃO SD ===\n");
    
    // Teste 1: Listar diretório raiz
    Serial.println("1. Listando diretório raiz:");
    sd_listDir("/", 0);
    
    // Teste 2: Criar diretório
    Serial.println("\n2. Criando diretório /teste");
    sd_createDir("/teste");
    
    // Teste 3: Escrever arquivo
    Serial.println("\n3. Escrevendo arquivo /teste/hello.txt");
    sd_writeFile("/teste/hello.txt", "Olá do ESP32!\n");
    
    // Teste 4: Adicionar ao arquivo
    Serial.println("\n4. Adicionando texto ao arquivo");
    sd_appendFile("/teste/hello.txt", "Segunda linha de texto.\n");
    sd_appendFile("/teste/hello.txt", "Terceira linha de texto.\n");
    
    // Teste 5: Ler arquivo
    Serial.println("\n5. Lendo arquivo:");
    String content = sd_readFile("/teste/hello.txt");
    Serial.println("Conteúdo:");
    Serial.println(content);
    
    // Teste 6: Renomear arquivo
    Serial.println("\n6. Renomeando arquivo");
    sd_renameFile("/teste/hello.txt", "/teste/renamed.txt");
    
    // Teste 7: Listar novamente
    Serial.println("\n7. Listando diretório /teste:");
    sd_listDir("/teste", 0);
    
    // Teste 8: Deletar arquivo
    Serial.println("\n8. Deletando arquivo");
    sd_deleteFile("/teste/renamed.txt");
    
    // Teste 9: Remover diretório
    Serial.println("\n9. Removendo diretório");
    sd_removeDir("/teste");
    
    Serial.println("\n=== FIM DOS TESTES ===\n");
}

bool sd_logSensorData(float temperature, double latitude, double longitude, 
                      int satellites, float altitude) {
    if (!sdAvailable) {
        return false;
    }
    
    // Cria o arquivo se não existir, com cabeçalho formatado
    File file = SD.open("/sensor_log.csv");
    if (!file) {
        // Arquivo não existe, cria com cabeçalho usando ponto e vírgula como separador
        sd_writeFile("/sensor_log.csv", 
            "Hora;Milisegundos;Temperatura (C);Latitude;Longitude;Satelites;Altitude (m)\n");
    } else {
        file.close();
    }

    // Prepara a linha de dados com separador ponto e vírgula
    char dataLine[250];
    unsigned long timestamp = millis();

    // Calcula segundos e converte para HH:MM:SS
    unsigned long seconds = timestamp / 1000;
    unsigned long hours = (seconds / 3600) % 24;
    unsigned long minutes = (seconds / 60) % 60;
    unsigned long secs = seconds % 60;
    unsigned long ms = timestamp % 1000;

    snprintf(dataLine, sizeof(dataLine), 
             "%02lu:%02lu:%02lu;%03lu;%.1f;%.6f;%.6f;%d;%.1f\n",
             hours, minutes, secs, ms, temperature, latitude, longitude, satellites, altitude);

    // Adiciona ao arquivo
    return sd_appendFile("/sensor_log.csv", dataLine);
}
