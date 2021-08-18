#include <Arduino.h>
#define GEAR_RATIO 1.2
// Модели ускорения
  float arrAcc_0[] = {1.00, 1.00};                                                         // Move(ms): 9757 / Calculate(ms):   98 / totalSteps = 7600 / minDelay = 80 / maxDelay = 700 /
  float arrAcc_1[] = {0.00, 10.00, 0.00};                                                  // Move(ms): 4417 / Calculate(ms):  148 / totalSteps = 7600 / minDelay = 80 / maxDelay = 700 /
  float arrAcc_2[] = {0.00, 8.00, 9.50, 10.00, 9.50, 8.00, 0.00};                          // Move(ms): 3197 / Calculate(ms): 1068 / totalSteps = 7600 / minDelay = 80 / maxDelay = 700 /
  float arrAcc_3[] = {4.00, 8.50, 10.00, 9.20, 7.00};                                      // Move(ms): 2706 / Calculate(ms):  499 / totalSteps = 7600 / minDelay = 80 / maxDelay = 700 /
  float arrAcc_4[] = {3.00, 9.40, 9.80, 10.00, 9.60, 8.00, 4.00};                          // Move(ms): 2523 / Calculate(ms): 1132 / totalSteps = 7600 / minDelay = 80 / maxDelay = 700 /
  float arrAcc_5[] = {3.00, 9.80, 10.00, 9.60, 7.00};                                      // Move(ms): 2209 / Calculate(ms):  499 / totalSteps = 7600 / minDelay = 80 / maxDelay = 700 /
  float arrAcc_6[] = {4.00, 9.80, 10.00, 9.80, 8.00};                                      // Move(ms): 1996 / Calculate(ms):  499 / totalSteps = 7600 / minDelay = 80 / maxDelay = 700 /
  float arrAcc_7[] = {4.00, 9.50, 9.80, 10.00, 9.80, 9.50, 8.00};                          // Move(ms): 1898 / Calculate(ms): 1132 / totalSteps = 7600 / minDelay = 80 / maxDelay = 700 /
  float arrAcc_8[] = {4.00, 9.70, 9.80, 9.90, 10.00, 9.90, 9.80, 9.70, 8.00};              // Move(ms): 1713 / Calculate(ms): 2176 / totalSteps = 7600 / minDelay = 80 / maxDelay = 700 /
  float arrAcc_9[] = {4.00, 10.00, 10.00, 10.00, 10.00, 10.00, 10.00, 10.00, 10.00, 8.00}; // Move(ms): 1517 / Calculate(ms): 2883 / totalSteps = 7600 / minDelay = 80 / maxDelay = 700 /
// Переменные
  bool direction = false;
  bool stop = false;
  size_t arrAccSize = 0;
  byte PUL=19;
  byte DIR=18;
  byte ENA=23;
  byte accelerationModel = 0;
  const byte gearsTransmission = 9;
  float arrAccSumm = 0;
  float currentGear = 0;
  float defaultMinDelay = 80;
  float defaultMaxDelay = 700;
  float minDelay = 0;
  float maxDelay = 0;
  float currentDelay = 0;
  float * arrYi;
  float yI = 0;
  int * arrXi;
  int xI = 0;
  int totalSteps = 0;
  short int partSize = 0;
  short int * arrDelays;
