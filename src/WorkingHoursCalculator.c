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
Time calculateBreakTime(Time end)
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

    /* 昼休み開始時刻前に退勤した場合：休憩時間を0分で出力 */
    if (isEarlierThan(end, lunchbreakStartTime))
    {
        return breakTime;
    }
    /* 昼休み終了時刻前に退勤した場合：休憩時間を0分で出力 */
    else if (isEarlierThan(end, lunchbreakEndTime))
    {
        return breakTime;
    }
    /* 定時前or定時に退勤した場合：昼のみの休憩時間を出力 */
    else if (isEarlierThan(end, standardEndTime))
    {
        breakTime = subtractTime(lunchbreakEndTime, lunchbreakStartTime);
    }
    /* 残業開始時刻前に退勤した場合：昼のみの休憩時間を出力 */
    else if(isEarlierThan(end, overtimeStartTime))
    {
        breakTime = subtractTime(lunchbreakEndTime, lunchbreakStartTime);        
    }
    /* 残業開始時刻以降に退勤した場合：残業時の休憩時間を出力 */
    else
    {
        breakTime = subtractTime(lunchbreakEndTime, lunchbreakStartTime);      

        Time overtimeBreakTime;
        overtimeBreakTime = subtractTime(overtimeStartTime, standardEndTime);   
        
        breakTime = addTime(breakTime, overtimeBreakTime);
    }
    return breakTime;
}

/** @brief 残業時間の計算 */
Time calculateOverTime(Time start, Time end, Time breakTime)
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

    Time totalWorkTime;     /* 実際の勤務時間 */
    
    /* 昼休み開始時刻前に退勤した場合：残業時間をマイナスで出力 */
    if (isEarlierThan(end, lunchbreakStartTime))
    {
        totalWorkTime = subtractTime(end, start);
    }
    /* 昼休み終了時刻前に退勤した場合：昼休み開始時刻に退勤したとみなす 残業時間をマイナスで出力 */
    else if (isEarlierThan(end, lunchbreakEndTime))
    {
        totalWorkTime = subtractTime(lunchbreakStartTime, start);
    }
    /* 定時前or定時に退勤した場合：残業時間をマイナスor0で出力 */
    else if (isEarlierThan(end, standardEndTime))
    {
        totalWorkTime = subtractTime(end, start);
        totalWorkTime = subtractTime(totalWorkTime, breakTime);
    }
    /* 残業開始時刻前に退勤した場合：残業時間を0で出力 */
    else if(isEarlierThan(end, overtimeStartTime))
    {
        return overTime;
    }
    /* 残業開始時刻以降に退勤した場合：残業時時間を出力 */
    else
    {
        totalWorkTime = subtractTime(end, start);
        totalWorkTime = subtractTime(totalWorkTime, breakTime);
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
    Time start, end;
    if (parseTime(argv[1], &start) != 2 || parseTime(argv[2], &end) != 2)
    {
        fprintf(stderr, "Invalid time. Input between 0:00 and 23:59.\n");
        return 1;
    }

    /* 退勤時間が出勤時間よりも先の場合、エラーを出力 */
    if(isEarlierThan(end, start))
    {
        fprintf(stderr, "usage: ./WorkingHoursCalculator 8:30 17:00\n");
        return 1;
    };

    /* 休憩時間の計算 */
    Time breakTime = calculateBreakTime(end);

    /* 残業時間の計算 */
    Time overTime = calculateOverTime(start, end, breakTime);

    /* 休憩時間と残業時間の出力 */
    printResult("休憩時間: ", breakTime);
    printResult("残業時間: ", overTime);

    return 0;
}
#endif
