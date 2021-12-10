#include <queue>
#include <vector>

#include "app.h"
#include "argument.h"
#include <FrameTimer.h>

#include "doremi.h"

const int SENSOR_HZ = 20;
const int LED_PIN   = 10;


struct SoundParam
{
    int freq;
    int duration;
};

const int SOUND_TIMER_ID = 1;
//std::vector<SoundParam> soundQueue(32, {.freq=0, .duration=0});
//std::queue<SoundParam> soundQueue;

#define	TEMPO		(120)
#define	NOTE4MS		(60*1000/TEMPO)
#define	NOTE2MS		(NOTE4MS*2)
#define	NOTE1MS		(NOTE2MS*2)
#define	NOTE8MS		(NOTE4MS/2)
#define	NOTE16MS	(NOTE8MS/2)
#define	NOTE32MS	(NOTE16MS/2)
#define	NOTE64MS	(NOTE32MS/2)
#define	NOTE128MS	(NOTE64MS/2)
#define	NOTE2PMS	(NOTE2MS+NOTE4MS)
#define	NOTE4PMS	(NOTE4MS+NOTE8MS)
#define	NOTE8PMS	(NOTE8MS+NOTE16MS)
#define	NOTE16PMS	(NOTE16MS+NOTE32MS)
#define	NOTE32PMS	(NOTE32MS+NOTE64MS)


#define	MAKE_NOTE(f,d)	{.freq=(f) , .duration=(d)}

/**
 * @brief サウンドリスト
 * 
 */
std::vector<std::vector<SoundParam>> soundList = {
    {MAKE_NOTE(B__6, 40 ), MAKE_NOTE(D__6, 60 ), MAKE_NOTE(B__6, 40 ), MAKE_NOTE(D__6, 60 )},
    {MAKE_NOTE(1200, 30 ), MAKE_NOTE(900 , 30 ), MAKE_NOTE(600 , 30 ), MAKE_NOTE(300 , 30 ), MAKE_NOTE(150 , 30 )},
    {MAKE_NOTE(750 , 30 ), MAKE_NOTE(900 , 30 ), MAKE_NOTE(1200, 30 ), MAKE_NOTE(1800, 30 )},
    {MAKE_NOTE(660 , 60 ), MAKE_NOTE(550 , 40 )},
    {MAKE_NOTE(660 , 400), MAKE_NOTE(440 , 800)},
    {MAKE_NOTE(1200, 100), MAKE_NOTE(1200, 100), MAKE_NOTE(1200, 100)},
    {MAKE_NOTE(1200, 500), MAKE_NOTE(1800, 500), MAKE_NOTE(2400, 500)},
	
	{MAKE_NOTE(F__4, NOTE64MS), MAKE_NOTE(F__6, NOTE64MS),
	 MAKE_NOTE(E__4, NOTE64MS), MAKE_NOTE(E__6, NOTE64MS),
	 MAKE_NOTE(DS_4, NOTE64MS), MAKE_NOTE(DS_6, NOTE64MS),
	 MAKE_NOTE(D__4, NOTE64MS), MAKE_NOTE(D__6, NOTE64MS),
	 MAKE_NOTE(F__4, NOTE64MS), MAKE_NOTE(F__6, NOTE64MS),
	 MAKE_NOTE(E__4, NOTE64MS), MAKE_NOTE(E__6, NOTE64MS),
	 MAKE_NOTE(DS_4, NOTE64MS), MAKE_NOTE(DS_6, NOTE64MS),
	 MAKE_NOTE(D__4, NOTE64MS), MAKE_NOTE(D__6, NOTE64MS),},

	{MAKE_NOTE(FS_4, NOTE64MS), MAKE_NOTE(FS_6, NOTE64MS),
	 MAKE_NOTE(G__4, NOTE64MS), MAKE_NOTE(G__6, NOTE64MS),
	 MAKE_NOTE(GS_4, NOTE64MS), MAKE_NOTE(GS_6, NOTE64MS),
	 MAKE_NOTE(A__4, NOTE64MS), MAKE_NOTE(A__6, NOTE64MS),
	 MAKE_NOTE(FS_4, NOTE64MS), MAKE_NOTE(FS_6, NOTE64MS),
	 MAKE_NOTE(G__4, NOTE64MS), MAKE_NOTE(G__6, NOTE64MS),
	 MAKE_NOTE(GS_4, NOTE64MS), MAKE_NOTE(GS_6, NOTE64MS),
	 MAKE_NOTE(A__4, NOTE64MS), MAKE_NOTE(A__6, NOTE64MS),},
	// {MAKE_NOTE(F__4, NOTE64MS), MAKE_NOTE(F__6, NOTE64MS),MAKE_NOTE(F__4, NOTE64MS), MAKE_NOTE(F__6, NOTE64MS),
	//  MAKE_NOTE(E__4, NOTE64MS), MAKE_NOTE(E__6, NOTE64MS),MAKE_NOTE(E__4, NOTE64MS), MAKE_NOTE(E__6, NOTE64MS),
	//  MAKE_NOTE(DS_4, NOTE64MS), MAKE_NOTE(DS_6, NOTE64MS),MAKE_NOTE(DS_4, NOTE64MS), MAKE_NOTE(DS_6, NOTE64MS),
	//  MAKE_NOTE(D__4, NOTE64MS), MAKE_NOTE(D__6, NOTE64MS),MAKE_NOTE(D__4, NOTE64MS), MAKE_NOTE(D__6, NOTE64MS),},

	/// LvUP!
	{MAKE_NOTE(F__6, NOTE16MS), MAKE_NOTE(F__6, NOTE16MS), MAKE_NOTE(F__6, NOTE16MS),
	 MAKE_NOTE(F__6, NOTE16MS), MAKE_NOTE(REST, NOTE16MS),
	 MAKE_NOTE(DS_6, NOTE16MS), MAKE_NOTE(REST, NOTE16MS), MAKE_NOTE(G__6, NOTE16MS), MAKE_NOTE(REST, NOTE16MS),
	 MAKE_NOTE(F__6, NOTE4MS),},

	/// FF.
	{MAKE_NOTE(B__6, NOTE16PMS), MAKE_NOTE(G__6, NOTE16PMS ), MAKE_NOTE(A__6, NOTE16PMS ),
	 MAKE_NOTE(B__6, NOTE32MS), MAKE_NOTE(REST, NOTE32MS), MAKE_NOTE(A__6, NOTE32MS), MAKE_NOTE(B__6, NOTE8MS)},
	
	/// doremi.
	{MAKE_NOTE(C__6, NOTE4MS ), MAKE_NOTE(C__6, NOTE8MS ), MAKE_NOTE(D__6, NOTE8MS ),
	 MAKE_NOTE(E__6, NOTE4MS ), MAKE_NOTE(E__6, NOTE8MS ), MAKE_NOTE(C__6, NOTE8MS ),
	 MAKE_NOTE(E__6, NOTE4MS ), MAKE_NOTE(C__6, NOTE4MS ), MAKE_NOTE(E__6, NOTE2MS ),},

	/// Fuuga
	{MAKE_NOTE(D__5, NOTE16MS), MAKE_NOTE(A__5, NOTE16MS), MAKE_NOTE(F__5, NOTE16MS), MAKE_NOTE(F__5, NOTE32MS),
	 MAKE_NOTE(E__5, NOTE32MS), MAKE_NOTE(D__5, NOTE32MS), MAKE_NOTE(F__5, NOTE32MS),
	 MAKE_NOTE(E__5, NOTE32MS), MAKE_NOTE(D__5, NOTE32MS), MAKE_NOTE(CS_5, NOTE32MS),
	 MAKE_NOTE(E__5, NOTE32MS), MAKE_NOTE(A__4, NOTE16MS),},
	
	// Unmei
	{MAKE_NOTE(A__5, NOTE16MS), MAKE_NOTE(G__5, NOTE16MS), MAKE_NOTE(A__5, NOTE4MS ),
	 MAKE_NOTE(G__5, NOTE32MS), MAKE_NOTE(F__5, NOTE32MS), MAKE_NOTE(E__5, NOTE32MS), MAKE_NOTE(D__5, NOTE32MS),
	 MAKE_NOTE(CS_5, NOTE8MS ), MAKE_NOTE(D__5, NOTE8MS ),},
};

