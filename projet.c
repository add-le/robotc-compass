#pragma config(Sensor, S1, TouchSensor, sensorEV3_Touch)
#pragma config(Sensor, S2, GyroSensor, sensorEV3_Gyro)
#pragma config(Motor, motorA, moteurA, tmotorEV3_Large, PIDControl, encoder)

/*
 * @author: Addwyn Le lann m2 iliade
 */

// Constantes
#define PENTE 9.885
#define MAX_SPEED 860
#define MIN_POWER 5
#define MAX_POWER 87

#define PENTE_2 9.5
#define MAX_SPEED_2 494
#define MIN_POWER_2 9
#define MAX_POWER_2 52

#define P 100
#define D -1

#define CONSIGNE_GYRO 0

// Variables globales
int consigne = 0;
TSemaphore mutexConsigne;
int changement = false;

// Déclare le prototype la fonction "choix" en avance pour pouvoir faire des appels récursifs
void choix();

// Permet de limiter la vitesse du moteur à notre constante calculé avec l'encodeur optique
void launchMotorSpeed(int speed) {
	if(abs(speed) > MAX_SPEED) {
		speed = MAX_SPEED;
	}
	setMotorSpeed(motorA, speed/PENTE);
}

// Permet de limiter la vitesse du moteur à notre constante calculé avec le gyromètre
void launchMotorSpeed2(int speed) {
	if(abs(speed) > MAX_SPEED_2) {
		speed = MAX_SPEED_2;
	}
	setMotorSpeed(motorA, speed/PENTE_2);
}

// Fonction qui indique le cap à exécuter pour le moteur
task keepHeading() {
	int consigneCopy;
	while(1) {
		delay(50);
		// Sémaphore pour empêcher les lectures et écritues sur la même consigne
		semaphoreLock(mutexConsigne);
		// Création d'une copie de la consigne, pour relâcher le sémaphore plus vite
		consigneCopy = consigne;
		semaphoreUnlock(mutexConsigne);
		if(changement) {
			// Fait tourner le moteur à la consigne indiquée avec une puissance de 20
			setMotorTarget(motorA, consigneCopy, 20);
			// Indique que l'action a été exécutée
			changement = false;
		}
	}
}

// Fonction qui éxécute le cap avec la formule Vitesse = P(consigne-cap)+D(vitesseAngulaire)
task keepHeadingPD() {
	// Mise par défaut des différents capteurs
	resetMotorEncoder(motorA);
	resetGyro(S2);

	int consigneCopy;
	int vitesse;

	while(1) {
		delay(50);
		semaphoreLock(mutexConsigne);
		consigneCopy = consigne;
		semaphoreUnlock(mutexConsigne);
		if(changement) {
			// Calcule de la vitesse en fonction des constantes P et D
			vitesse = P * (consigneCopy - getMotorEncoder(motorA)) + D * getMotorRPM(motorA)*6;
			launchMotorSpeed(vitesse);
			setMotorTarget(motorA, consigneCopy, getMotorSpeed(motorA));
			changement = false;
		}
	}
}

// Fonction qui utilise la formule P et D provenant du gyromètre
task keepHeadingPD2() {
	resetMotorEncoder(motorA);
	resetGyro(S2);

	int vitesse;

	while(1) {
		delay(50);
		// Force le cap à être toujours dirigé vers 0
		vitesse = P * (CONSIGNE_GYRO - getGyroDegrees(S2)) + D * getGyroRate(S2);
		launchMotorSpeed2(-vitesse/10);
	}
}

// Fonction qui affiche une IHM indiquant les différentes actions possibles
task IHM() {
	eraseDisplay();
	displayString(1, "R : Turn motor 10d right");
	displayString(2, "L : Turn motor 10d left");
	displayString(3, "U : Turn motor 90d right");
	displayString(4, "D : Turn motor 90d left");
	//displayString(5, "Enter : End program");
	displayString(5, "Enter : Return to menu");

	// Delay pour actualiser l'écran sur les consignes
	while(1) {
		delay(100);
		displayString(7, "consigne : %d", consigne);
		displayString(8, "cap : %f", getMotorEncoder(motorA));
	}
}

