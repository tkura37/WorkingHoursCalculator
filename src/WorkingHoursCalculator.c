/**
 * ソースコード：WorkingHoursCalculator.c
 * 単体テストコード：test_WorkingHoursCalculator.cpp
 * @brief   休憩時間・残業時間計算プログラム
 * @date    2024/12/29
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "WorkingHoursCalculator.h"

/** @brief 時刻をマクロ定義から構造体メンバへ変換 */
int parseTime(const char *timeStr, Time *time)
{
    int parseResult = sscanf(timeStr, "%d:%d", &time->hours, &time->minutes);

    /* 時：0～23のみ有効 */
    if (time->hours < 0 || time->hours > 23)
    {
        return 1;
    }
    /* 分：0～59のみ有効 */
    if (time->minutes < 0 || time->minutes > 59)
    {
        return 1;
    }
    return parseResult;
}

/** @brief Timeオブジェクト同士の時刻比較 */
bool isEarlierThan(Time timeA, Time timeB)
{
    if (timeA.hours < timeB.hours)
    {
        return true;
    }
    else if (timeA.hours == timeB.hours && timeA.minutes <= timeB.minutes)
    {
        return true;
    }
    return false;
}

/** @brief Timeオブジェクト同士の差分計算(timeA - timeB) */
Time subtractTime(Time timeA, Time timeB)
{
    Time timeDiff;

    timeDiff.hours   = timeA.hours - timeB.hours;
    timeDiff.minutes = timeA.minutes - timeB.minutes;

    if (timeDiff.hours > 0)
    {
        if (timeDiff.minutes < 0)
        {
            timeDiff.hours   -= 1;
            timeDiff.minutes += 60;
        }
    }
    else if (timeDiff.hours == 0)
    {
    }
    else if (timeDiff.hours < 0)
    {
        if (timeDiff.minutes > 0)
        {
            timeDiff.hours   += 1;
            timeDiff.minutes -= 60;
        }
        else
        {
            timeDiff.minutes = abs(timeDiff.minutes);
        }
    }
    else
    {}

    return timeDiff;
}

/** @brief Timeオブジェクト同士の合計計算(timeA + timeB) */
Time addTime(Time timeA, Time timeB)
{
    Time timeSum;

    timeSum.hours   = timeA.hours + timeB.hours;
    timeSum.minutes = timeA.minutes + timeB.minutes;

    if (timeSum.minutes >= 60)
    {
        timeSum.hours   += 1;
        timeSum.minutes -= 60;
    }

    return timeSum;
}

