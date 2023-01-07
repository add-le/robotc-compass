#pragma config(Sensor, S1, TouchSensor, sensorEV3_Touch)
#pragma config(Sensor, S2, GyroSensor, sensorEV3_Gyro)
#pragma config(Motor, motorA, motor, tmotorEV3_Large, PIDControl, encoder)

/*
 * @author: Addwyn Le lann m2 iliade
 * Fichier préliminaire permettant de déterminer les constantes liées au moteur.
 * Cela nous donne la vitesse de rotation du moteur en fonction de la puissance.
 * La puissance est testée de 0% à 100% avec un pas de 1%.
 */

// Variables globales
string s_val;
long fileHandle;

task motorCharacterization() {
	// Tableau contentant les différentes vitesse par puissance
	float tab [101];
	// Tabulation entre les différentes puissances
	for (int i=0;i<=100;i++) {
		// Règle la puissance du moteur
		setMotorSpeed(motorA, i);
		// Légère attente pour laisser le moteur tourner à la puissance 
		delay(100);
		// Conversion en degré par seconde
		tab[i]=getMotorRPM(motorA)*6;
	}
	
	// Sauvegarde des données dans le fichier
	for (int i=0; i<=100;i++) {
		stringFormat(s_val, "%ld\n", tab[i]);
		fileWriteData(fileHandle, s_val, strlen(s_val));
	}
	
	// Fin du programme et fermeture du fichier
	fileClose(fileHandle);
	stopAllTasks();
}

task main() {
	// Créer un fichier pour récupérer les résultats
	fileHandle = fileOpenWrite("puissance");
	// Lance notre tâche d'analyse
	startTask(motorCharacterization);
}
