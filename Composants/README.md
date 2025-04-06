_**Pour réaliser des modifications de l'extension, il est impératif d'utiliser les librairies fournies car plusieurs ont été modifiées afin de fonctionner avec mblock suite à plusieurs bugs découverts lors des essais.**_
# Composants pris en charge
![image](https://github.com/user-attachments/assets/0b320a84-faf4-4cc7-92fe-e13d6deb435e)

# Télémètre ultrason
Si on met plusieurs blocs de mesures de distance trop rapprochés, un bug peut apparaître :
![image](https://github.com/user-attachments/assets/9bc026dd-af62-46a3-9e95-f89095e93568)
Je pense que c'est du au fait que le programme fait relancer un 2ème signal ultrasonnique au capteur avant que le premier lui soit revenu (ou en même temps) ce qui perturbe la mesure.
Pour corriger ce défaut (et faire un code plus propre) il suffit de stocker la valeur du capteur dans une variable et de faire les tests sur cette variable :
![image](https://github.com/user-attachments/assets/1f82acd4-3911-4cc3-be0e-1eaf01e90a46)

#Servomoteur
Le servomoteur ne peut s'orienter qu'entre 0° et 180°. les angles inférieur à 0 sont considérés comme nul, les angles supérieur à 180 sont considérés comme égaux à 180° : https://github.com/arduino-libraries/Servo/blob/master/src/avr/Servo.cpp
