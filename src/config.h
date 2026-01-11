#ifndef CONFIG_H
#define CONFIG_H

// Ativar simulação de sensores (1 = simular, 0 = hardware real)
#define SIMULATE_SENSORS 0

// 0 = modo Access Point; 1 = conectar no roteador (STA)
#define WIFI_USE_STA 1

// Credenciais para STA (quando WIFI_USE_STA = 1)
#define WIFI_STA_SSID "Dianna-Giovana-Leo"
#define WIFI_STA_PASS "Giovana18"

// Credenciais para Access Point (quando WIFI_USE_STA = 0 ou fallback)
#define WIFI_AP_SSID  "gih"
#define WIFI_AP_PASS  "12345678"
// Canal do AP (1, 6 ou 11 são mais estáveis)
#define WIFI_AP_CHANNEL 1

// ===== Configuração do Cartão SD (SPI) =====
// Pinos padrão VSPI do ESP32:
#define SD_MISO  19  // Master In Slave Out
#define SD_MOSI  23  // Master Out Slave In
#define SD_SCK   18  // Clock
#define SD_CS     5  // Chip Select (pode ser alterado conforme hardware)

// Nota: Se você estiver usando um módulo SD específico,
// verifique os pinos corretos no seu hardware.
// Alguns módulos podem usar pinos diferentes.

#endif