/** @brief 休憩時間の計算 */
Time calculateBreakTime(Time startTime, Time endTime)
{
    Time breakTime = {0, 0};

    Time lunchbreakStartTime;
    Time lunchbreakEndTime;
    Time standardEndTime;
    Time overtimeStartTime;

    parseTime(LUNCHBREAK_START_TIME, &lunchbreakStartTime);
    parseTime(LUNCHBREAK_END_TIME, &lunchbreakEndTime);
    parseTime(STANDARD_END_TIME, &standardEndTime);
    parseTime(OVERTIME_START_TIME, &overtimeStartTime);

    /* 昼休み開始時刻までに出勤した場合 */
    /* 出勤時刻：入力された値 */
    if (isEarlierThan(startTime, lunchbreakStartTime))
    {
        /* 昼休み開始時刻までに退勤した場合 */
        /* 休憩時間：0分 */
        if (isEarlierThan(endTime, lunchbreakStartTime))
        {
            return breakTime;
        }
        /* 昼休み開始時刻～昼休み終了時刻までに退勤した場合 */
        /* 休憩時間：0分 */
        else if (isEarlierThan(endTime, lunchbreakEndTime))
        {
            return breakTime;
        }
        /* 昼休み終了時刻～定時までに退勤した場合 */
        /* 休憩時間：昼のみ */
        else if (isEarlierThan(endTime, standardEndTime))
        {
            breakTime = subtractTime(lunchbreakEndTime, lunchbreakStartTime);
        }
        /* 定時～残業開始時刻までに退勤した場合 */
        /* 休憩時間：昼のみ */
        else if(isEarlierThan(endTime, overtimeStartTime))
        {
            breakTime = subtractTime(lunchbreakEndTime, lunchbreakStartTime);        
        }
        /* 残業開始時刻以降に退勤した場合 */
        /* 休憩時間：昼+残業 */
        else
        {
            breakTime = subtractTime(lunchbreakEndTime, lunchbreakStartTime);

            Time overtimeBreakTime;
            overtimeBreakTime = subtractTime(overtimeStartTime, standardEndTime);   
            
            breakTime = addTime(breakTime, overtimeBreakTime);
        }
        return breakTime;
    }
    /* 昼休み開始時刻～昼休み終了時刻までに出勤した場合(午前休) */
    /* 出勤時刻：昼休み終了時刻とみなす */
    else if (isEarlierThan(startTime, lunchbreakEndTime))
    {
        /* 昼休み開始時刻～昼休み終了時刻までに退勤した場合 */
        /* 休憩時間：0分 */
        if (isEarlierThan(endTime, lunchbreakEndTime))
        {
            return breakTime;
        }
        /* 昼休み終了時刻～定時までに退勤した場合 */
        /* 休憩時間：0分 */
        else if (isEarlierThan(endTime, standardEndTime))
        {
            return breakTime;
        }
        /* 定時～残業開始時刻までに退勤した場合 */
        /* 休憩時間：0分 */
        else if(isEarlierThan(endTime, overtimeStartTime))
        {
            return breakTime;
        }
        /* 残業開始時刻以降に退勤した場合 */
        /* 休憩時間：標準勤務時間以下の場合、0分 */
        /* 休憩時間：標準勤務時間以上の場合、昼+残業 */
        else
        {
            Time totalWorkTime = subtractTime(endTime, lunchbreakEndTime);
            Time standardWorkTime;
            parseTime(STANDARD_WORK_HOURS, &standardWorkTime);

            if (isEarlierThan(totalWorkTime, standardWorkTime))
            {
                return breakTime;
            }
            else
            {
                breakTime = subtractTime(lunchbreakEndTime, lunchbreakStartTime);

                Time overtimeBreakTime;
                overtimeBreakTime = subtractTime(overtimeStartTime, standardEndTime);   
                
                breakTime = addTime(breakTime, overtimeBreakTime);
            }
        }
        return breakTime;
    }
    /* 昼休み終了時刻～定時までに出勤した場合 */
    /* 出勤時刻：入力された値 */
    else if (isEarlierThan(startTime, standardEndTime))
    {
        /* 昼休み終了時刻～定時までに退勤した場合 */
        /* 休憩時間：0分 */
        if (isEarlierThan(endTime, standardEndTime))
        {
            return breakTime;
        }
        /* 定時～残業開始時刻までに退勤した場合 */
        /* 休憩時間：0分 */
        else if(isEarlierThan(endTime, overtimeStartTime))
        {
            return breakTime;
        }
        /* 残業開始時刻以降に退勤した場合 */
        /* 休憩時間：標準勤務時間以下の場合、0分 */
        /* 休憩時間：標準勤務時間以上の場合、昼+残業 */
        else
        {
            Time totalWorkTime = subtractTime(endTime, startTime);
            Time standardWorkTime;
            parseTime(STANDARD_WORK_HOURS, &standardWorkTime);

            if (isEarlierThan(totalWorkTime, standardWorkTime))
            {
                return breakTime;
            }
            else
            {
                breakTime = subtractTime(lunchbreakEndTime, lunchbreakStartTime);

                Time overtimeBreakTime;
                overtimeBreakTime = subtractTime(overtimeStartTime, standardEndTime);   
                
                breakTime = addTime(breakTime, overtimeBreakTime);
            }
        }
        return breakTime;
    }
    /* 定時以降に出勤した場合 */
    else
    {
        /* 定時後の出勤は最初のバリデーションでNGとするため、このパターンには入らない */
    }
}

