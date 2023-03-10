#pragma config(Sensor, S1,     TouchSensor,    sensorEV3_Touch)
#pragma config(Sensor, S2,     GyroSensor,     sensorEV3_Gyro)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// exerice 5

long min, max;
string s_min, s_max, s_val;
long fileHandle;
TSemaphore mutexMinMax;

long val;

task watch() {
	while(1){
		semaphoreLock(mutexMinMax);
		val=getGyroRate(S2);
		if (val<min) min=val;
		if (val>max) max=val;
		semaphoreUnlock(mutexMinMax);
		delay(100);
	}
}

task write() {
	while(1){
		semaphoreLock(mutexMinMax);
		val=getGyroRate(S2);
		if (val<min) min=val;
		if (val>max) max=val;
		stringFormat(s_val, "%ld;", val);
		semaphoreUnlock(mutexMinMax);
		fileWriteData(fileHandle, s_val, strlen(s_val));
		delay(60000);
	}
}

task halt(){
	while (getTouchValue(S1)==0){}
	stopTask(write);
	stopTask(watch);
	stringFormat(s_min, "\n min = %ld\n", min);
	stringFormat(s_max, "max = %ld\n", max);
	fileWriteData(fileHandle, s_min, strlen(s_min));
	fileWriteData(fileHandle, s_max, strlen(s_max));
	fileClose(fileHandle);
	stopAllTasks();

}

task main()
{
	min=getGyroRate(S2);
	max=min;
	fileHandle = fileOpenWrite("GyroData");
	semaphoreInitialize(mutexMinMax);
	startTask(watch);
	startTask(write);
	startTask(halt);
	while(1){}

}
