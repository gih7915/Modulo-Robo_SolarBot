#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>

// Inicializa o cartão SD
bool sd_init();

// Verifica se o SD está disponível
bool sd_isAvailable();

// Escreve texto em um arquivo (append mode)
bool sd_appendFile(const char* path, const char* message);

// Escreve texto em um arquivo (sobrescreve)
bool sd_writeFile(const char* path, const char* message);

// Lê o conteúdo de um arquivo
String sd_readFile(const char* path);

// Lista arquivos no diretório raiz
void sd_listDir(const char* dirname, uint8_t levels);

// Deleta um arquivo
bool sd_deleteFile(const char* path);

// Renomeia um arquivo
bool sd_renameFile(const char* path1, const char* path2);

// Cria um diretório
bool sd_createDir(const char* path);

// Remove um diretório
bool sd_removeDir(const char* path);

// Obtém informações do cartão SD
void sd_printCardInfo();

// Função de teste completa
void sd_testCardFunctions();

// Salva dados de sensores no formato CSV
bool sd_logSensorData(float temperature, float voltage, double latitude, double longitude, 
                      int satellites, float altitude);

#endif
