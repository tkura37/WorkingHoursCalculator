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
    if(timeStr == NULL || time == NULL)
    {
        return 1;
    }
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
bool isEarlierThan(Time time1, Time time2)
{
    if (time1.hours < time2.hours)
    {
        return true;
    }
    else if (time1.hours == time2.hours && time1.minutes <= time2.minutes)
    {
        return true;
    }
    return false;
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
        breakTime.hours     = lunchbreakEndTime.hours - lunchbreakStartTime.hours;
        breakTime.minutes   = lunchbreakEndTime.minutes - lunchbreakStartTime.minutes;
        if (breakTime.minutes < 0)
        {
            breakTime.hours -= 1;
            breakTime.minutes += 60;
        }
    }
    /* 残業開始時刻前に退勤した場合：昼のみの休憩時間を出力 */
    else if(isEarlierThan(end, overtimeStartTime))
    {
        breakTime.hours     = lunchbreakEndTime.hours - lunchbreakStartTime.hours;
        breakTime.minutes   = lunchbreakEndTime.minutes - lunchbreakStartTime.minutes;

        if (breakTime.minutes < 0)
        {
            breakTime.hours -= 1;
            breakTime.minutes += 60;
        }
    }
    /* 残業開始時刻以降に退勤した場合：残業時の休憩時間を出力 */
    else
    {
        breakTime.hours     = lunchbreakEndTime.hours - lunchbreakStartTime.hours;
        breakTime.minutes   = lunchbreakEndTime.minutes - lunchbreakStartTime.minutes;

        Time overtimeBreakTime;
        overtimeBreakTime.hours     = overtimeStartTime.hours - standardEndTime.hours;
        overtimeBreakTime.minutes   = overtimeStartTime.minutes - standardEndTime.minutes;
        
        breakTime.hours     += overtimeBreakTime.hours;
        breakTime.minutes   += overtimeBreakTime.minutes;
        
        if (breakTime.minutes < 0)
        {
            breakTime.hours -= 1;
            breakTime.minutes += 60;
        }
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

    int standardWorkMinutes;    /* 標準勤務時間(分) */
    int totalWorkMinutes;       /* 実際の勤務時間(分) */
    int overTimeMinutes;        /* 残業時間(分) */
    standardWorkMinutes = standardWorkTime.hours * 60 + standardWorkTime.minutes;
    
    /* 昼休み開始時刻前に退勤した場合：残業時間をマイナスで出力 */
    if (isEarlierThan(end, lunchbreakStartTime))
    {
        totalWorkMinutes = (end.hours * 60 + end.minutes) - (start.hours * 60 + start.minutes);
    }
    /* 昼休み終了時刻前に退勤した場合：昼休み開始時刻に退勤したとみなす 残業時間をマイナスで出力 */
    else if (isEarlierThan(end, lunchbreakEndTime))
    {
        totalWorkMinutes = (lunchbreakStartTime.hours * 60 + lunchbreakStartTime.minutes) - (start.hours * 60 + start.minutes);
    }
    /* 定時前or定時に退勤した場合：残業時間をマイナスor0で出力 */
    else if (isEarlierThan(end, standardEndTime))
    {
        totalWorkMinutes = (end.hours * 60 + end.minutes) - (start.hours * 60 + start.minutes) - (breakTime.hours * 60 + breakTime.minutes);
    }
    /* 残業開始時刻前に退勤した場合：残業時間を0で出力 */
    else if(isEarlierThan(end, overtimeStartTime))
    {
        return overTime;
    }
    /* 残業開始時刻以降に退勤した場合：残業時時間を出力 */
    else
    {
        totalWorkMinutes = (end.hours * 60 + end.minutes) - (start.hours * 60 + start.minutes) - (breakTime.hours * 60 + breakTime.minutes);
    }
    overTimeMinutes = totalWorkMinutes - standardWorkMinutes;
    overTime.hours = overTimeMinutes / 60;
    overTime.minutes = abs(overTimeMinutes % 60);
    
    return overTime;
}

/** @brief 時間の出力 */
void printResult(const char *outputLabel, const Time *time)
{
    printf("%s", outputLabel);
    printf("%d時間%02d分", time->hours, abs(time->minutes));
    printf(" (%.2f)\n", (time->hours * 60 + time->minutes) / 60.0);
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
    if((end.hours < start.hours) || (end.hours == start.hours && end.minutes < start.minutes))
    {
        fprintf(stderr, "usage: ./WorkingHoursCalculator 8:30 17:00\n");
        return 1;
    };

    /* 休憩時間の計算 */
    Time breakTime = calculateBreakTime(end);

    /* 残業時間の計算 */
    Time overTime = calculateOverTime(start, end, breakTime);

    /* 休憩時間と残業時間の出力 */
    printResult("休憩時間: ", &breakTime);
    printResult("残業時間: ", &overTime);

    return 0;
}
#endif
