
// Query
_Bool KeyboardStatus_IsUSBDeviceAttached(void);
_Bool KeyboardStatus_IsKeyboardWorking(void);
_Bool KeyboardStatus_isKeyboardDisconnected(void);

// Setting state (from USB callbacks)
void KeyboardStatus_SetDeviceDesconnected(void);
void KeyboardStatus_SetDeviceAttached(void);
void KeyboardStatus_SetKeyboardWorking(void);
