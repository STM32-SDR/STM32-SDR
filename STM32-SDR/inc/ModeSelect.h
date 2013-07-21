/*
 * ModeSelect.h
 * Provide an interface for setting and querying the current mode of the system.
 * - The UserMode is the mode as described to the user (such as USB, LSB, DIG-U, ...)
 * - The Mode is the actual radio mode (SSB, CW, PSK)
 * - The Mode and the Side band are set based on the user mode (see array in ModeSelect.c).
 */

#include <stdint.h>

// Modes that the user knows about and thinks about.
typedef enum {
	// Note: Order must match the s_modeData[] array in ModeSelect.c!
	USERMODE_USB = 0,
	USERMODE_LSB,
	USERMODE_CW,
	USERMODE_CWR,
	USERMODE_DIGU,
	USERMODE_DIGL,

	USERMODE_NUM_MODES	// Automatically set to the count.
} UserModeType;

// Modes that the radio actually operates in.
typedef enum {
	// Start at a "strange" offset to differentiate from other constants.
	MODE_SSB = 123,
	MODE_CW,
	MODE_PSK
} ModeType;

// Side bands
typedef enum {
	SIDEBAND_USB = 321,	// Strange offset to differentiate from other constants.
	SIDEBAND_LSB,
} SideBandType;


void Mode_Init(void);

void Mode_SetCurrentMode(UserModeType newMode);

const char* Mode_GetCurrentUserModeName(void);
const char* Mode_GetCurrentUserModeDescription(void);
UserModeType Mode_GetCurrentUserMode(void);
ModeType Mode_GetCurrentMode(void);
SideBandType Mode_GetCurrentSideBand(void);

const char* Mode_GetUserModeName(UserModeType userMode);
const char* Mode_GetUserModeDescription(UserModeType userMode);