/** @brief 残業時間の計算 */
Time calculateOverTime(Time startTime, Time endTime, Time breakTime)
{
    Time overTime = {0, 0};

    Time lunchbreakStartTime;
    Time lunchbreakEndTime;
    Time standardEndTime;
    Time overtimeStartTime;
    Time standardWorkTime;

    parseTime(LUNCHBREAK_START_TIME, &lunchbreakStartTime);
    parseTime(LUNCHBREAK_END_TIME, &lunchbreakEndTime);
    parseTime(STANDARD_END_TIME, &standardEndTime);
    parseTime(OVERTIME_START_TIME, &overtimeStartTime);
    parseTime(STANDARD_WORK_HOURS, &standardWorkTime);

    Time totalWorkTime;     /* 勤務時間 */
    
    /* 昼休み開始時刻までに出勤した場合 */
    /* 出勤時刻：入力された値 */
    if (isEarlierThan(startTime, lunchbreakStartTime))
    {
        /* 昼休み開始時刻までに退勤した場合 */
        /* 勤務時間：退勤時刻-出勤時刻 */
        if (isEarlierThan(endTime, lunchbreakStartTime))
        {
            totalWorkTime = subtractTime(endTime, startTime);
        }
        /* 昼休み開始時刻～昼休み終了時刻までに退勤した場合 */
        /* 勤務時間：昼休み開始時刻-出勤時刻 */
        else if (isEarlierThan(endTime, lunchbreakEndTime))
        {
            totalWorkTime = subtractTime(lunchbreakStartTime, startTime);
        }
        /* 昼休み終了時刻～定時までに退勤した場合 */
        /* 勤務時間：退勤時刻-出勤時刻-休憩時間(昼) */
        else if (isEarlierThan(endTime, standardEndTime))
        {
            totalWorkTime = subtractTime(endTime, startTime);
            totalWorkTime = subtractTime(totalWorkTime, breakTime);
        }
        /* 定時～残業開始時刻までに退勤した場合 */
        /* 勤務時間：定時退勤時刻-出勤時刻-休憩時間(昼) */
        else if(isEarlierThan(endTime, overtimeStartTime))
        {
            totalWorkTime = subtractTime(standardEndTime, startTime);
            totalWorkTime = subtractTime(totalWorkTime, breakTime);
        }
        /* 残業開始時刻以降に退勤した場合 */
        /* 勤務時間：退勤時刻-出勤時刻-休憩時間(昼+残業) */
        else
        {
            totalWorkTime = subtractTime(endTime, startTime);
            totalWorkTime = subtractTime(totalWorkTime, breakTime);
        }
    }
    /* 昼休み開始時刻～昼休み終了時刻までに出勤した場合(午前休) */
    /* 出勤時刻：昼休み終了時刻とみなす */
    else if (isEarlierThan(startTime, lunchbreakEndTime))
    {
        /* 昼休み開始時刻～昼休み終了時刻までに退勤した場合 */
        /* 勤務時間：0 */
        if (isEarlierThan(endTime, lunchbreakEndTime))
        {
            return overTime;
        }
        /* 昼休み終了時刻～定時までに退勤した場合 */
        /* 勤務時間：退勤時刻-昼休み終了時刻 */
        else if (isEarlierThan(endTime, standardEndTime))
        {
            totalWorkTime = subtractTime(endTime, lunchbreakEndTime);
        }
        /* 定時～残業開始時刻までに退勤した場合 */
        /* 勤務時間：定時退勤時刻-昼休み終了時刻 */
        else if(isEarlierThan(endTime, overtimeStartTime))
        {
            totalWorkTime = subtractTime(standardEndTime, lunchbreakEndTime);
        }
        /* 残業開始時刻以降に退勤した場合 */
        /* 勤務時間：標準勤務時間以下の場合、退勤時刻-出勤時刻-休憩時間(0分) */
        /* 勤務時間：標準勤務時間以上の場合、退勤時刻-出勤時刻-休憩時間(昼+残業) */
        else
        {
            totalWorkTime = subtractTime(endTime, lunchbreakEndTime);
            totalWorkTime = subtractTime(totalWorkTime, breakTime);
        }
    }
    /* 昼休み終了時刻～定時までに出勤した場合 */
    /* 出勤時刻：入力された値 */
    else if (isEarlierThan(startTime, standardEndTime))
    {
        /* 昼休み終了時刻～定時までに退勤した場合 */
        /* 勤務時間：退勤時刻-出勤時刻 */
        if (isEarlierThan(endTime, standardEndTime))
        {
            totalWorkTime = subtractTime(endTime, startTime);
        }
        /* 定時～残業開始時刻までに退勤した場合 */
        /* 勤務時間：定時退勤時刻-出勤時刻 */
        else if(isEarlierThan(endTime, overtimeStartTime))
        {
            totalWorkTime = subtractTime(standardEndTime, startTime);
        }
        /* 残業開始時刻以降に退勤した場合 */
        /* 勤務時間：標準勤務時間以下の場合、退勤時刻-出勤時刻-休憩時間(0分) */
        /* 勤務時間：標準勤務時間以上の場合、退勤時刻-出勤時刻-休憩時間(昼+残業) */
        else
        {
            totalWorkTime = subtractTime(endTime, startTime);
            totalWorkTime = subtractTime(totalWorkTime, breakTime);
        }
    }
    /* 定時以降に出勤した場合 */
    else
    {
        /* 定時後の出勤は最初のバリデーションでNGとするため、このパターンには入らない */
    }

    overTime = subtractTime(totalWorkTime, standardWorkTime);

    return overTime;
}

/** @brief 時間の出力 */
float printResult(const char *outputLabel, Time time)
{
    printf("%s", outputLabel);
    printf("%d時間%02d分", time.hours, time.minutes);
    
    float resultTime;
    if (time.hours < 0)
    {
        time.hours = abs(time.hours);
        resultTime = (time.hours * 60 + time.minutes) / 60.0;
        resultTime *= -1;
    }
    else{
        resultTime = (time.hours * 60 + time.minutes) / 60.0;
    }
    printf(" (%.2f)\n", resultTime);
    return resultTime;
}

#ifndef TESTING
/** @brief main */
int main(int argc, char *argv[])
{
    /* 入力フォーマットのチェック・エラー出力 */
    if (argc != 3) {
        fprintf(stderr, "usage: ./WorkingHoursCalculator 8:30 17:00\n");
        return 1;
    }

    /* 出勤時間・退勤時間の取り込み */
    Time startTime, endTime;
    if (parseTime(argv[1], &startTime) != 2 || parseTime(argv[2], &endTime) != 2)
    {
        fprintf(stderr, "Invalid time. Input between 0:00 and 23:59.\n");
        return 1;
    }

    /* 退勤時間が出勤時間よりも先の場合、エラーを出力 */
    if(isEarlierThan(endTime, startTime))
    {
        fprintf(stderr, "usage: ./WorkingHoursCalculator 8:30 17:00\n");
        return 1;
    };

    /* 休憩時間・残業時間の計算 */
    Time breakTime = calculateBreakTime(startTime, endTime);
    Time overTime = calculateOverTime(startTime, endTime, breakTime);

    /* 休憩時間と残業時間の出力 */
    printResult("休憩時間: ", breakTime);
    printResult("残業時間: ", overTime);

    return 0;
}
#endif