hw_timer_t * timer = NULL;
hw_timer_t* soundTimer     = nullptr;
portMUX_TYPE soundTimerMux = portMUX_INITIALIZER_UNLOCKED;

int id;
int soundId;
int soundSeqId;
bool isPlayng;

void initSound();
void playSound(int);
void nextSeqSound();
void stopSound();
void IRAM_ATTR onSoundInerrupt();

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

    isPlayng = false;
}

/**
 * @brief 
 * 
 */
void stopSound()
{
    timerAlarmDisable(soundTimer);
    M5.Beep.mute();
    isPlayng = false;
}

/**
 * @brief 
 * 
 */
void nextSeqSound()
{
    if(soundSeqId + 1 == soundList[soundId].size())
    {
        stopSound();
        return;
    }
    
    portENTER_CRITICAL_ISR(&soundTimerMux);
    {
        soundSeqId++;
    }
    portEXIT_CRITICAL_ISR(&soundTimerMux);

    auto param = soundList[soundId][soundSeqId];
    
    // Serial.printf("nextSeq[%d][%d]\n", soundId, soundSeqId);

    if(param.freq == 0)
    {
        M5.Beep.mute();
    }else{
        M5.Beep.tone(param.freq);
    }
    
    timerWrite(soundTimer, 0);      // 経過値を0に
    timerAlarmWrite(soundTimer, param.duration * 1000, false);
    timerAlarmEnable(soundTimer);
}

/**
 * @brief 
 * 
 */
void playSound(const int _soundId)
{
    if(_soundId >= soundList.size())
    {
        stopSound();
        return;
    }

    portENTER_CRITICAL_ISR(&soundTimerMux);
    {
        soundId    = _soundId;
        soundSeqId = 0;
    }
    portEXIT_CRITICAL_ISR(&soundTimerMux);

    auto param = soundList[soundId][soundSeqId];

    // Serial.printf("play[%d][%d]\n", soundId, soundSeqId);

    if(param.freq == 0)
    {
        M5.Beep.mute();
    }else{
        M5.Beep.tone(param.freq);
    }

    isPlayng = true;
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
    nextSeqSound();
}

void IRAM_ATTR onTimer()
{
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
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

            //stopSound();
            playSound(id++);
            id = id % soundList.size();
        }else
        if(M5.BtnB.wasPressed())
        {
            playSound(id);
        }
    }
}
