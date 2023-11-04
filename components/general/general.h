/*
 * general.h
 *
 *  Created on: Feb 4, 2023
 *      Author: Jaimit
 */

#ifndef INC_GENERAL_H_
#define INC_GENERAL_H_

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
/************************************************************************
 * Define Macros
 ************************************************************************/
#define PACKED  __attribute__((packed))
#define PUBLIC  extern
#define PRIVATE static
/*
 * Macro to check the outputs of TWDT functions and trigger an abort if an
 * incorrect code is returned.
 */
#define CHECK_ERROR_CODE(returned, expected) ({                        \
            if(returned != expected){                                  \
                abort();                                               \
            }                                                          \
})


/**
* Set single bit at pos to '1' by generating a mask
* in the proper bit location and ORing (|) x with the mask.
*/
#define BIT_SET(x, pos) (x |= (1U << pos))
/**
* Set single bit at pos to '0' by generating a mask
* in the proper bit location and Anding x with the mask.
*/
#define BIT_CLEAR(x, pos) (x &= (~(1U<< pos)))
/**
* Set single bit at pos to '1' by generating a mask
* in the proper bit location and ex-ORing x with the mask.
*/
#define BIT_TOGGLE(x, pos) x ^= (1U<< pos)
/**
* Set single bit at pos to '1' by generating a mask
* in the proper bit location and Anding x with the mask.
* It evaluates 1 if a bit is set otherwise 0.
*/
#define BIT_CHECK(x, pos) (x & (1UL << pos) )
/**
* Macro to swap nibbles
*/
#define SWAP_NIBBLES(x) ((x & 0x0F)<<4 | (x & 0xF0)>>4)
/**
* Macro to swap byte of 32-bit +ve integer variable
*/
#define SWAP_BYTES(u32Value) ((u32Value & 0x000000FF) << 24)\
                                |((u32Value & 0x0000FF00) << 8) \
                                |((u32Value & 0x00FF0000) >> 8) \
                                |((u32Value & 0xFF000000) >> 24)


//*****************************************************************************************************
//Error Codes
//*****************************************************************************************************
/* Definitions for error constants. */
#define NRF_OK          0       /*!< nrf_err value indicating success (no error) */
#define NRF_FAIL       -1      /*!< Generic app_err_t code indicating failure */

#define NRF_ERR_NO_MEM              0x101   /*!< Out of memory */
#define NRF_ERR_INVALID_ARG         0x102   /*!< Invalid argument */
#define NRF_ERR_INVALID_STATE       0x103   /*!< Invalid state */
#define NRF_ERR_INVALID_SIZE        0x104   /*!< Invalid size */
#define NRF_ERR_NOT_FOUND           0x105   /*!< Requested resource not found */
#define NRF_ERR_NOT_SUPPORTED       0x106   /*!< Operation or feature not supported */
#define NRF_ERR_TIMEOUT             0x107   /*!< Operation timed out */
#define NRF_ERR_INVALID_RESPONSE    0x108   /*!< Received response was invalid */
#define NRF_ERR_INVALID_CRC         0x109   /*!< CRC or checksum was invalid */
#define NRF_ERR_INVALID_VERSION     0x10A   /*!< Version was invalid */
#define NRF_ERR_INVALID_MAC         0x10B   /*!< MAC address was invalid */
#define NRF_ERR_NOT_FINISHED        0x10C   /*!< There are items remained to retrieve */


#define NRF_ERR_WIFI_BASE           0x3000  /*!< Starting number of WiFi error codes */
#define NRF_ERR_MESH_BASE           0x4000  /*!< Starting number of MESH error codes */
#define NRF_ERR_FLASH_BASE          0x6000  /*!< Starting number of flash error codes */
#define NRF_ERR_HW_CRYPTO_BASE      0xc000  /*!< Starting number of HW cryptography module error codes */
#define NRF_ERR_MEMPROT_BASE        0xd000  /*!< Starting number of Memory Protection API error codes */
#define NRF_ERR_NVS_NOT_FOUND       0xe000
//***********************************************************************************************************


#define MAX_NUM_CONNECTOR 1


/*Charger Type*/
#define BHARAT_AC001 1
#define TYPE2_AC 2
#define NONE 0xFF



#define FIXED_POINT_BIT_RESOLUTION_METER_CHANNEL (22U)
#define FIXED_POINT_BIT_RESOLUTION_ADC_CHANNEL (14U)
#define fixed_point_t int32_t

typedef enum {
	OCPP_MODE = 1,			/**< Interface Type WIFI*/
	MOBILE_APP_MODE,				/**< Interface Type GSM*/
    PLUG_N_CHARGE_MODE,				/**< Interface Type GSM*/
    RFID_MODE,				/**< Interface Type GSM*/
}__attribute__((packed)) charger_mode_t;
/************************************************************************
 * Define Private Variables
 ************************************************************************/

/************************************************************************
 * Define Global Variables
 ************************************************************************/
typedef int app_err_t;
/************************************************************************
 * Define Private Function Prototypes
 ************************************************************************/

/************************************************************************
 * Define Global Function Prototypes
 ************************************************************************/

/**
 * @brief: This api will decide connector type.
 *
 * @param[in] arg
 *
 * @param[out] void
 */
void decide_ConnectorType(void);
/**
 * @brief: This api will decide hardware type.
 *
 * @param[in] arg
 *
 * @param[out] void
 */
void decide_HardwareVersion(void);
/**
 * @brief: This api will return connector number
 *
 * @param[in] connector number
 *
 * @param[out] uint8_t connector type
 */
uint8_t get_ConnectorType(uint8_t conn);

fixed_point_t float_to_fixed(uint8_t ui8BitResolution, double input);

charger_mode_t get_charger_mode(void);
#endif /* INC_GENERAL_H_ */
