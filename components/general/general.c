/*
 * utils.c
 *
 *  Created on: Mar 5, 2023
 *      Author: Jaimit
 */

/***************************************************************************
 * INCLUDES
 ***************************************************************************/
#include "general.h"
#include "math.h"
/***************************************************************************
 * MACROS
 ****************************************************************************/

/***************************************************************************
 * TYPES
 ***************************************************************************/

/***************************************************************************
 * GLOBAL VARIABLES
 ***************************************************************************/

/***************************************************************************
 * PRIVATE FUNCTION DEFINATION
 ***************************************************************************/

/***************************************************************************
 * FUNCTIONS
 ***************************************************************************/
#if 0
void decide_ConnectorType(void) {
	DeviceInfo_t Devinfo = {0U};

	if (HIGH == digitalRead(MCU_BRD_ID1)) {
		Devinfo.connectorType[0] = TYPE2_AC;
	} else {
		Devinfo.connectorType[0] = BHARAT_AC001;
	}
	if (HIGH == digitalRead(MCU_BRD_ID2)) {
		Devinfo.connectorType[1] = TYPE2_AC;
	} else {
		Devinfo.connectorType[1] = BHARAT_AC001;
	}
	if (HIGH == digitalRead(MCU_BRD_ID3)) {
		Devinfo.connectorType[2] = TYPE2_AC;
	} else {
		Devinfo.connectorType[2] = BHARAT_AC001;
	}
	UpdateDeviceInfo(&Devinfo);
}

void decide_HardwareVersion(void) {
	//FIXME :Need to ask hw person for this
	DeviceInfo_t Devinfo = {0U};

	if (HIGH == digitalRead(POW_BRD_ID1)) {
		Devinfo.boardVersion[0] = TYPE2_AC;
	} else {
		Devinfo.boardVersion[0] = BHARAT_AC001;
	}
	if (HIGH == digitalRead(POW_BRD_ID2)) {
		Devinfo.boardVersion[1] = TYPE2_AC;
	} else {
		Devinfo.boardVersion[1] = BHARAT_AC001;
	}
	if (HIGH == digitalRead(POW_BRD_ID3)) {
		Devinfo.boardVersion[2] = TYPE2_AC;
	} else {
		Devinfo.boardVersion[2] = BHARAT_AC001;
	}
	UpdateDeviceInfo(&Devinfo);
}
uint8_t get_ConnectorType(uint8_t conn)
{
	DeviceInfo_t Devinfo = {0U};
	GetDeviceInfo(&Devinfo);
	return Devinfo.connectorType[conn - 1];
}
#endif
fixed_point_t float_to_fixed(uint8_t ui8BitResolution, double input)
{
	return (fixed_point_t)(round(input * (1 << ui8BitResolution)));
}
charger_mode_t get_charger_mode(void)
{
	charger_mode_t mode = PLUG_N_CHARGE_MODE;
	if (true)
	{
		mode = MOBILE_APP_MODE;
	}
	else if (true)
	{
		mode = PLUG_N_CHARGE_MODE;
	}
	else if (true)
	{
		mode = RFID_MODE;
	}
	else if (true)
	{
		mode = OCPP_MODE;
	}
	return mode;
}
