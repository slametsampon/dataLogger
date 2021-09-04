/*
  SequenceTimer.h - Library untuk Sequence Timer
  Dibuat Oleh : Abi May 2017
*/
#ifndef SequenceTimer_h
#define SequenceTimer_h

#include "Arduino.h"

const int A_SECOND	= 1000; //1000 MILLI
const int A_MINUTE	= 60000;  //60000 MILLI

class SequenceTimer
{
  public:
  SequenceTimer(String );
	void setPulsePeriod(long val);
	long getPulsePeriod();
  void setSetingMiliSecond(long val);
	long getSetingMiliSecond();
  void setSetingSecond(float val);
	float getSetingSecond();
	boolean isMiliSecondEvent();
	boolean isSecondEvent();
	boolean isASecondEvent();
	boolean isAMinuteEvent();
	void execute();
  private:
  long _SetingMiliSecond;
  long _SetingSecond;
	unsigned long _prevMilli,_prevSetMilli, _PrevSetASecond,_prevSetSecond,_prevAMinute;
	boolean		_MiliSecondEvent;
	boolean		_SecondEvent;
	boolean		_aMinute;
	boolean		_aSecond,_runCycle;
	String 		_id;
};

#endif
