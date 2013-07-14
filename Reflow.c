#include <pic.h> //�K�{�w�b�_�̎�荞��

#pragma config BOREN=OFF //�u���E���A�E�g���Z�b�g����
#pragma config CPD=OFF //�v���O�����̃v���e�N�g����
#pragma config DEBUG=OFF //�f�o�b�O���[�h����
#pragma config WRT=OFF //�������ݐ�������
#pragma config FOSC=INTOSCIO //�����N���b�N������g�p
#pragma config MCLRE=OFF //�������Z�b�g��H�L��
#pragma config WDTE=OFF //�E�H�b�`�h�b�O�^�C�}����
#pragma config CP=OFF //�v���O�����̃v���e�N�g����
#pragma config LVP=OFF //��d���v���O���~���O����
#pragma config PWRTE=OFF //�p���[�A�b�v�^�C�}����

#define HEATER RA6 //15�ԃs�����q�[�^�[�Ɏg�p
#define FAN RA4 //3�ԃs�����t�@���Ɏg�p
#define BUZZER RA2 //1�ԃs�����u�U�[�Ɏg�p
#define PRE_HEAT RB0 //6�ԃs����\�M�����v�Ɏg�p
#define PRE_TO_REFLOW RB3 //9�ԃs�������[�h�ڍs�����v�Ɏg�p
#define REFLOW RB1 //7�ԃs����{���M�����v�Ɏg�p

#define PRE_L 1015 //100�x100-110(http://www.analog.com/static/imported-files/jp/data_sheets/AD594_AD595_jp.pdf)
#define PRE_H 1117 //110�x125-137
#define REF_L 1718 //170�x175-182
#define REF_H 1817 //180�x192-195
#define PRE_T 2136 //70�b(�b/0.032768)
#define REF_T 915 //30�b


#define _XTAL_FREQ 8000000

unsigned short count;
unsigned short mvlt;

void interrupt entry(void) { // ���荞�ݏ��� (32.768ms����1��)
    TMR0IF = 0;
    count++;
}

void main(void) {
    //�^�C�}�E����N���b�N�ݒ�
    OSCCON = 0b01110000; //����N���b�N���ō�(8MHz)�ɐݒ�
    OPTION_REG = 0b10000111; //�v���X�P�[����256

    //���o�͐ݒ�
    TRISA = 0b10101011; //�|�[�gA�̓��o�͕����̐ݒ� ���Ԃ�7�`0
    TRISB = 0b11110100; //�|�[�gB�̓��o�͕����̐ݒ� 0=�o�� 1=����
    PORTA = 0b00000000; //�|�[�gA�̏o�͂̐ݒ� ���Ԃ�7�`0
    PORTB = 0b00000000; //�|�[�gB�̏o�͂̐ݒ� 0=OFF 1=ON

    //AD�ϊ��ݒ�
    ANSEL = 0b00001010; //AD�̑I�� 0=�f�W�^�� 1=�A�i���O
    ADCS2 = 1;
    ADCS1 = 0;
    ADCS0 = 1;
    ADFM = 1;
    VCFG1 = 1;
    VCFG0 = 0;
    CHS2 = 0;
    CHS1 = 0;
    CHS0 = 1;

    //�o�́E�ϐ��ݒ�
    PRE_HEAT = 0;
    PRE_TO_REFLOW = 0;
    REFLOW = 0;
    HEATER = 1;
    FAN = 0;
    BUZZER = 0;
    count = 0;

    //���荞�ݐݒ�
    TMR0 = 0;
    GIE = 1;
    TMR0IE = 0;

    do {
        ADON = 1;
        __delay_us(30);
        GO_DONE = 1; //AD�ϊ�
        while (GO_DONE);
        mvlt = ((ADRESH << 8) | ADRESL) << 2;

        if (PRE_HEAT) { // �\�M��
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
        } else if (REFLOW) { // ���t���[��
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
        } else if (PRE_TO_REFLOW) { //�ڍs��
            if (mvlt > REF_L) {
                PRE_TO_REFLOW = 0;
                REFLOW = 1;
                TMR0IE = 1;
            }
        } else { //���M��
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