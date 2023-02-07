#include <iostream>
#include <map>
#include <string>

#include "LCD_Display.h"

int main(int argc, char** argv)
{
	LCD_Display* d = new LCD_Display(23,24,5,6,13,19);
	std::string end;
	printf("If you want to exit enter e \n");
	do{
		std::cin >> end;
		d->send_raw(end);
	}
	while (end != "e");
	d->clear_display();
	delete d;
	
}