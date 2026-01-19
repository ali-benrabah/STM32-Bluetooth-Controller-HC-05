# Radar de Recul STM32 avec Interface Python Bluetooth

Ce projet implémente un système de détection d'obstacles utilisant un microcontrôleur STM32F4, un capteur ultrason HC-SR04 et une transmission Bluetooth vers une interface graphique PC.

## 🛠 Matériel
* **Carte :** STM32F4 (Discovery/Nucleo)
* **Capteur :** HC-SR04 (Ultrason)
* **Communication :** Module Bluetooth HC-05
* **Actuateurs :** LED (PWM) et Buzzer

## 🔌 Câblage (Pinout)
* **HC-SR04 Trig :** PA9
* **HC-SR04 Echo :** PA8
* **HC-05 RX :** PA2 (USART2 TX)
* **HC-05 TX :** PA3 (USART2 RX)
* **LED :** PA6
* **Buzzer :** PA5

## 💻 Fonctionnalités
1.  **Embarqué (C) :**
    * Mesure de distance précise via Timer.
    * Gestion des états (NORMAL, DETECT, ALERT).
    * Transmission UART via Bluetooth à 9600 bauds.
2.  **Interface (Python/Tkinter) :**
    * Réception des données série en temps réel.
    * Affichage visuel (Code couleur : Vert/Orange/Rouge).

## 🚀 Utilisation
1.  Flasher le code C sur la STM32.
2.  Lancer l'interface : `python3 interface.py`
