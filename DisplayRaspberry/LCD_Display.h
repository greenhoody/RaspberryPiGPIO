#pragma once
#include <map>
#include <vector>
#include <stdint.h>
#include <string>


//LCD display 2x16 complied with HD44780 for raspberry pi
//for now only 4 bits comunication  
class LCD_Display
{
public:
    ~LCD_Display();
    LCD_Display(int, int, int, int, int, int);
    void send_chr(uint8_t data);
    void send_cmd(uint8_t cmd);
    void send_half(uint8_t data);

    void send_string(std::string mes);

    //string must have 3 characters no spaces.
    //first one says if it is command or character to write
    //m - command, h - character
    //next two are hexadecimal numbers
    void send_raw(std::string raw);

    //clears display memory and return cursor to first place in ddram
    void clear_display();
    void reset();
    //moves cursor and display shift to start position
    void return_home();
    void shift_display(int movement);
    void shift_cursor(int movement);

    /* sets parameters about display
    data_lenght 1 - 8 bits(not supported yet) / 0 - 4 - bits
    no lines 1 - 2 lines / 0 - 1 line
    font 1 - 5x10 dots/ 0 - 5x8 dots*/
    void set_function(bool data_lenght,bool no_lines,bool font);
    
    void set_dis_add(uint8_t add);
    void set_cgram_add(uint8_t add);
    //options fo display setup, turining on/off display, cursor, cursor blinking 1-on 0-off
    void set_display_controll(bool display, bool cursor, bool blinking);
    //turn on/off cursor blinking
    void set_blinking(bool blinking);
    //turn on/off cursor
    void set_cursor(bool cursor);
    //turn on/off display
    void set_display(bool display);
    // 0-decrement cursor position after writing char, 1 increase, shift 1 - display shifts after writing character 
    void set_entry_mode(bool cursor_direction, bool shift);
    
    
private:
    // variables to keep truck of used options in set up display
    std::map<char,uint8_t> dictionary = {
        {'0',0},{'1',1},{'2',2},{'3',3},{'4',4},{'5',5},{'6',6},{'7',7},
        {'8',8},{'9',9},{'a',10},{'b',11},{'c',12},{'d',13},{'e',14},{'f',15}};
        
    bool display_on_off_ = true;
    bool cursor_on_off_ = true;
    bool blinking_on_off_ = true;
    
    // 0-decrement cursor position after writing char, 1 increase
    bool cursor_direction_ = true;
    // shift 1 - display shifts after writing character;  
    bool shift_;
    
    // data lenght of comunication with controller, 1 - 8 bits / 0 - 4 bits
    bool data_lenght_ = false;
    // number of line sin display 1 - 2 lines / 0 - 1 line
    bool no_lines_ = true;
    // font 1 - 5x10 dots/ 0 - 5x8 dots
    bool font_ = false;
    //data pins, only four bit transfer supported
    std::vector<int> data_pins_;

    void initialise_display();
    void send(uint8_t data);

    void set_rs(bool state);
    
    //pin to choose between data and and instruction register
    //0 & rw 0 - instruction, 0 & rw 1 - busy flag
    //1 - data
    int rs_pin;
    //pin to state that state of on datalines are ready and they can be red. Writing data hapenns on falling edge 
    int e_pin;
    //TO-DO only writing supported for the time, pin connected to ground
    //0 - write
    //1 - read
    //int rw_pin = -1;
    
};
