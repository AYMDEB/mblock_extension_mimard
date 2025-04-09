_**Pour réaliser des modifications de l'extension, il est impératif d'utiliser les librairies fournies car plusieurs ont été modifiées afin de fonctionner avec mblock suite à plusieurs bugs découverts lors des essais.**_
# Composants pris en charge
![image](https://github.com/user-attachments/assets/0b320a84-faf4-4cc7-92fe-e13d6deb435e)

# Télémètre ultrason
Si on met plusieurs blocs de mesures de distance trop rapprochés, un bug peut apparaître :
![image](https://github.com/user-attachments/assets/9bc026dd-af62-46a3-9e95-f89095e93568)
Je pense que c'est du au fait que le programme fait relancer un 2ème signal ultrasonnique au capteur avant que le premier lui soit revenu (ou en même temps) ce qui perturbe la mesure.
Pour corriger ce défaut (et faire un code plus propre) il suffit de stocker la valeur du capteur dans une variable et de faire les tests sur cette variable :
![image](https://github.com/user-attachments/assets/1f82acd4-3911-4cc3-be0e-1eaf01e90a46)

# Servomoteur
Le servomoteur ne peut s'orienter qu'entre 0° et 180°. les angles inférieur à 0 sont considérés comme nul, les angles supérieur à 180 sont considérés comme égaux à 180° : https://github.com/arduino-libraries/Servo/blob/master/src/avr/Servo.cpp

# Carte moteur L298P
Comme il se branche en I2C, il nécessite d'être initialisé au démarage :

![image](https://github.com/user-attachments/assets/22c88d67-b82e-4aac-9894-9494f8b35ee2)

### Adressage I2C
![image](https://github.com/user-attachments/assets/15ce8b1a-7cc5-4677-9e6f-24ac51e8fd68)

L’adresse I2C de ces cartes est paramétrable à l’aide de petits cavalier (encadrés en rouge).
L’adresse se construit en binaire et peut donc aller de 0b0000 à 0b1111.
Une absence de cavalier correspond à un 1 et la présence d’un cavalier à un 0.
Ainsi, par défaut la carte n’ayant pas de cavalier, sont adresse est 0b1111 soit 0x0F.
L’attribution de l’adresse se fait à la mise sous tension de l’ensemble carte moteur + Arduino.
### Branchement des moteurs
![image](https://github.com/user-attachments/assets/d2c4281d-6d82-42e6-9cf1-3a108e310ef9)

Les moteurs se branchent sur ces borniers à vis :
M1- et M1+ pour le moteur 1
M2- et M2+ pour le moteur 2
Pour les moteurs à courant continu, il n’y a pas vraiment de + et de – car le sens du courant détermine le sens de rotation. Donc il suffit de brancher le moteur et s’il tourne dans le mauvais sens, on inverse le branchement.
La vitesse de rotation d’un moteur à courant continu dépend de la tension qu’il reçoit, la carte va donc faire varier la tension pour ajuster la vitesse des moteurs.

Dans mblock, ce bloc sert à piloter les moteurs :
 ![image](https://github.com/user-attachments/assets/40a5d63a-b517-4607-8975-331ebd164e5e)
 
Vitesse du moteur 1 à 100 signifie qu’on envoie 100% de la tension au moteur 1
Vitesse à -100 signifie qu’on envoie 100% de la tension au moteur mais en inversant la polarité (+ et -) donc le moteur 2 tournera à l’envers
 ![image](https://github.com/user-attachments/assets/82b964f9-1e23-4cf2-b668-3209fee022f3)
 
Ce bloc enverra 25% de la tension au moteur 1 donc le moteur 1 tournera 4 fois moins vite que sa vitesse max.

### Alimentation de la carte
![image](https://github.com/user-attachments/assets/f4e19615-58b4-4ac4-ba33-c0d53b4e1430) ![image](https://github.com/user-attachments/assets/7a7f3803-5226-4e3b-b188-bd5b1ffc4bce)

#### Via la carte Arduino
Le connecteur Grove (blanc) apporte la tension de l’Arduino à la carte moteur. La tension de l’Arduino transmise par le connecteur Grove est sélectionnée avec un interrupteur : Pour alimenter des moteurs, il faut le mettre en position 5V
Pour utiliser cette tension pour faire tourner les moteurs, il faut mettre un cavalier sur les broches encadrées en rouge.
Avantages : Simplicité d’utilisation, moins de composants nécessaires
Inconvénients : Tension limité, courant limité

#### Via le bornier à vis
Si on a besoin de tension plus élevées (pour aller plus vite) ou de courant plus élevé (pour avoir plus de couple) il est nécessaire d’utiliser le bornier à vis avec une source d’alimentation annexe. (encadré en vert)

#### Via la broche Vin
Cette alimentation est situationnelle et nécessite de prendre certaines précautions car il peut y avoir une dégradation de la carte Arduino, demander au professeur si aucune des 2 autres méthodes ne vous convient

# gyro-accéléromètre
Comme il se branche en I2C, il nécessite d'être initialisé au démarage :

![image](https://github.com/user-attachments/assets/a26957c2-74bf-4557-9911-0351729e260a)

Ce capteur permet de mesurer des angles dans l'espace :

![image](https://github.com/user-attachments/assets/81ee5787-ee1f-4473-a88b-94bef90e9548)

Le tangage et le roulis sont mesurés par rapport au sol (utilisation de la gravité). Pour la mesure du lacet, le gyroscope mesure la vitesse de rotation (en °/s) autours des 3 axes. En échantillonnant le signal il est donc possible de remonter à un angle de rotation.
Par contre cette méthode n’est pas précise dans la durée et doit être réservée pour mesurer le lacet sur une seule rotation, un relevé permanent des valeurs va forcément glisser jusqu’à être complètement faussé.
Concernant les blocs, la valeur du lacet ne peut être utilisée que à l'interieur de la répétition associée :

![image](https://github.com/user-attachments/assets/6fd9d7f5-c433-40fa-935d-cb499b76a74d)


# Capteur de couleur 
Comme il se branche en I2C, il nécessite d'être initialisé au démarage :

![image](https://github.com/user-attachments/assets/c2ad1efd-27c1-42e5-a5df-91bb7b386df7)

Les données renvoyée par le capteur de couleur dépendent de la distance entre le capteur et l'objet à détecter, de la composition de la couleur ainsi que de la luminosité ambiante. Ainsi il est nécessaire de réaliser un premier étalonnage des couleurs à détecter :
1) afficher les données brutes RVB du capteur sur un écran. 
![image](https://github.com/user-attachments/assets/073552a0-6d46-4326-ac4e-19884953b5bf)

2) Procéder à une détection de la couleur dans les conditions finales d'utilisation du capteur (distance, luminosité)
3) recopier les valeurs lues dans les blocs d'étalonnage. 
![image](https://github.com/user-attachments/assets/8fdd2ccd-08e5-4a6f-9db6-67d62a24d8cb)

4) Les blocs de détection peuvent maintenant être utilisés car ils vont comparer les valeurs lue par le capteur aux valeurs des couleurs initialisées, celle qui a l'écart le plus faible sera la couleur détectée.
![image](https://github.com/user-attachments/assets/9d7c198d-3144-4d6a-a16e-52be129283b7)


# Encodeur
Un encodeur sert à mesurer l'angle de rotation d'un moteur.
Il faut rechercher dans la documentation de l'encodeur son nombre d'impulsions pour 1 tour.
Lorsque le capteur renverra cette valeur c'est que le moteur aura fait 1 tour.
Si le moteur est équipé d'une transmission, il faut prendre en compte le rapport de transmission.
ex : l'encodeur du petit moteur effectue 17 impulsions par tour. Il est équipé d'un réducteur 298:1 sur lequel est fixé une roue.
Si on veux que la roue avance de 10 tours, il faut que l'encodeur envoie (17x10x298) 50 660 impulsions 
