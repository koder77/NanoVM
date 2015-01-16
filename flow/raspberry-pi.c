#include <global_d.h>


#if RPI_GPIO

#include <wiringPi.h>

S2 rpi_gpio_start (void)
{
	S2 ret;
	
	if (wiringPiSetup () == -1)
	{
		return (1);		/* ERROR!! */
	}
	else
	{
		return (0);		/* OK */
	}
}


void rpi_gpio_mode (S2 pin, S2 mode)
{
	pinMode (pin, mode);
}

void rpi_gpio_write (S2 pin, S2 value)
{
	digitalWrite (pin, value);
}

S2 rpi_gpio_read (S2 pin)
{
	S2 ret;
	
	ret = digitalRead (pin);
	return (ret);
}

#else

S2 rpi_gpio_start (void)
{	
	printf ("Raspberry Pi GPIO support not available in this build!\n");
	return (2);
}


void rpi_gpio_mode (S2 pin, S2 mode)
{

}

void rpi_gpio_write (S2 pin, S2 value)
{

}

S2 rpi_gpio_read (S2 pin)
{
	return (0);
}

#endif
