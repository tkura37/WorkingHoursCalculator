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
bool isEarlierThan(const Time *timeA, const Time *timeB)
{
    if (timeA->hours < timeB->hours)
    {
        return true;
    }
    else if ((timeA->hours == timeB->hours) && (timeA->minutes <= timeB->minutes))
    {
        return true;
    }
    return false;
}

/** @brief マクロ定義時刻の順番バリデーション */
bool validateSettingTimeOrder(const USER_SETTING *userSetting)
{
    if (isEarlierThan(&userSetting->lunchbreakEndTime, &userSetting->lunchbreakStartTime))
    {
        return false;
    }
    if (isEarlierThan(&userSetting->standardEndTime, &userSetting->lunchbreakEndTime))
    {
        return false;
    }
    if (isEarlierThan(&userSetting->overtimeStartTime, &userSetting->standardEndTime))
    {
        return false;
    }
    return true;
}

/** @brief Timeオブジェクト同士の差分計算(timeA - timeB) */
Time subtractTime(const Time *timeA, const Time *timeB)
{
    Time timeDiff;

    timeDiff.hours   = timeA->hours - timeB->hours;
    timeDiff.minutes = timeA->minutes - timeB->minutes;

    /* hoursが0以外の場合、minutesは常に正の値を取る */
    if (timeDiff.hours > 0)
    {
        if (timeDiff.minutes < 0)
        {
            timeDiff.hours   -= 1;
            timeDiff.minutes += 60;
        }
    }
    /* hoursが0の場合、minutesは負の値を取りうる */
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
        /* hoursが0以外の場合、minutesは常に正の値を取る */
        if (timeDiff.hours != 0)
        {
            timeDiff.minutes = abs(timeDiff.minutes);
        }
    }
    else
    {
    }

    return timeDiff;
}

/** @brief Timeオブジェクト同士の合計計算(timeA + timeB) */
Time addTime(const Time *timeA, const Time *timeB)
{
    Time timeSum;

    timeSum.hours   = timeA->hours + timeB->hours;
    timeSum.minutes = timeA->minutes + timeB->minutes;

    if (timeSum.minutes >= 60)
    {
        timeSum.hours   += 1;
        timeSum.minutes -= 60;
    }

    return timeSum;
}

