// Local variables to the file
static _Bool s_isDeviceAttached = 0;
static _Bool s_isKeyboardWorking = 0;


// Query
_Bool KeyboardStatus_IsUSBDeviceAttached(void)
{
	return s_isDeviceAttached;
}
_Bool KeyboardStatus_IsKeyboardWorking(void)
{
	return s_isKeyboardWorking;
}
_Bool KeyboardStatus_IsKeyboarddisconnected(void)
{
	return !s_isDeviceAttached ;
}


// Operation
void KeyboardStatus_SetDeviceDesconnected(void)
{
	s_isDeviceAttached = 0;
	s_isKeyboardWorking = 0;
}
void KeyboardStatus_SetDeviceAttached(void)
{
	s_isDeviceAttached = 1;
}
void KeyboardStatus_SetKeyboardWorking(void)
{
	s_isKeyboardWorking = 1;
}
