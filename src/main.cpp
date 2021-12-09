#include <queue>

#include "app.h"
#include "argument.h"
#include <FrameTimer.h>


const int SENSOR_HZ = 20;
const int LED_PIN   = 10;


struct SoundParam
{
    int freq;
    int duration;
};

const int SOUND_TIMER_ID = 1;
//std::vector<SoundParam> soundQueue(32, {.freq=0, .duration=0});
std::queue<SoundParam> soundQueue;

hw_timer_t * timer = NULL;
hw_timer_t* soundTimer     = nullptr;
portMUX_TYPE soundTimerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onSoundInerrupt();
void IRAM_ATTR onTimer()
{
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}



void popSound()
{
    if(soundQueue.empty())
    {
        return;
    }

    SoundParam param;
    
    portENTER_CRITICAL_ISR(&soundTimerMux);
    {
        param = soundQueue.front();
        soundQueue.pop();
    }
    portEXIT_CRITICAL_ISR(&soundTimerMux);

    if(param.freq == 0)
    {
        M5.Beep.mute();
    }else{
        M5.Beep.tone(param.freq);
    }

    soundTimer = timerBegin(1, 80, true);
    timerAttachInterrupt(soundTimer, &onSoundInerrupt, true);
    timerAlarmWrite(soundTimer, param.duration * 1000, false);
    
    timerAlarmEnable(soundTimer);
}

void stopSound()
{
    M5.Beep.mute();

    portENTER_CRITICAL_ISR(&soundTimerMux);
    {
        while(!soundQueue.empty())
        {
            soundQueue.pop();
        }
    }
    portEXIT_CRITICAL_ISR(&soundTimerMux);
}

/**
 * @brief 
 * 
 */
void playSound()
{
    if(soundQueue.empty())
    {
        return;
    }

    SoundParam param;
    
    portENTER_CRITICAL_ISR(&soundTimerMux);
    {
        param = soundQueue.front();
        soundQueue.pop();
    }
    portEXIT_CRITICAL_ISR(&soundTimerMux);

    if(param.freq == 0)
    {
        M5.Beep.mute();
    }else{
        M5.Beep.tone(param.freq);
    }

    timerWrite(soundTimer, 0);      // 経過値を0に
    timerAlarmWrite(soundTimer, param.duration * 1000, false);  // 発火設定
    timerAlarmEnable(soundTimer);   // Timer開始
}

/**
 * @brief 
 * 
 */
void IRAM_ATTR onSoundInerrupt()
{
    if(soundQueue.empty())
    {
        M5.Beep.mute();
    }else{
        playSound();
    }
}

AppArgument arg;

typedef void (*FpTimerTask)(bool);
FrameTimer_g<FpTimerTask> sensorTimer;
FrameTimer_g<FpTimerTask> drawTimer;

/**
 * @brief センサー処理
 * 
 * @param isEv 
 */
void SensorFunction(bool isEv)
{
    if(!isEv)
    {
        return;
    }
}



/**
 * @brief 描画
 * 
 * @param isEv 
 */
void DrawFunction(bool isEv)
{
    if(!isEv)
    {
        return;
    }
    
    if(arg.prev_state != arg.state)
    {
        M5.Lcd.fillScreen(BLACK);
    }
    
    auto* p_lcd = &M5.Lcd;
    p_lcd->setTextColor(GREEN, BLACK);
    p_lcd->setTextDatum(4);
    
    // put your main code here, to run repeatedly:
    if(arg.state == State::CARIBRATION)
    {
        auto sz = String("calibration");
        p_lcd->drawString(sz, p_lcd->width() / 2, p_lcd->height() / 2);
    }else
    if(arg.state == State::READY)
    {
        auto sz = String("scr-mode: ") + arg.screenMode;
        p_lcd->drawString(sz, p_lcd->width() / 2, 20);

        char szAngle[16];
        sprintf(szAngle, "angle:%+4d", (int)(arg.bankAngle));
        p_lcd->drawString(szAngle, p_lcd->width() / 2, 40);
        //p_lcd->drawString("Freq: "+SENSOR_HZ, p_lcd->width() / 2, 50);
    }

    arg.prev_state = arg.state;
}

/**
 * @brief 
 * 
 */
void initSound(int timerId)
{
    //ASSERT(timerId>=0 && timerId<=3, "ERROR !!! invalid timer id.\n");

    M5.Beep.setVolume(10);
    soundTimer = timerBegin(timerId, 80, true);
    timerAttachInterrupt(soundTimer, &onSoundInerrupt, true);
}

/**
 * @brief 
 * 
 */
void setup() 
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    M5.begin();

    M5.Axp.ScreenBreath(9);
    M5.IMU.Init();

    pinMode(LED_PIN, OUTPUT);
    
    //sensorTimer.SetFPS(SENSOR_HZ);
    //sensorTimer.SetFunction(&SensorFunction);

    drawTimer.SetFPS(20);
    drawTimer.SetFunction(&DrawFunction);

    M5.Lcd.setRotation(arg.screenMode);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(GREEN, BLACK);    

    arg.state = State::CARIBRATION;
    DrawFunction(true);
    
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 1000000, true);
    timerAlarmEnable(timer);

    /// Sound関係初期化
    initSound(SOUND_TIMER_ID);
    
    arg.state = State::READY;
}

/**
 * @brief 
 * 
 */
void loop() 
{
    M5.update();
    sensorTimer.Update();
    drawTimer.Update();

    if(arg.state == State::READY)
    {
        if(M5.BtnA.wasPressed())
        {
            arg.screenMode = (arg.screenMode + 1) % 4;
            M5.Lcd.setRotation(arg.screenMode);
            M5.Lcd.fillScreen(BLACK);

            // M5.Beep.tone(3300); // 329.628
            // delay(40);
            // M5.Beep.mute();

            // M5.Beep.tone(2600); // 261.626
            // delay(60);
            // M5.Beep.mute();

            // M5.Beep.tone(3300); // 329.628
            // delay(40);
            // M5.Beep.mute();

            // M5.Beep.tone(2600); // 261.626
            // delay(60);
            // M5.Beep.mute();

            stopSound();
            // soundQueue.push({.freq=3300, .duration=40});
            // soundQueue.push({.freq=2600, .duration=60});
            // soundQueue.push({.freq=3300, .duration=40});
            // soundQueue.push({.freq=2600, .duration=60});
            // soundQueue.push({.freq=3300, .duration=40});
            soundQueue.push({.freq=1800, .duration=40});
            soundQueue.push({.freq=1200, .duration=60});
            soundQueue.push({.freq=1800, .duration=40});
            soundQueue.push({.freq=1200, .duration=60});
            playSound();
        }else
        if(M5.BtnB.wasPressed())
        {
            soundQueue.push({.freq=660, .duration=60});
            soundQueue.push({.freq=550, .duration=40});

            soundQueue.push({.freq=0  , .duration=500});
            
            soundQueue.push({.freq=660, .duration=400});
            soundQueue.push({.freq=440, .duration=800});
            
            soundQueue.push({.freq=0  , .duration=500});

            soundQueue.push({.freq=1200, .duration=100});
            soundQueue.push({.freq=1200, .duration=100});
            soundQueue.push({.freq=1200, .duration=100});

            soundQueue.push({.freq=0  , .duration=500});

            soundQueue.push({.freq=1200, .duration=500});
            soundQueue.push({.freq=1800, .duration=500});
            soundQueue.push({.freq=2400, .duration=500});

            playSound();
        }
    }
}
