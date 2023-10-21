/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define IS_RGBW false
#define NUM_PIXELS 256 
// NUM_PIXELS は パネルのドット数

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 2
#endif

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}
  
void pattern_EDP(uint len, uint t) {
    //8*4の四角
    uint8_t print_char[] = {0b00000001,0b00000001,0b00000001,0xFF};

    const uint8_t print_char_len = sizeof(print_char) / sizeof(print_char[0]);
    printf("print_char_len:%d",print_char_len);
    const uint panel_row = 32;
    const uint panel_column = 8;
    const uint8_t red = 0xFF;
    const uint8_t green = 0xFF;
    const uint8_t blue = 0; 
    printf("print_char_len:%d",print_char_len);

    // framebuffer init
    uint8_t framebuffer[panel_column][panel_row][3];
    for (uint i=0; i<panel_column; i++){
        for(uint j=0; j<panel_row; j++){
            for(uint k=0; k<3; k++){
                framebuffer[i][j][k] = 0;
                //printf("framebuffer[%d][%d][%d]:%d",i,j,k,framebuffer[i][j][k]);
            }
        }
    }

    // set chardata in framebuffer
    for(uint i=0; i<print_char_len; i++){
        bool char_bit[8];
        const uint8_t char_bit_len = sizeof(char_bit) / sizeof(char_bit[0]);
        //printf("char_bit_len:%d",char_bit_len);
        uint8_t char_tmp = print_char[i];
        for(uint bit=0; bit<panel_column; bit++){
            char_bit[bit] = (char_tmp >> bit) & 1;
            //printf("char_bit[%d]:%d",bit ,char_bit[bit]);
        }
        /*
        uint8_t roop_cnt = 0;
        while(char_tmp <= 1){
            char_bit[8 - roop_cnt] = char_tmp % 2;
            char_tmp /= 2;
            roop_cnt += 1;
        }
        */
        for(uint j=0; j<char_bit_len; j++){
            if(char_bit[j] == 0){
                // set no color
                for(uint k=0; k<3; k++){
                    framebuffer[j][i][k] = 0;
                }
            }else if(char_bit[j] == 1){
                // set RGB color
                framebuffer[j][i][0] = red;
                framebuffer[j][i][1] = green;
                framebuffer[j][i][2] = blue;
            }
        } 
    }

    for(uint i=0; i<panel_column; i++){
        for(uint j=0; j<panel_row; j++){
            printf("framebuffer[%d][%d]:%d,%d,%d\n",i,j,framebuffer[i][j][0],framebuffer[i][j][1],framebuffer[i][j][2]);
        }
    }

    //put_pixel(urgb_u32(framebuffer[0][0][0],framebuffer[0][0][1],framebuffer[0][0][2]));
/*
    // printing pre framebuffer cache clear
    for(uint i=0; i<panel_column*panel_row; i++){
            put_pixel(urgb_u32(0,0,0));
    }
    */
   for(uint i; i<8; i++){
        for(uint j; j<32; j++){
            for(uint k; k<3; k++){
                printf("framebuffer[%d][%d][%d]:%d",i,j,k,framebuffer[i][j][k]);
            }
        }
   }
   put_pixel(urgb_u32(framebuffer[0][0][0],framebuffer[0][0][1],framebuffer[0][0][2]));
   put_pixel(urgb_u32(framebuffer[1][0][0],framebuffer[1][1][1],framebuffer[1][0][2]));
/*
    // print framebuffer    
    for(uint i=0; i<panel_row; i++){
        if (i % 2 == 0){
            for(uint j=0; j<panel_column; j++){
                put_pixel(urgb_u32(framebuffer[j][i][0], framebuffer[j][i][1], framebuffer[j][i][2]));
            }
        }
        if (i % 2 == 1){   
            for(uint j=panel_column-1; j>=0; j--){     
            //for(uint j=panel_column-1; j>0; j--){  
                put_pixel(urgb_u32(framebuffer[j][i][0], framebuffer[j][i][1], framebuffer[j][i][2]));    
            }
        }
    }
    sleep_ms(1000);
    *?
    
}

void pattern_snakes(uint len, uint t) { 
    for (uint i = 0; i < len; ++i) {
        uint x = (i + (t >> 1)) % 64;
        if (x < 10)
            put_pixel(urgb_u32(0xff, 0, 0));
        else if (x >= 15 && x < 25)
            put_pixel(urgb_u32(0, 0xff, 0));
        else if (x >= 30 && x < 40)
            put_pixel(urgb_u32(0, 0, 0xff));
        else
            put_pixel(0);
    }
}

void pattern_random(uint len, uint t) {
    if (t % 8)
        return;
    for (int i = 0; i < len; ++i)
        put_pixel(rand());
}

void pattern_sparkle(uint len, uint t) {
    if (t % 8)
        return;
    for (int i = 0; i < len; ++i)
        put_pixel(rand() % 16 ? 0 : 0xffffffff);
}

void pattern_greys(uint len, uint t) {
    int max = 100; // let's not draw too much current!
    t %= max;
    for (int i = 0; i < len; ++i) {
        put_pixel(t * 0x10101);
        if (++t >= max) t = 0;
    }
}

typedef void (*pattern)(uint len, uint t);
const struct {
    pattern pat;
    const char *name;
} pattern_table[] = {
        {pattern_EDP,     "EDP"}/*,
        {pattern_snakes,  "Snakes!"},
        {pattern_random,  "Random data"},
        {pattern_sparkle, "Sparkles"},
        {pattern_greys,   "Greys"}, 
        */   
};

int main() {
    //set_sys_clock_48();
    stdio_init_all();
    printf("WS2812 Smoke Test, using pin %d", WS2812_PIN);

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    int t = 0;
    while(1){
            pattern_EDP(0,0);
    }

    /*
    while (1) {
        int pat = rand() % count_of(pattern_table);
        int dir = (rand() >> 30) & 1 ? 1 : -1;
        puts(pattern_table[pat].name);
        puts(dir == 1 ? "(forward)" : "(backward)");
        for (int i = 0; i < 1000; ++i) {
            pattern_table[pat].pat(NUM_PIXELS, t);
            sleep_ms(10);
            t += dir;
        }
    }
    */
}
