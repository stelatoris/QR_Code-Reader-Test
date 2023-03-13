
#define SUCCESS                                  \
    {                                            \
        0x02, 0x00, 0x00, 0x01, 0x00, 0x33, 0x31 \
    }
#define QRSCANNER_TRIGGER <QRSCANNER_TRIGGER> // replace with actual trigger pin

// OK response from scanner
byte SUCCESS_RESPONSE[] = SUCCESS;

// serial port mode
byte SERIAL_ADDR[] = {0x00, 0x0D};
byte SERIAL_VALUE = 0xA0; // use serial port for data

// We switch the scanner to continuous mode to initiate scanning and
// to command mode to stop scanning. No external trigger is necessary.
byte SETTINGS_ADDR[] = {0x00, 0x00};

// After basic scanner configuration (9600 bps) uart is set to 115200 bps
// to support fast scanning of animated qrs.
byte BAUD_RATE_ADDR[] = {0x00, 0x2A};
byte BAUD_RATE[] = {0x1A, 0x00}; // 115200

// commands
byte SCAN_ADDR[] = {0x00, 0x02};

// timeout
byte TIMOUT_ADDR[] = {0x00, 0x06};

// After the scanner obtains a scan it waits 100ms and starts a new scan.
byte INTERVAL_OF_SCANNING_ADDR[] = {0x00, 0x05};
byte INTERVAL_OF_SCANNING = 0x01; // 100ms

// DELAY_OF_SAME_BARCODES of 5 seconds means scanning the same barcode again
// (and sending it over uart) can happen only when the interval times out or resets
// which happens if we scan a different qr code.
byte DELAY_OF_SAME_BARCODES_ADDR[] = {0x00, 0x13};
byte DELAY_OF_SAME_BARCODES = 0x85; // 5 seconds

#define RX_PIN 16 // Green
#define TX_PIN 17 // Blue

#define RECOVERY_TIME 30

#define QR_CODE_LENGTH 1024
#define UART_BAUD 9600
#define UART_TXD GPIO_NUM_10
#define UART_RXD GPIO_NUM_9
#define UART_PORT UART_NUM_1

#define TAG "QR_SCANNER"
QueueHandle_t QRCodeQueue;
uint8_t qrCode[QR_CODE_LENGTH];
void QRScannerTask(void *args)
{
    int size = 0;
    uart_flush(UART_PORT);

    while (true)
    {
        size = uart_read_bytes(UART_PORT, qrCode, (QR_CODE_LENGTH - 1), pdMS_TO_TICKS(50));
        if (size)
        {
            qrCode[size] = '\0';
            ESP_LOGI(TAG, "QR code: %s", qrCode);
            xQueueOverwrite(QRCodeQueue, (void *)qrCode);
            uart_flush(UART_PORT);
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void UART_Setup()
{
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        //.source_clk = UART_SCLK_REF_TICK,
        //.use_ref_tick = false,
    };

    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, 2048, 2048, 0, NULL, 0);
}

// bool writeCommand(const char *command, size_t commandSize)
// {
//     return writeCommand(UART_NUM_1, command, commandSize);
// }

bool writeCommand(uart_port_t uart_num, const char *command, size_t commandSize)
{
  uart_write_bytes(uart_num, command, commandSize);

  // Wait for the response from the GM811 QR code reader
  vTaskDelay(pdMS_TO_TICKS(10));

  uint8_t response[32];
  int len = uart_read_bytes(uart_num, response, sizeof(response), pdMS_TO_TICKS(100));
  if (len > 0)
  {
    // Read the response from the GM811 QR code reader
    Serial.print("Response: ");
    for (int i = 0; i < len; i++)
    {
      Serial.printf("0x%02X ", response[i]);
    }
    Serial.println();
    return true;
  }
  else
  {
    Serial.println("No response received.");
    return false;
  }
}
