#pragma config(Sensor, S1, TouchSensor, sensorEV3_Touch)
#pragma config(Sensor, S2, GyroSensor, sensorEV3_Gyro)
#pragma config(Motor, motorA, moteurA, tmotorEV3_Large, PIDControl, encoder)

/*
 * @author: Addwyn Le lann m2 iliade
 * Fichier préliminaire permettant de déterminer les constantes liées au gyromètre.
 * Cela nous donne la vitesse délivrée par le gyromètre en degrés par 
 * seconde en fonction de la puissance.
 * La puissance est testée de 0% à 100% avec un pas de 1%.
 */

// Variables globales
string s_val;
long fileHandle;

task gyroRateCharacterization() {
	// Tableau contentant les différentes vitesse par puissance
	long tab [101];
	// Tabulation entre les différentes puissances
	for (int i=0;i<=100;i++) {
		// Règle la puissance du moteur dans le sens antihoraire
		setMotorSpeed(motorA, -i);
		// Légère attente pour laisser le moteur tourner à la puissance
		// Et ainsi prendre la mesure après que le moteur est fait son demi-tour
		delay(200);
		// Sauvegarde de la vitesse dans le tableau
		tab[i] = getGyroRate(S2);
		// Règle la puissance du moteur dans le sens horaire
		// Permet de dérouler le câble du gyromètre
		setMotorSpeed(motorA, i);
		// Laisse le câble se dérouler
		delay(200);
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
	fileHandle = fileOpenWrite("gyro");
	// Lance notre tâche d'analyse
	startTask(gyroRateCharacterization);
}
