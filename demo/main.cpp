
#include "mbed.h"
#include "uLCD_4DGL.h"
#include "mbed_events.h"
#include<iostream>
using namespace std::chrono;
using namespace std;

const double pi = 3.141592653589793238462;
const double amplitude = 0.5f;
const double offset = 56173;//65535;


uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;

AnalogIn Ain(A5);
PwmOut Aout(D6);

DigitalOut led1(LED1);
DigitalOut led2(LED2);
InterruptIn btn(USER_BUTTON);

DigitalIn goUp(A2);
DigitalIn goDn(A1);
DigitalIn Rst(A0);
AnalogOut aout(PA_4);

void print(int fre);
int sample_fre = 500;
float ADCdata[500];

EventQueue eventQueue;
EventQueue printfQueue;

void sampling() {
    while(1) {

        for (int i = 0; i < sample_fre; i++){
            Aout = Ain;
            ADCdata[i] = Ain;
            ThisThread::sleep_for(1000ms/sample_fre);
        }
        for (int i = 0; i < sample_fre; i++){
            cout << ADCdata[i] << "\r\n";
            //printf("%f\r\n", ADCdata[i]);
        }
        ThisThread::sleep_for(5000ms);
    }
}


int main() {
    int state = 0;

    uLCD.background_color(WHITE);
    uLCD.cls();
    uLCD.textbackground_color(WHITE);
    uLCD.text_width(3);
    print(state);

    double rads = 0.0;
    uint16_t Digit_signal = 0;
    int count = 0;
    int periodArray[4] = {10, 20, 35, 50};


    eventQueue.call(&sampling);
    Thread eventThread(osPriorityNormal);
    eventThread.start(callback(&eventQueue, &EventQueue::dispatch_forever));

    //float Digit_signal;
    //int count = 0;
    while(1) {
        if(Rst) {
            count = 0;
            state = 0;
            print(1000/periodArray[state]);
            ThisThread::sleep_for(1ms);
        } else if(goUp) {
            if (state < 3) state++;  else state = state;
            print(1000/periodArray[state]);
            ThisThread::sleep_for(1ms);
        } else if (goDn) {
            if (state > 0) state--;  else state = state;
            print(1000/periodArray[state]);
            ThisThread::sleep_for(1ms);
        } else {
            for (int i = 0; i < periodArray[state]; i++) {
                if (i < periodArray[state]*0.7)
                    Digit_signal = (uint16_t)((offset * i / (periodArray[state]*0.7)) + 0);
                else
                    Digit_signal = (uint16_t)(-(offset * (i-periodArray[state]*0.7) / (periodArray[state]*0.3)) + offset);
                aout.write_u16(Digit_signal);
                /*
                count++;
                if(count >= 128){
                    for(int i = 0 ; i < count ; i++)
                    {
                        cout<<ADCdata[i]<<"\r\n";
                    }
                    count = 0;
                }           
                */     
                ThisThread::sleep_for(1ms);
            }
        }
    }

}

void print(int fre) {
    uLCD.cls();
    uLCD.textbackground_color(WHITE);
    uLCD.printf("\n100Hz, 50Hz, 28Hz, 20Hz\n");
    uLCD.text_width(3);
    uLCD.locate(2, 5);
    uLCD.printf("\n%dHz\n", fre);
}


