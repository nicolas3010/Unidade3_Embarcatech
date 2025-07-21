#include "aht10.h"

void aht10_i2c_init(){
    i2c_init(i2c0, 400 * 1000); // 400kHz
    gpio_set_function(AHT10_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(AHT10_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(AHT10_I2C_SDA_PIN);
    gpio_pull_up(AHT10_I2C_SCL_PIN);
}

void aht10_init() {
    // Esta linha prepara o comando de inicialização para o AHT10
    uint8_t cmd[] = {0xBE, 0x08, 0x00};
    i2c_write_blocking(AHT10_I2C_PORT, AHT10_ADDR, cmd, 3, false);
    sleep_ms(50); // aumente o tempo de espera para garantir a inicialização
}

void aht10_trigger_measurement() {
    // Esta linha prepara o comando para iniciar uma medição no AHT10
    uint8_t cmd[] = {0xAC, 0x33, 0x00};
    i2c_write_blocking(AHT10_I2C_PORT, AHT10_ADDR, cmd, 3, false);
    sleep_ms(80); // tempo de espera da medição
}

bool aht10_read(float *temperature, float *humidity) {
    // Esta linha lê 6 bytes de dados do sensor AHT10 via I2C e armazena no array 'data'
    uint8_t data[6];
    int res = i2c_read_blocking(AHT10_I2C_PORT, AHT10_ADDR, data, 6, false);
    if (res != 6) return false;

    // Checa se o sensor ainda está ocupado (bit 7 do primeiro byte)
    if (data[0] & 0x80) {
        // Sensor ainda está processando, leitura inválida
        return false;
    }

    uint32_t hum_raw = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | ((data[3] >> 4) & 0x0F);
    uint32_t temp_raw = (((uint32_t)data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];

    // Cálculo conforme datasheet do AHT10
    *humidity = ((float)hum_raw / 1048576.0f) * 100.0f;
    *temperature = ((float)temp_raw / 1048576.0f) * 200.0f - 50.0f;
    return true;
}