/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


/*
２桁のhexをbinに変換する関数hexToBin
1. 1つのhexをbinに変換
2. 戻り値で１つのbinを返す

16進数の各桁を１０進数に変換　strtolで
それを２で割って

char型をint型にキャストするとchar型の文字に割り当てられてる文字コードの数字に変換される。
    -> ex. 'A' だったら、'65'

1. hexをcharにキャスト
2. charの配列の要素を指定して、hexの各桁のみを取り出す。
3. 取り出したhexの値が、0...9...A...F なら dexの 0...9...10...15 に変換する。
    1. 取り出した各桁を int型 にキャスト
        2.(0-9) '0' = 48
        2.(A-F) 'A' = 65
    2. キャストした値から48を引く
    3. 2.の値が9より大きいなら
        65-48-7 を引く


１０進数を２で割った余りを使う方法
１０進数を４bitの２進数の各桁が示す１０進数の値の内大きい順から割っていく方法
*/


char hexToBin(uint32_t hex){
    uint8_t hexlen = sizeo((char)hex) / sizeof((char)hex[0]);
    printf('hexlen:'%d,hexlen);
    for( i=0; i < hexlen; i++ ){
        char bin = strtol((char)hex, NULL, 16);
    }

    printf('binnum:'%d,bin);
    return bin;
}


static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

void framebuffer() {
    for(uint i=0; i<128; i++){
        put_pixel(urgb_u32(0x00, 0x00, 0x10));
    }
}
  
void pattern_EDP(uint len, uint t) {
    //文字データ：新潟大学コンピュータクラブ  -- EDP --
    uint8_t print_char[] = { 0xf2, 0x00,0x01,0x02,0x03, 0xf0, 0xa8,0xe8,0xc9,0xda,0xee,0xb4,0xa4,0xde,0xcb, 0xf1, 0x00,0x00,0x0d,0x0d,0x00,0x25,0x24,0x30,0x00,0x0d,0x0d, };
    for (uint i = 0; i < len; ++i) {
        put_pixel(urgb_u32(0, 0, 0x20));
    }
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
    //    {pattern_EDP,     "EDP"},
        //{framebuffer,    "test"}
        {pattern_snakes,  "Snakes!"},
        {pattern_random,  "Random data"},
        {pattern_sparkle, "Sparkles"},
        {pattern_greys,   "Greys"},    
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
//    while (1) {
        int pat = rand() % count_of(pattern_table);
        int dir = (rand() >> 30) & 1 ? 1 : -1;
        puts(pattern_table[pat].name);
        puts(dir == 1 ? "(forward)" : "(backward)");
        for (int i = 0; i < 1000; ++i) {
            pattern_table[pat].pat(NUM_PIXELS, t);
            sleep_ms(10);
            t += dir;
        }
//    }
}
