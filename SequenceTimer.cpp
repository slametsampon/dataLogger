/*
 * lihat header file SequenceTimer.h lebih detail
 * extention file harus .cpp bukan .CPP
 */

/* Dependencies */
#include "SequenceTimer.h"

SequenceTimer::SequenceTimer(String id)
{
	_id = id;
	_prevSetMilli = millis();
	_PrevSetASecond = _prevSetMilli;
	_prevSetSecond = _prevSetMilli;
	_prevAMinute = _prevSetMilli;
}
//---------------

void SequenceTimer::setSetingSecond(float val)
{
	_SetingSecond = val*1000;
}
float SequenceTimer::getSetingSecond()
{
	return (_SetingSecond/1000);
}
//---------------

void SequenceTimer::setSetingMiliSecond(long val)
{
	_SetingMiliSecond = val;
}
long SequenceTimer::getSetingMiliSecond()
{
	return _SetingMiliSecond;
}
//---------------

boolean SequenceTimer::isMiliSecondEvent()
{
	boolean valFl;
	valFl = _MiliSecondEvent;
	_MiliSecondEvent = false;
	return valFl;
}
//------------------

boolean SequenceTimer::isSecondEvent()
{
	boolean valFl;
	valFl = _SecondEvent;
	_SecondEvent = false;
	return valFl;
}
//---------------

boolean SequenceTimer::isASecondEvent()
{
	boolean valFl;
	valFl = _aSecond;
	_aSecond = false;
	return valFl;
}
//------------------

boolean SequenceTimer::isAMinuteEvent()
{
	boolean valFl;
	valFl = _aMinute;
	_aMinute = false;
	return valFl;
}
//------------------

void SequenceTimer::execute()
{
	unsigned long currMilli = millis();
	if ((currMilli - _PrevSetASecond) >= A_SECOND){
		_aSecond = true;
		_PrevSetASecond = currMilli;
	}		
	if ((currMilli - _prevAMinute) >= A_MINUTE){
		_aMinute = true;
		_prevAMinute = currMilli;
	}		
	if ((currMilli - _prevSetMilli) >= _SetingMiliSecond){
		_MiliSecondEvent = true;
		_prevSetMilli = currMilli;
	}
	if ((currMilli - _prevSetSecond) >= _SetingSecond){
		_SecondEvent = true;
		_prevSetSecond = currMilli;
	}
}
