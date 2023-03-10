#pragma config(Sensor, S1,     TouchSensor,    sensorEV3_Touch)
#pragma config(Sensor, S2,     GyroSensor,     sensorEV3_Gyro)
#pragma config(Motor,  motorA,          motor,         tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

string s_val;
long fileHandle;

task motorCharacterization() {
	float tab [101];
	for (int i=0;i<=100;i++){
		setMotorSpeed(motorA, i);
		delay(100);
		tab[i]=getMotorRPM(motorA)*6;
	}
	for (int i=0; i<=100;i++){
		stringFormat(s_val, "%ld\n", tab[i]);
		fileWriteData(fileHandle, s_val, strlen(s_val));
	}
		fileClose(fileHandle);
		stopAllTasks();
}

task main()
{
	fileHandle = fileOpenWrite("puissance");
	startTask(motorCharacterization);

}