// Fonction qui "écoute" les différentes touches préssées
task watchButtons() {
	while(1) {
		delay(50);

		// Tourne de 10 degrés vers la droite
		if(getButtonPress(buttonRight)) {
			semaphoreLock(mutexConsigne);
			consigne -= 10;
			semaphoreUnlock(mutexConsigne);
			changement = true;
			while(getButtonPress(buttonRight)){}
		}

		// Tourne de 10 degrés vers la gauche
		if(getButtonPress(buttonLeft)) {
			semaphoreLock(mutexConsigne);
			consigne += 10;
			semaphoreUnlock(mutexConsigne);
			changement = true;
			while(getButtonPress(buttonLeft)){}
		}

		// Tourne de 90 degrés vers la droite
		if(getButtonPress(buttonUp)) {
			semaphoreLock(mutexConsigne);
			consigne -= 90;
			semaphoreUnlock(mutexConsigne);
			changement = true;
			while(getButtonPress(buttonUp)){}
		}

		// Tourne de 90 degrés vers la gauche
		if(getButtonPress(buttonDown)) {
			semaphoreLock(mutexConsigne);
			consigne += 90;
			semaphoreUnlock(mutexConsigne);
			changement = true;
			while(getButtonPress(buttonDown)){}
		}
		
		// Quitte le mode en cours
		//if(getButtonPress(buttonEnter)) stopAllTasks();
		if(getButtonPress(buttonEnter)) {
			stopTask(IHM);
			stopTask(keepHeadingPD);
			choix();
			stopTask(watchButtons);
		}
	}
}

// Affiche une IHM pour le mode mobile
task IHM2() {
	eraseDisplay();
	//displayString(1, "Enter : End program");
	displayString(1, "Enter : Return to menu");

	while(1) {
		delay(100);
		displayString(3, "consigne : %d", CONSIGNE_GYRO);
		displayString(4, "cap : %f", getGyroHeading(S2));
	}
}

// Premet d'écouter les touches en mode mobile
task watchButtons2() {
	while(1) {
		delay(50);
		// Met fin au mode
		//if(getButtonPress(buttonEnter)) stopAllTasks();
		if(getButtonPress(buttonEnter)) {
			stopTask(IHM2);
			stopTask(keepHeadingPD2);
			launchMotorSpeed2(0);
			choix();
			stopTask(watchButtons2);
		}
	}
}

// IHM expliquant les touches pour dérouler le câble du gyromètre
void IHM_init() {
	eraseDisplay();
	displayString(1, "R : Turn motor right");
	displayString(2, "L : Turn motor left");
	//displayString(3, "Enter : End program");
	displayString(3, "Enter : Validate");
}

// Fonction qui init l'ensemble des modes
void initialize() {

	consigne = 0;
	changement = false;

	resetGyro(S2);
	resetMotorEncoder(motorA);
	setMotorSpeed(motorA, 0);

	IHM_init();
	// Permet de dérouler le câble
	while(1) {
		delay(50);

		// Tourne le moteur vers la gauche avec 20 de puissance
		if(getButtonPress(buttonLeft)) {
			setMotorSpeed(motorA, 20);
			while(getButtonPress(buttonLeft)){}
		}

		// Tourne le moteur vers la droite avec 20 de puissance
		if(getButtonPress(buttonRight)) {
			setMotorSpeed(motorA, -20);
			while(getButtonPress(buttonRight)){}
		}

		//if(getButtonPress(buttonEnter)) stopAllTasks();
		if(getButtonPress(buttonEnter)) {
			setMotorSpeed(motorA, 0);
			break;
		}
	}
}

// Permet de choisir entre les deux modes
void choix() {
	eraseDisplay();
	displayString(1, "R : Robot mobile");
	displayString(2, "L : Robot fixe");

	while(1) {
		delay(50);

		if(getButtonPress(buttonLeft)) {

			delay(200);
			initialize();
			delay(200);

			startTask(IHM);
			startTask(keepHeadingPD);
			startTask(watchButtons);

			break;
		}

		if(getButtonPress(buttonRight)) {

			delay(200);
			initialize();
			delay(200);

			startTask(IHM2);
			startTask(keepHeadingPD2);
			startTask(watchButtons2);

			break;
		}

	}
}

// Permet de quitter le programme à tout moment grâce au bouton poussoir
task watchStop() {
	while(1) {
		delay(20);
		if(getTouchValue(S1)) stopAllTasks();
	}
}

// Programme principal
task main() {
	resetMotorEncoder(motorA);
	resetGyro(S2);
	semaphoreInitialize(mutexConsigne);

	//startTask(keepHeading);

	startTask(watchStop);
	choix();

	while(1);
}
