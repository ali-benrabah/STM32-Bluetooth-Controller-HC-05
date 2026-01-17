import serial
import time
import tkinter as tk
from tkinter import messagebox

# --- CONFIGURATION ---
PORT_BLUETOOTH = '/dev/rfcomm0'
BAUDRATE = 9600

# --- INITIALISATION SÉRIE ---
try:
    # On ouvre le port série comme avec la commande 'screen'
    ser = serial.Serial(PORT_BLUETOOTH, BAUDRATE, timeout=1)
    print(f"Connecté à {PORT_BLUETOOTH}")
except serial.SerialException:
    print("ERREUR: Impossible de se connecter.")
    print("Vérifie que tu as bien lancé 'sudo rfcomm connect ...' dans un autre terminal.")
    exit()

# --- FONCTIONS DE COMMANDE ---
def allumer_led():
    # On envoie le caractère '1' encodé en bytes (octets)
    ser.write(b'1')
    label_etat.config(text="État : ALLUMÉ", fg="green")
    print("Commande envoyée : 1")

def eteindre_led():
    # On envoie le caractère '0'
    ser.write(b'0')
    label_etat.config(text="État : ÉTEINT", fg="red")
    print("Commande envoyée : 0")

def quitter():
    if ser.is_open:
        ser.close()
    fenetre.destroy()

# --- INTERFACE GRAPHIQUE (GUI) ---
fenetre = tk.Tk()
fenetre.title("Contrôle TorpyBot")
fenetre.geometry("300x250")

# Titre
lbl_titre = tk.Label(fenetre, text="Torpy Control Center", font=("Arial", 16, "bold"))
lbl_titre.pack(pady=10)

# Indicateur d'état
label_etat = tk.Label(fenetre, text="État : INCONNU", font=("Arial", 12))
label_etat.pack(pady=10)

# Bouton ON
btn_on = tk.Button(fenetre, text="LED ON", bg="#4CAF50", fg="white", font=("Arial", 12), width=10, command=allumer_led)
btn_on.pack(pady=5)

# Bouton OFF
btn_off = tk.Button(fenetre, text="LED OFF", bg="#f44336", fg="white", font=("Arial", 12), width=10, command=eteindre_led)
btn_off.pack(pady=5)

# Bouton Quitter
btn_quit = tk.Button(fenetre, text="Quitter", command=quitter)
btn_quit.pack(pady=20)

# Lancement de la boucle principale
fenetre.mainloop()