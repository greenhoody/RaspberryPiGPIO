#include "LCD_Display.h"
#include <pigpio.h>
#include <thread>
#include <iostream>

LCD_Display::~LCD_Display()
{
    gpioTerminate();
}

LCD_Display::LCD_Display(int rs, int e, int data4, int data5, int data6, int data7)
{
    rs_pin = rs;
    e_pin = e;
    data_pins_.reserve(4);
    
    data_pins_[0] = data4;
    data_pins_[1] = data5;
    data_pins_[2] = data6;
    data_pins_[3] = data7;
    
    if(gpioInitialise()<0)
    {
        printf("Failed to initialize GPIO. /n");
        return;
    }
    gpioSetMode(rs, PI_OUTPUT);
    gpioSetMode(e, PI_OUTPUT);
    gpioSetMode(data4, PI_OUTPUT);
    gpioSetMode(data5, PI_OUTPUT);
    gpioSetMode(data6, PI_OUTPUT);
    gpioSetMode(data7, PI_OUTPUT);
    initialise_display();
}

void LCD_Display::send_chr(uint8_t data)
{
    gpioWrite(rs_pin, 1);
    send(data);
}

void LCD_Display::send_cmd(uint8_t cmd)
{
    gpioWrite(rs_pin, 0);
    send(cmd);
}

void LCD_Display::clear_display()
{
    send_cmd(0x01);
    //this commend need longer time to execute
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

void LCD_Display::return_home()
{
    send_cmd(0x02);
    //this commend need longer time to execute
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

void LCD_Display::shift_display(int movement)
{
    while(abs(movement) > 0)
    {
        if(movement > 0)
        {
            send_cmd(0x1c);
            movement --;
        }
        else
        {
            send_cmd(0x18);
            movement ++;
        }
    }
}

void LCD_Display::shift_cursor(int movement)
{
    while(abs(movement) > 0)
    {
        if(movement > 0)
        {
            send_cmd(0x14);
            movement --;
        }
        else
        {
            send_cmd(0x10);
            movement ++;
        }
    }
}

void LCD_Display::set_function(bool data_lenght, bool no_lines, bool font)
{
    data_lenght_ = data_lenght;
    no_lines_ = no_lines;
    font_ = font;
    
    uint8_t cmd = 0x20;
    
    if(data_lenght_)
        cmd = cmd | 0x10;
    if(no_lines_)
        cmd = cmd | 0x08;
    if(font_)
        cmd = cmd | 0x04;
    send_cmd(cmd);
}

void LCD_Display::set_dis_add(uint8_t add)
{
    send_cmd(add | 0x80);
}

void LCD_Display::set_cgram_add(uint8_t add)
{
    send_cmd((add & 0x7f)|0x40);
}

void LCD_Display::set_display_controll(bool display, bool cursor, bool blinking)
{
    display_on_off_ = display;
    cursor_on_off_ = cursor;
    blinking_on_off_ = blinking;
    uint8_t cmd = 0x08;

    if(display_on_off_)
        cmd = cmd | 0x04;
    if(cursor_on_off_)
        cmd = cmd | 0x02;
    if(blinking_on_off_)
        cmd = cmd | 0x01;
    send_cmd(cmd);
}

void LCD_Display::set_blinking(bool blinking)
{
    blinking_on_off_ = blinking;
    uint8_t cmd = 0x08;
    if(display_on_off_)
        cmd = cmd | 0x04;
    if(cursor_on_off_)
        cmd = cmd | 0x02;
    if(blinking_on_off_)
        cmd = cmd | 0x01;
    send_cmd(cmd);
}

void LCD_Display::set_cursor(bool cursor)
{
    cursor_on_off_ = cursor;
    uint8_t cmd = 0x08;
    if(display_on_off_)
        cmd = cmd | 0x04;
    if(cursor_on_off_)
        cmd = cmd | 0x02;
    if(blinking_on_off_)
        cmd = cmd | 0x01;
    send_cmd(cmd);
}

void LCD_Display::set_display(bool display)
{
    display_on_off_ = display;
    uint8_t cmd = 0x08;
    if(display_on_off_)
        cmd = cmd | 0x04;
    if(cursor_on_off_)
        cmd = cmd | 0x02;
    if(blinking_on_off_)
        cmd = cmd | 0x01;
    send_cmd(cmd);
}

void LCD_Display::set_entry_mode(bool cursor_direction, bool shift)
{
    cursor_direction_ = cursor_direction;
    shift_ = shift;
    uint8_t cmd = 0x04;
    if(cursor_direction_)
        cmd = cmd | 0x02;
    if(shift_)
        cmd = cmd | 0x01;
    send_cmd(cmd);
}

void LCD_Display::reset()
{
    send_half(0x03);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    send_half(0x03);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    send_half(0x03);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    send_half(0x02);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void LCD_Display::send_half(uint8_t data)
{
    gpioWrite(e_pin,1);

    //just in case if gpioWrite second argument cannot be diffrent then 0 and 1l

    uint8_t result = 10;
    for (int i = 0; i < 4; i++)
    {
        std::cerr << "Pin: " << data_pins_[i] << "State: " <<  (data & 0x01) << "\n";
        result = data & 0x01;
        gpioWrite(data_pins_[i], result);
        data = data >> 1;
    }
    gpioWrite(e_pin,0);
}

void LCD_Display::set_rs(bool state)
{
    if(state)
    {
        gpioWrite(rs_pin,1);
    }
    else
    {
        gpioWrite(rs_pin,0);
    }
}

void LCD_Display::send(uint8_t data)
{
    send_half(data>>4);
    //std::this_thread::sleep_for(std::chrono::microseconds(60));
    send_half(data);
    std::this_thread::sleep_for(std::chrono::microseconds(200));
}

void LCD_Display::send_raw(std::string raw)
{
    if(raw[0] == 'h')
    {
        set_rs(true);
        send_half(dictionary[raw[1]]);
        send_half(dictionary[raw[2]]);
    }
    else if(raw[0] == 'm')
    {
        set_rs(false);
        send_half(dictionary[raw[1]]);
        send_half(dictionary[raw[2]]);
    }
}

void LCD_Display::initialise_display()
{
    gpioWrite(rs_pin,0);
    //using figure 24 in documentation
    //setting 4 bit mode
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    reset();
    send_cmd(0x28);
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    send_cmd(0x0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    send_cmd(0x01);
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    send_cmd(0x06);
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

void LCD_Display::send_string(std::string mes)
{
    for( std::string::iterator it=mes.begin(); it!=mes.end(); ++it)
    {
        send(*it);
    }
}