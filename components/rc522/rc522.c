/************************************************************************
 * Include Header Files
 ************************************************************************/
#include <zephyr/kernel.h>
#include "zephyr/logging/log.h"
#include "components/gpio_config/gpio_config.h"
#include <zephyr/drivers/spi.h>

#include "components/rc522/rc522.h"
#include "components/general/general.h"
#include "components/gpio_config/gpio_config.h"

#include <zephyr/drivers/spi.h>
/************************************************************************
 * Define Macros
 ************************************************************************/
#define BUFFER_SIZE 7U
#define READ_BUFFER_SIZE 4U
#define BUFF_SIZE 3U
#define SPI_TIMEOUT 100U
#define WRITE_OPERATION 0x00
#define READ_OPERATION 0x80

#define BYTE_1 8
#define BYTE_2 16
#define BYTE_3 24
#define BYTE_4 32

#define TIMEOUT 100U

#define TAG "internal_meter"

#define METER_TASK_NAME "meter task"
#define METER_TASK_STACK_SIZE (4 * 1024) ///< Define Task Stack Size
#define METER_TASK_PRIORITY 9
#define TASK_DELAY_MS 100U
#define METERING_DELAY 50

const struct device *spi_dev;

static const struct spi_config spi_cfg = {
    .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB |
                 SPI_MODE_CPHA,
    .frequency = 400000,
    .slave = 0,
    //.cs = &spim_cs,
};
#define MY_SPI_MASTER DT_NODELABEL(meter_spi)

void spi_init(void)
{
    spi_dev = DEVICE_DT_GET(MY_SPI_MASTER);
    if (!device_is_ready(spi_dev))
    {
        printk("SPI master device not ready!\n");
    }
    // if(!device_is_ready(spim_cs.gpio.port)){
    // 	printk("SPI master chip select device not ready!\n");
    // }
}

void ClearBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    tmp = rc522_read(reg, (uint8_t *)&tmp, 1);
    rc522_write(reg, tmp & (~mask), 1); // clear bit mask
}

void SetBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    tmp = rc522_read(reg, (uint8_t *)&tmp, 1);

    rc522_write(reg, tmp | mask, 1); // set bit mask
}
void AntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}
void AntennaOn(void)
{
    uint8_t u8TxControlReg = 0;
    rc522_read(TxControlReg, (uint8_t *)&u8TxControlReg, 1);
    //	if (!(temp & 0x03))
    //	{
    //		SetBitMask(TxControlReg, 0x03);
    //	}
    SetBitMask(TxControlReg, 0x03);
}

void MFRC522_Reset(void)
{
    rc522_write(CommandReg, PCD_RESETPHASE, 1);
}
void MFRC522_Init(void)
{

    // spi config
    // MFRC522_SPI_Init();

    MFRC522_Reset();

    // Timer: TPrescaler*TreloadVal/6.78MHz = 24ms

    rc522_write(TModeReg, 0x8D, 1);
    rc522_write(TPrescalerReg, 0x3E, 1);
    rc522_write(TReloadRegL, 30, 1);
    rc522_write(TReloadRegH, 0, 1); // auto=1; f(Timer) = 6.78MHz/TPreScaler

    rc522_write(TxAutoReg, 0x40, 1);
    rc522_write(ModeReg, 0x3D, 1);

    AntennaOn(); // Mo Anten
}
app_err_t rc522_write(uint8_t ui8Address, uint8_t ui8val, uint8_t connector)
{
    app_err_t err = NRF_FAIL;
    err = spi8bit_write(ui8Address, ui8val, connector);
    return err;
}

