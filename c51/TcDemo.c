#include "ts2x.api.h"
main()
{
twU08 sensorState;
ts2xDebug_t dbgInfo;

dbgInfo.command=twCmd_StartCal;
dbgInfo.sensitivity=800;        //灵敏度
dbgInfo.burst=35;

dbgInfo.command|=twCmd_SetMode;
dbgInfo.mode=twStateWorking;

dbgInfo.command|=twCmd_Tune；
dbgInfo.signalTarget=100;       
dbgInfo.triggerTarget=10;       //触发阈值
dbgInfo.longPressDelay=156;     //长按时间5000ms/32ms

do
{
twS16 adcVal=getAdc();
sensorState=ts2xSensorUpdate(adcVal,&dbgInfo)
if(sensorState==1)
  {}
else if(sensorState==0)
  {}
printf("%4d,%4d,%4d,%4d,%4d,%4d,%4d,%4d,%4d",
        adcVal,
        sensorState<<4,
        dbgInfo.base,
        dbgInfo.resVal,       //校准后的基线值
        dbgInfo.admVal,       
        dbgInfo.kineState,
        dbgInfo.fwVer,
        dbgInfo.sensitivity,
        dbgInfo.triggerTarget
        )
Delay31ms();          //延时30~50ms传参
}
while(true);
}