// Инструкции
  void InitializationOgVariables(){
    totalSteps = 3800;
    currentGear = 9;
    accelerationModel = 2;
    minDelay = 80;
    maxDelay = 700;
  }
  void PrepareArrow(float arrAcc[]){
    if(minDelay == 0 || maxDelay == 0){
      minDelay = defaultMinDelay;
      maxDelay = defaultMaxDelay;
    }
    arrXi = new int[arrAccSize];
    arrYi = new float[arrAccSize];
    partSize = round(totalSteps / (arrAccSize - 1));
    for(byte i = 0; i < arrAccSize; i++)
    {
      if(i == 0){
        xI = 0;
      }else{
        if(i == (arrAccSize - 1)){
          xI = totalSteps;
        }else{
          xI = xI + partSize;
        }
      }
      arrXi[i] = xI;
      arrYi[i] = arrAcc[i];
      arrAccSumm = arrAccSumm + arrAcc[i];
    }
  }
  void WriteArrayDelays(){
    delete[] arrDelays;
    arrDelays = new short int[totalSteps];
    byte counter = 0;
    byte argProdCounter = 0;
    float arrArgNumer[arrAccSize] = {};
    float arrArgDenom[arrAccSize - 1] = {};
    float arrArgI[arrAccSize] = {};
    float y = 0;
    float numerator = 0;
    float denominator = 0;
    float minGearValue = (arrAccSumm / arrAccSize) / GEAR_RATIO;
    float deltaGear = gearsTransmission - minGearValue;
    float specificGear = deltaGear / gearsTransmission;
    currentGear = minGearValue + (currentGear * specificGear);
    currentDelay = maxDelay - (((maxDelay - minDelay) / gearsTransmission) * currentGear);
    // Обход всех X и запись всех Y в массив
    for(int x = 1; x <= totalSteps; x++)
    {
      counter = 0; // Счетчик перебора Yi-х
      // Чтение всех Yi из массива arrYi
      for(byte i = 0; i < arrAccSize; i++)
      {
        yI = arrYi[counter];
        // Запись массива i-х аргументов
        for(byte i = 0; i < arrAccSize; i++)
        {
          // Запись массива с числителями i-го агрумента
          arrArgNumer[counter] = yI; // Запись Yi-го
          for(byte i = 0; i < arrAccSize; i++) // Запись Хi-ых
          {
            if(i != counter){
              arrArgNumer[i] = x - arrXi[i];
              #ifdef DEBUG1
                Serial.print("Counter:\t\t");
                Serial.println(counter);
                Serial.print("i:\t\t\t");
                Serial.println(i);
                Serial.print("Xi:\t\t\t");
                Serial.println(arrArgNumer[i]);
              #endif
            }
          }
          // Запись массива со знаменателями i-го аргумента
          int denom = 0;
          int counterArrArgDenom = 0;
          for(byte i = 0; i < arrAccSize; i++)
          {
            denom = arrXi[counter] - arrXi[i];
            if(denom != 0){
              arrArgDenom[counterArrArgDenom] = denom;
              counterArrArgDenom++;
            }
          }
          // Подсчет числителя
          numerator = 1;
          for(byte i = 0; i < arrAccSize; i++)
          {
            numerator = numerator * arrArgNumer[i];
          }
          // Подсчет знаминателя
          denominator = 1; 
          for(byte i = 0; i < (arrAccSize - 1); i++)
          {
            denominator = denominator * arrArgDenom[i];
          }
          // Запись i-го аргумента в массив
          arrArgI[counter] = numerator / denominator;
          // Расчет и запись всех Y
          if(argProdCounter == 0){
            y = y + arrArgI[counter];
            if(counter == (arrAccSize - 1)){
              arrDelays[(x-1)] = round(maxDelay -(y * (maxDelay - currentDelay)/(gearsTransmission + 1)));
              y = 0;
            }
          }
          argProdCounter++;
        }
        argProdCounter = 0;
        counter++;
      }
      counter = 0;
    }
    delete[] arrXi;
    delete[] arrYi;
  }
  
  void setup(){
    pinMode (PUL, OUTPUT);
    pinMode (DIR, OUTPUT);
    pinMode (ENA, OUTPUT);

    InitializationOgVariables();

    switch (accelerationModel)
    {
      case 1:
        arrAccSize = sizeof(arrAcc_1) / sizeof(arrAcc_1[0]);
        PrepareArrow(arrAcc_1);
      break;
      case 2:
        arrAccSize = sizeof(arrAcc_2) / sizeof(arrAcc_2[0]);
        PrepareArrow(arrAcc_2);
      break;
      case 3:
        arrAccSize = sizeof(arrAcc_3) / sizeof(arrAcc_3[0]);
        PrepareArrow(arrAcc_3);
      break;
      case 4:
        arrAccSize = sizeof(arrAcc_4) / sizeof(arrAcc_4[0]);
        PrepareArrow(arrAcc_4);
      break;
      case 5:
        arrAccSize = sizeof(arrAcc_5) / sizeof(arrAcc_5[0]);
        PrepareArrow(arrAcc_5);
      break;
      case 6:
        arrAccSize = sizeof(arrAcc_6) / sizeof(arrAcc_6[0]);
        PrepareArrow(arrAcc_6);
      break;
      case 7:
        arrAccSize = sizeof(arrAcc_7) / sizeof(arrAcc_7[0]);
        PrepareArrow(arrAcc_7);
      break;
      case 8:
        arrAccSize = sizeof(arrAcc_8) / sizeof(arrAcc_8[0]);
        PrepareArrow(arrAcc_8);
      break;
      case 9:
        arrAccSize = sizeof(arrAcc_9) / sizeof(arrAcc_9[0]);
        PrepareArrow(arrAcc_9);
      break;
      default:
        arrAccSize = sizeof(arrAcc_0) / sizeof(arrAcc_0[0]);
        PrepareArrow(arrAcc_0);
      break;
    }
    WriteArrayDelays();
    delay(2000);
  }
// Движение
  int Move(bool direction, int totalSteps){
    int x = 0;
    digitalWrite(DIR,direction);
    digitalWrite(ENA,HIGH);
    for(; x <= totalSteps; x++)
    {
      digitalWrite(PUL,HIGH);
      delayMicroseconds(arrDelays[x]);
      digitalWrite(PUL,LOW);
      delayMicroseconds(arrDelays[x]);
      if(stop == true){
        break;
      }
    }
    digitalWrite(ENA,LOW);
    return x;
  }
  void loop(){
    Move(true, totalSteps);
    delay(2000);
    Move(false, totalSteps);
    delay(2000);
  }