app_err_t rc522_read(uint8_t ui8Address, uint8_t *ui32val, uint8_t connector)
{
    app_err_t err = NRF_FAIL;
    err = spi8bit_read(ui8Address, *ui32val, connector);
    return err;
}
app_err_t spi8bit_read(uint8_t ui8Address, uint8_t *ui32val, uint8_t connector)
{
    // This is the algorithm that reads from a 32-bit register in the ADE7953.
    // The argument is the 8-bit address of the register.
    app_err_t err = NRF_OK;
    uint8_t ui32ReadBuffer = 0x00U;
    uint8_t ui8Buffer[READ_BUFFER_SIZE] = {0X00U};

    ui8Buffer[0] = ui8Address;
    ui8Buffer[1] = READ_OPERATION;

    static uint8_t rx_buffer[2] = {0};

    const struct spi_buf tx_buf = {
        .buf = ui8Buffer,
        .len = sizeof(ui8Buffer)};

    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1};

    struct spi_buf rx_buf = {
        .buf = rx_buffer,
        .len = READ_BUFFER_SIZE,
    };
    const struct spi_buf_set rx = {
        .buffers = &rx_buf,
        .count = 1};

    if (1U == connector)
    {
        GPIO_OnOff(OP_SPI1_CS1, LOW);
    }
    else if (2U == connector)
    {
        // Handle other connectors as needed.
    }
    else if (3U == connector)
    {
        // Handle other connectors as needed.
    }

    k_msleep(METERING_DELAY);

    if (NRF_OK != spi_write(spi_dev, &spi_cfg, &tx))
    {
        printk("spiAlgorithm8_read spi write failed\n");
        err = NRF_FAIL;
    }
    if (NRF_OK != spi_read(spi_dev, &spi_cfg, &rx))
    {
        printk("spiAlgorithm8_read spi read failed\n");
        err = NRF_FAIL;
    }
    printk("spiAlgorithm8_read rx_buffer[0]: %d rx_buffer[1]: %d rx_buffer[3]: %d rx_buffer[4]: %d\n", rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3]);
    ui32ReadBuffer = rx_buffer[0] | (rx_buffer[1] << 8) | (rx_buffer[2] << 16) | (rx_buffer[3] << 24);

    if (1U == connector)
    {
        GPIO_OnOff(OP_SPI1_CS1, HIGH);
    }
    else if (2U == connector)
    {
        // Handle other connectors as needed.
    }
    else if (3U == connector)
    {
        // Handle other connectors as needed.
    }

    *ui32val = (ui32ReadBuffer & 0xFF); // Mask to extract the 8-bit value.
    return err;
}
app_err_t spi8bit_write(uint8_t ui8Address, uint8_t ui8val, uint8_t connector)
{
    // This is the algorithm that writes to a register in the ADE7953.
    app_err_t err = NRF_OK;
    uint8_t ui8Buffer[BUFFER_SIZE] = {0x00};

    ui8Buffer[0] = ui8Address; // 8-bit address
    ui8Buffer[1] = WRITE_OPERATION;
    ui8Buffer[2] = ui8val; // 8-bit value to be written

    if (1U == connector)
    {
        GPIO_OnOff(OP_SPI1_CS1, LOW);
    }
    else if (2U == connector)
    {
        // Handle other connectors as needed.
    }
    else if (3U == connector)
    {
        // Handle other connectors as needed.
    }

    k_msleep(METERING_DELAY);

    const struct spi_buf tx_buf = {
        .buf = ui8Buffer,
        .len = BUFFER_SIZE};

    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1};

    if (NRF_OK != spi_write(spi_dev, &spi_cfg, &tx))
    {
        printk("spiAlgorithm8_write spi write failed\n");
        err = NRF_FAIL;
    }

    if (1U == connector)
    {
        GPIO_OnOff(OP_SPI1_CS1, HIGH);
    }
    else if (2U == connector)
    {
        // Handle other connectors as needed.
    }
    else if (3U == connector)
    {
        // Handle other connectors as needed.
    }

    k_msleep(METERING_DELAY);

    return err;
}

uint8_t MFRC522_ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint *backLen)
{
    uint8_t status = MI_ERR;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint8_t data = 0x00;
    uint i;

    switch (command)
    {
    case PCD_AUTHENT: // Acknowledging the liver
    {
        irqEn = 0x12;
        waitIRq = 0x10;
        break;
    }
    case PCD_TRANSCEIVE: // FIFO data collection
    {
        irqEn = 0x77;
        waitIRq = 0x30;
        break;
    }
    default:
        break;
    }

    rc522_write(CommIEnReg, irqEn | 0x80, 1); // Yeu cau ngat
    ClearBitMask(CommIrqReg, 0x80);           // Clear all the bits
    SetBitMask(FIFOLevelReg, 0x80);           // FlushBuffer=1, FIFO

    rc522_write(CommandReg, PCD_IDLE, 1); // NO action; Huy bo lenh hien hanh	???

    // Record in FIFO
    for (i = 0; i < sendLen; i++)
    {
        rc522_write(FIFODataReg, sendData[i], 1);
    }

    // chay
    rc522_write(CommandReg, command, 1);
    if (command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg, 0x80); // StartSend=1,transmission of data starts
    }

    // The team is allowed to be stored
    i = 2000; // i tuy thuoc tan so thach anh, thoi gian toi da cho the M1 la 25ms
    do
    {
        // CommIrqReg[7..0]
        // Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = rc522_read(CommIrqReg, (uint8_t *)&n, 1);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

    ClearBitMask(BitFramingReg, 0x80); // StartSend=0

    if (i != 0)
    {
        data = rc522_read(ErrorReg, (uint8_t *)&data, 1);
        if (!(data & 0x1B)) // BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR; //??
            }

            if (command == PCD_TRANSCEIVE)
            {
                n = rc522_read(FIFOLevelReg, (uint8_t *)&n, 1);
                lastBits = rc522_read(ControlReg, (uint8_t *)&lastBits, 1) & 0x07;
                if (lastBits)
                {
                    *backLen = (n - 1) * 8 + lastBits;
                }
                else
                {
                    *backLen = n * 8;
                }

                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAX_LEN)
                {
                    n = MAX_LEN;
                }

                // FIFO doc in the received data
                for (i = 0; i < n; i++)
                {
                    backData[i] = rc522_read(FIFODataReg, (uint8_t *)&n, 1);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }
    }

    // SetBitMask(ControlReg,0x80);           //timer stops
    // Write_MFRC522(CommandReg, PCD_IDLE);

    return status;
}