/** @brief 休憩時間と残業時間の計算 */
void calculateBreakAndOverTime(const USER_INPUT *userInput, const USER_SETTING *userSetting, Time *breakTime, Time *overTime)
{
    Time lunchBreakTime = subtractTime(&userSetting->lunchbreakEndTime, &userSetting->lunchbreakStartTime);
    Time overtimeBreakTime = subtractTime(&userSetting->overtimeStartTime, &userSetting->standardEndTime);
    Time fullBreakTime = addTime(&lunchBreakTime, &overtimeBreakTime);
    Time totalWorkTime;

    /* 昼休み開始時刻までに出勤した場合 */
    /* 出勤時刻：入力された値 */
    if (isEarlierThan(&userInput->startTime, &userSetting->lunchbreakStartTime))
    {
        /* 昼休み開始時刻までに退勤した場合 */
        /* 休憩時間：0分 */
        /* 勤務時間：退勤時刻-出勤時刻 */
        if (isEarlierThan(&userInput->endTime, &userSetting->lunchbreakStartTime))
        {
            totalWorkTime = subtractTime(&userInput->endTime, &userInput->startTime);
        }
        /* 昼休み開始時刻～昼休み終了時刻までに退勤した場合 */
        /* 休憩時間：0分 */
        /* 勤務時間：昼休み開始時刻-出勤時刻 */
        else if (isEarlierThan(&userInput->endTime, &userSetting->lunchbreakEndTime))
        {
            totalWorkTime = subtractTime(&userSetting->lunchbreakStartTime, &userInput->startTime);
        }
        /* 昼休み終了時刻～定時までに退勤した場合 */
        /* 休憩時間：昼のみ */
        /* 勤務時間：退勤時刻-出勤時刻-休憩時間(昼) */
        else if (isEarlierThan(&userInput->endTime, &userSetting->standardEndTime))
        {
            *breakTime = lunchBreakTime;
            totalWorkTime = subtractTime(&userInput->endTime, &userInput->startTime);
            totalWorkTime = subtractTime(&totalWorkTime, breakTime);
        }
        /* 定時～残業開始時刻までに退勤した場合 */
        /* 休憩時間：昼のみ */
        /* 勤務時間：定時退勤時刻-出勤時刻-休憩時間(昼) */
        else if (isEarlierThan(&userInput->endTime, &userSetting->overtimeStartTime))
        {
            *breakTime = lunchBreakTime;
            totalWorkTime = subtractTime(&userSetting->standardEndTime, &userInput->startTime);
            totalWorkTime = subtractTime(&totalWorkTime, breakTime);
        }
        /* 残業開始時刻以降に退勤した場合 */
        /* 休憩時間：昼+残業 */
        /* 勤務時間：退勤時刻-出勤時刻-休憩時間(昼+残業) */
        else
        {
            *breakTime = fullBreakTime;
            totalWorkTime = subtractTime(&userInput->endTime, &userInput->startTime);
            totalWorkTime = subtractTime(&totalWorkTime, breakTime);
        }
    }
    /* 昼休み開始時刻～昼休み終了時刻までに出勤した場合(午前休) */
    /* 出勤時刻：昼休み終了時刻とみなす */
    else if (isEarlierThan(&userInput->startTime, &userSetting->lunchbreakEndTime))
    {
        /* 昼休み開始時刻～昼休み終了時刻までに退勤した場合 */
        /* 休憩時間：0分 */
        /* 勤務時間：0分 */
        if (isEarlierThan(&userInput->endTime, &userSetting->lunchbreakEndTime))
        {
            return;
        }
        /* 昼休み終了時刻～定時までに退勤した場合 */
        /* 休憩時間：0分 */
        /* 勤務時間：退勤時刻-昼休み終了時刻 */
        else if (isEarlierThan(&userInput->endTime, &userSetting->standardEndTime))
        {
            totalWorkTime = subtractTime(&userInput->endTime, &userSetting->lunchbreakEndTime);
        }
        /* 定時～残業開始時刻までに退勤した場合 */
        /* 休憩時間：0分 */
        /* 勤務時間：定時退勤時刻-昼休み終了時刻 */
        else if (isEarlierThan(&userInput->endTime, &userSetting->overtimeStartTime))
        {
            totalWorkTime = subtractTime(&userSetting->standardEndTime, &userSetting->lunchbreakEndTime);
        }
        /* 残業開始時刻以降に退勤した場合 */
        /* 休憩時間：標準勤務時間以下の場合、0分 */
        /* 休憩時間：標準勤務時間以上の場合、昼+残業 */
        /* 勤務時間：標準勤務時間以下の場合、退勤時刻-出勤時刻-休憩時間(0分) */
        /* 勤務時間：標準勤務時間以上の場合、退勤時刻-出勤時刻-休憩時間(昼+残業) */
        else
        {
            totalWorkTime = subtractTime(&userInput->endTime, &userSetting->lunchbreakEndTime);
            if (isEarlierThan(&totalWorkTime, &userSetting->standardWorkTime))
            {
            }
            else
            {
                *breakTime = fullBreakTime;
            }
            totalWorkTime = subtractTime(&totalWorkTime, breakTime);
        }
    }
    /* 昼休み終了時刻～定時までに出勤した場合 */
    /* 出勤時刻：入力された値 */
    else if (isEarlierThan(&userInput->startTime, &userSetting->standardEndTime))
    {
        /* 昼休み終了時刻～定時までに退勤した場合 */
        /* 休憩時間：0分 */
        /* 勤務時間：退勤時刻-出勤時刻 */
        if (isEarlierThan(&userInput->endTime, &userSetting->standardEndTime))
        {
            totalWorkTime = subtractTime(&userInput->endTime, &userInput->startTime);
        }
        /* 定時～残業開始時刻までに退勤した場合 */
        /* 休憩時間：0分 */
        /* 勤務時間：定時退勤時刻-出勤時刻 */
        else if (isEarlierThan(&userInput->endTime, &userSetting->overtimeStartTime))
        {
            totalWorkTime = subtractTime(&userSetting->standardEndTime, &userInput->startTime);
        }
        /* 残業開始時刻以降に退勤した場合 */
        /* 休憩時間：標準勤務時間以下の場合、0分 */
        /* 休憩時間：標準勤務時間以上の場合、昼+残業 */
        /* 勤務時間：標準勤務時間以下の場合、退勤時刻-出勤時刻-休憩時間(0分) */
        /* 勤務時間：標準勤務時間以上の場合、退勤時刻-出勤時刻-休憩時間(昼+残業) */
        else
        {
            totalWorkTime = subtractTime(&userInput->endTime, &userInput->startTime);
            if (isEarlierThan(&totalWorkTime, &userSetting->standardWorkTime))
            {
            }
            else
            {
                *breakTime = fullBreakTime;
            }
            totalWorkTime = subtractTime(&totalWorkTime, breakTime);
        }
    }
    /* 定時以降に出勤した場合 */
    else
    {
        /* 定時後の出勤は最初のバリデーションでNGとするため、このパターンには入らない */
    }

    /* 残業時間の計算 */
    *overTime = subtractTime(&totalWorkTime, &userSetting->standardWorkTime);
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

    /* ユーザー入力値(出勤時間・退勤時間)の取り込み */
    USER_INPUT userInput;
    if (parseTime(argv[1], &userInput.startTime) != 2 || parseTime(argv[2], &userInput.endTime) != 2)
    {
        fprintf(stderr, "Invalid time. Input between 0:00 and 23:59.\n");
        return 1;
    }
    /* 退勤時間が出勤時間よりも先の場合、エラーを出力 */
    if(isEarlierThan(&userInput.endTime, &userInput.startTime))
    {
        fprintf(stderr, "usage: ./WorkingHoursCalculator 8:30 17:00\n");
        return 1;
    };

    /* ユーザー設定値の取り込み */
    USER_SETTING userSetting;
    parseTime(LUNCHBREAK_START_TIME, &userSetting.lunchbreakStartTime);
    parseTime(LUNCHBREAK_END_TIME, &userSetting.lunchbreakEndTime);
    parseTime(STANDARD_END_TIME, &userSetting.standardEndTime);
    parseTime(OVERTIME_START_TIME, &userSetting.overtimeStartTime);
    parseTime(STANDARD_WORK_HOURS, &userSetting.standardWorkTime);

    if (!validateSettingTimeOrder(&userSetting))
    {
        return 1;
    }

    /* 休憩時間・残業時間の計算 */
    Time breakTime = {0, 0};
    Time overTime = {0, 0};
    calculateBreakAndOverTime(&userInput, &userSetting, &breakTime, &overTime);
    
    /* 休憩時間と残業時間の出力 */
    printResult("休憩時間: ", breakTime);
    printResult("残業時間: ", overTime);

    return 0;
}
#endif
