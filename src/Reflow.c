#include <pic.h> //必須ヘッダの取り込み

#pragma config BOREN=OFF //ブラウンアウトリセット無効
#pragma config CPD=OFF //プログラムのプロテクト無効
#pragma config DEBUG=OFF //デバッグモード無効
#pragma config WRT=OFF //書き込み制限無効
#pragma config FOSC=INTOSCIO //内部クロック生成器使用
#pragma config MCLRE=OFF //内部リセット回路有効
#pragma config WDTE=OFF //ウォッチドッグタイマ無効
#pragma config CP=OFF //プログラムのプロテクト無効
#pragma config LVP=OFF //低電圧プログラミング無効
#pragma config PWRTE=OFF //パワーアップタイマ無効

#define HEATER RA6 //15番ピンをヒーターに使用
#define FAN RA4 //3番ピンをファンに使用
#define BUZZER RA2 //1番ピンをブザーに使用
#define PRE_HEAT RB0 //6番ピンを予熱ランプに使用
#define PRE_TO_REFLOW RB3 //9番ピンをモード移行ランプに使用
#define REFLOW RB1 //7番ピンを本加熱ランプに使用

#define PRE_L 1015 //100度100-110(http://www.analog.com/static/imported-files/jp/data_sheets/AD594_AD595_jp.pdf)
#define PRE_H 1117 //110度125-137
#define REF_L 1718 //170度175-182
#define REF_H 1817 //180度192-195
#define PRE_T 2136 //70秒(秒/0.032768)
#define REF_T 915 //30秒


#define _XTAL_FREQ 8000000

unsigned short count;
unsigned short mvlt;

void interrupt entry(void) { // 割り込み処理 (32.768ms毎に1回)
    TMR0IF = 0;
    count++;
}

void main(void) {
    //タイマ・動作クロック設定
    OSCCON = 0b01110000; //動作クロックを最高(8MHz)に設定
    OPTION_REG = 0b10000111; //プリスケーラは256

    //入出力設定
    TRISA = 0b10101011; //ポートAの入出力方向の設定 順番は7～0
    TRISB = 0b11110100; //ポートBの入出力方向の設定 0=出力 1=入力
    PORTA = 0b00000000; //ポートAの出力の設定 順番は7～0
    PORTB = 0b00000000; //ポートBの出力の設定 0=OFF 1=ON

    //AD変換設定
    ANSEL = 0b00001010; //ADの選択 0=デジタル 1=アナログ
    ADCS2 = 1;
    ADCS1 = 0;
    ADCS0 = 1;
    ADFM = 1;
    VCFG1 = 1;
    VCFG0 = 0;
    CHS2 = 0;
    CHS1 = 0;
    CHS0 = 1;

    //出力・変数設定
    PRE_HEAT = 0;
    PRE_TO_REFLOW = 0;
    REFLOW = 0;
    HEATER = 1;
    FAN = 0;
    BUZZER = 0;
    count = 0;

    //割り込み設定
    TMR0 = 0;
    GIE = 1;
    TMR0IE = 0;

    do {
        ADON = 1;
        __delay_us(30);
        GO_DONE = 1; //AD変換
        while (GO_DONE);
        mvlt = ((ADRESH << 8) | ADRESL) << 2;

        if (PRE_HEAT) { // 予熱中
            if (count > PRE_T) {
                PRE_HEAT = 0;
                PRE_TO_REFLOW = 1;
                TMR0IE = 0;
                count = 0;
                HEATER = 1;
            } else if (mvlt > PRE_H) {
                HEATER = 0;
            } else if (mvlt < PRE_L) {
                HEATER = 1;
            }
        } else if (REFLOW) { // リフロー中
            if (count > REF_T) {
                REFLOW = 0;
                TMR0IE = 0;
                count = 0;
                HEATER = 0;
                FAN = 1;
                BUZZER = 1;
                break;
            } else if (mvlt > REF_H) {
                HEATER = 0;
            } else if (mvlt < REF_L) {
                HEATER = 1;
            }
        } else if (PRE_TO_REFLOW) { //移行中
            if (mvlt > REF_L) {
                PRE_TO_REFLOW = 0;
                REFLOW = 1;
                TMR0IE = 1;
            }
        } else { //加熱中
            if (mvlt > PRE_L) {
                PRE_HEAT = 1;
                TMR0IE = 1;
            }
        }
        ADON = 0;
        __delay_us(30);
    } while (1);
    while(1);
}