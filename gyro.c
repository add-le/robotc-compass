#pragma config(Sensor, S1,     TouchSensor,    sensorEV3_Touch)
#pragma config(Sensor, S2,     GyroSensor,     sensorEV3_Gyro)
#pragma config(Motor,  motorA,          moteurA,       tmotorEV3_Large, PIDControl, encoder)

string s_val;
long fileHandle;

task gyroRateCharacterization() {

	long tab [101];

	for (int i=0;i<=100;i++) {
		setMotorSpeed(motorA, -i);
		delay(200);
		tab[i] = getGyroRate(S2);
		setMotorSpeed(motorA, i);
    delay(200);
	}

	for (int i=0; i<=100;i++) {
		stringFormat(s_val, "%ld\n", tab[i]);
		fileWriteData(fileHandle, s_val, strlen(s_val));
	}

		fileClose(fileHandle);
		stopAllTasks();

}

task main() {
	fileHandle = fileOpenWrite("gyro");
	startTask(gyroRateCharacterization);

}
