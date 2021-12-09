/**
 * @file Framer.h
 * @author kaoru ota
 * @brief 
 * @version 0.1
 * @date 2019-09-23
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef __FrameTimer_h
#define __FrameTimer_h

#include "Arduino.h"

template<class T> struct Property // プロパティ構文を支援するクラス
{ 
    T& r;
    operator T() { return r; }
    void operator =(const T v) { r = v; }
};

template <class T, typename T_FuncPtr>
class FrameTimer 
{
private:
    unsigned long lastEvMillis;
    unsigned long wait;
    unsigned long judgeSec;
    unsigned int  prevResult;
    unsigned int  counter;
    uint8_t fps;
    T         *target;
    T_FuncPtr pfunc;            // void (*pfunc)(bool)

public:
    /**
     * @brief Construct a new Frame Timer object
     * 
     * @param n 
     */
    FrameTimer(uint8_t n = 10)
    : lastEvMillis(0)
    , judgeSec(0)
    , prevResult(0)
    , counter(0)
    , fps(n)
    , pfunc(nullptr)
    {
        SetFPS(fps);
    }

    /**
     * @brief Loopなどで呼ぶ
     * 
     */
    void Update()
    {
        auto now     = millis();

        auto isFpsEv = (now - lastEvMillis >= wait); ///< millis()の循環にも耐える
        auto sec     = now / 1000;
        
        if(isFpsEv)
        {
            if(judgeSec != sec)
            {
                judgeSec   = sec;
                prevResult = counter;
                counter    = 0;
            }
            counter++;
        }
        
        if(pfunc) 
        {
            (target->*pfunc)(isFpsEv); // void (*p)(bool)
        }
        if(isFpsEv)
        {
            lastEvMillis = now;
        }
    }

    /**
     * @brief 
     * 
     * @param n 
     */
    void SetFPS(uint8_t n)
    {
        fps  = (uint8_t)max(1, (int)(n));
        wait = 1000 / fps;
    }

    /**
     * @brief Set the Wait Time object
     * 
     * @param millis 
     */
    void SetWaitTime(int millis)
    {
        fps  = 1000 / max(1, millis);
        wait = millis;
    }

    /**
     * @brief Set the Function object
     * 
     * @param fp 
     */
    void SetFunction(T *target, T_FuncPtr fp)
    {
        this->target = target;
        pfunc = fp;
    }
    
    /**
     * @brief FPS設定値
     * 
     * @return uint8_t 
     */
    uint8_t GetFPS()
    {
        return fps;
    }
    
    /**
     * @brief 実際のフレーム数
     * 
     * @return int 
     */
    int ActualFPS()
    {
        return prevResult;
    }

    /**
     * @brief 強制実行
     * 
     */
    void ForceRaise()
    {
        lastEvMillis = 0;
    }
};


template <typename T_FuncPtr>
class FrameTimer_g
{
private:
    unsigned long lastEvMillis;
    unsigned long wait;
    unsigned long judgeSec;
    unsigned int  prevResult;
    unsigned int  counter;
    uint8_t fps;
    T_FuncPtr pfunc;            // void (*pfunc)(bool)

public:
    /**
     * @brief Construct a new Frame Timer object
     * 
     * @param n 
     */
    FrameTimer_g(uint8_t n = 10)
    : lastEvMillis(0)
    , judgeSec(0)
    , prevResult(0)
    , counter(0)
    , fps(n)
    , pfunc(nullptr)
    {
        SetFPS(fps);
    }

    /**
     * @brief Loopなどで呼ぶ
     * 
     */
    void Update()
    {
        auto now     = millis();

        auto isFpsEv = (now - lastEvMillis >= wait); ///< millis()の循環にも耐える
        auto sec     = now / 1000;
        
        if(isFpsEv)
        {
            if(judgeSec != sec)
            {
                judgeSec   = sec;
                prevResult = counter;
                counter    = 0;
            }
            counter++;
        }
        
        if(pfunc) 
        {
            (*pfunc)(isFpsEv); // void (*p)(bool)
        }
        if(isFpsEv)
        {
            lastEvMillis = now;
        }
    }

    /**
     * @brief 
     * 
     * @param n 
     */
    void SetFPS(uint8_t n)
    {
        fps  = (uint8_t)max(1, (int)(n));
        wait = 1000 / fps;
    }

    /**
     * @brief Set the Wait Time object
     * 
     * @param millis 
     */
    void SetWaitTime(int millis)
    {
        fps  = 1000 / max(1, millis);
        wait = millis;
    }

    /**
     * @brief Set the Function object
     * 
     * @param fp 
     */
    void SetFunction(T_FuncPtr fp)
    {
        pfunc = fp;
    }
    
    /**
     * @brief FPS設定値
     * 
     * @return uint8_t 
     */
    uint8_t GetFPS()
    {
        return fps;
    }
    
    /**
     * @brief 実際のフレーム数
     * 
     * @return int 
     */
    int ActualFPS()
    {
        return prevResult;
    }

    /**
     * @brief 強制実行
     * 
     */
    void ForceRaise()
    {
        lastEvMillis = 0;
    }
};

//void NotWork();

#endif // __Framer_h
