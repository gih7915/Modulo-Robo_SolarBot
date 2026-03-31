#ifndef CONFIG_H
#define CONFIG_H

// Fator do divisor resistivo do INA226: V_real = V_medida * INA226_BUS_DIVIDER
#define INA226_BUS_DIVIDER 3.0f

// Configuracao do ACS758 (corrente por leitura analogica)
#define ACS758_PIN 34
#define ACS758_VREF 3.3f
#define ACS758_ADC_MAX 4095.0f
#define ACS758_SAMPLES 100
#define ACS758_ZERO_CURRENT_V 1.65f
#define ACS758_SENSITIVITY_V_PER_A 0.0264f
#define ACS758_NOISE_THRESHOLD_A 0.15f

// 0 = modo Access Point; 1 = conectar no roteador (STA)
#define WIFI_USE_STA 1

// Credenciais para STA (quando WIFI_USE_STA = 1)
#define WIFI_STA_SSID "AP-ACCESS SS"
#define WIFI_STA_PASS "Fin@ApointBlH"


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


#endif
