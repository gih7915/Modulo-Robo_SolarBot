# Modulo-Robo_SolarBot

Projeto embarcado com ESP32 para aquisicao de dados de sensores, dashboard web local e registro em SD/LittleFS.

## Visao geral

Este firmware coleta dados dos sensores:

- `BMP280` (temperatura, pressao, altitude)
- `GPS` (latitude, longitude, satelites, hora UTC)
- `MPU6050` (acelerometro, giroscopio, temperatura interna)
- `INA226` (tensao com ajuste por divisor resistivo)
- `ACS758` (corrente via ADC)

E publica/armazena em:

- API HTTP (JSON)
- Arquivos no `LittleFS` para historico e exportacao
- Arquivo CSV no cartao SD

## Hardware alvo

- Placa: `DOIT ESP32 DEVKIT V1`
- Framework: `Arduino` (PlatformIO)

## Sensores e pinagem

### I2C (GPIO 21/22)

- `SDA`: GPIO `21`
- `SCL`: GPIO `22`
- Sensores no barramento:
	- BMP280 (`0x76`)
	- INA226 (`0x40`)
	- MPU6050 (`0x68`)

### GPS (UART2)

- `RX ESP32`: GPIO `16`
- `TX ESP32`: GPIO `17`
- Baud rate: `9600`

### ACS758 (analogico)

- Pino ADC: GPIO `34`
- Resolucao ADC: `12 bits`
- Atenuacao: `ADC_11db`

### Cartao SD (SPI)

- `MISO`: GPIO `19`
- `MOSI`: GPIO `23`
- `SCK`: GPIO `18`
- `CS`: GPIO `5`

## Estrutura do projeto

```text
src/
	main.cpp
	config.h
	sensors/
		bmp280.*
		gps.*
		mpu6050.*
		ina226.*
		acs758.*
		ds18b20.*   (em pausa)
	network/
		web_server.*
	storage/
		sd_card.*
data/
	index.html
	app.js
	styles.css
platformio.ini
```

## Configuracao

As configuracoes principais ficam em `src/config.h`:

- Wi-Fi STA/AP (`WIFI_USE_STA`, SSID e senha)
- Calibracao do INA226 (`INA226_BUS_DIVIDER`)
- Calibracao do ACS758:
	- `ACS758_ZERO_CURRENT_V`
	- `ACS758_SENSITIVITY_V_PER_A`
	- `ACS758_NOISE_THRESHOLD_A`
	- `ACS758_SAMPLES`

Importante:

- Ajuste SSID/senha antes de publicar firmware.
- O ACS758 pode exigir recalibracao no hardware real para maior estabilidade.

## Como compilar e gravar

### Requisitos

- VS Code
- Extensao PlatformIO

### Passos

1. Abrir a pasta do projeto no VS Code.
2. Conferir `src/config.h`.
3. Compilar:

```bash
pio run
```

4. Gravar firmware:

```bash
pio run -t upload
```

5. Monitor serial:

```bash
pio device monitor -b 115200
```

## Rede e dashboard

O firmware pode operar em:

- `AP+STA` (`WIFI_USE_STA = 1`): tenta conectar no roteador e mantem AP ativo.
- `AP` (`WIFI_USE_STA = 0`): apenas ponto de acesso local.

No boot, o serial imprime os IPs para acesso ao dashboard.

## Endpoints HTTP

- `GET /api/temperature`
	- temperatura, pressao, altitude, tensao, corrente, timestamp
- `GET /api/gps`
	- lat, lng, fix, satelites, utc, timestamp
- `GET /api/mpu6050`
	- aceleracao, giro e temperatura do MPU6050
- `GET /api/all`
	- agrega dados de todos os sensores
- `GET /api/robot/stats`
	- alias para `api/all`
- `GET /export?format=csv`
	- exporta logs em CSV (LittleFS)
- `GET /export?format=json`
	- exporta logs em JSON (LittleFS)

## Logs

### LittleFS

- Arquivo: `/logs.csv`
- Colunas:
	- `timestamp,temperature,pressure,lat,lng,satellites,fix,voltage,current`

### SD

- Arquivo: `/sensor_log.csv`
- Colunas:
	- `Hora;Milisegundos;Temperatura (C);Tensao (V);Corrente (A);Latitude;Longitude;Satelites;Altitude (m)`

## Status atual

- Fluxos de simulacao de sensores removidos.
- DS18B20 mantido no projeto, mas atualmente em pausa.

## Melhorias sugeridas

- Trocar `DynamicJsonDocument` por `JsonDocument` (ArduinoJson v7) para remover warnings.
- Criar rotina de calibracao guiada para ACS758.
- Persistir configuracao Wi-Fi/offset dos sensores em arquivo de configuracao.
