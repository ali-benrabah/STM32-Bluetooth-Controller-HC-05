import serial
import tkinter as tk
from tkinter import ttk
import threading
import time

# --- CONFIGURATION ---
PORT = '/dev/rfcomm0'   # Ton port Bluetooth
BAUD_RATE = 9600        # La vitesse qui marche (validée par ton test screen)

class RadarInterface:
    def __init__(self, root):
        self.root = root
        self.root.title("Interface Radar STM32")
        self.root.geometry("400x350")
        self.root.configure(bg="#2c3e50")

        # Titre
        self.label_title = tk.Label(root, text="ALERTE INTRUSION", font=("Arial", 20, "bold"), bg="#2c3e50", fg="white")
        self.label_title.pack(pady=20)

        # Indicateur Visuel (Cercle)
        self.canvas = tk.Canvas(root, width=150, height=150, bg="#2c3e50", highlightthickness=0)
        self.circle = self.canvas.create_oval(10, 10, 140, 140, fill="grey", outline="white", width=2)
        self.canvas.pack(pady=10)

        # Texte d'état
        self.label_status = tk.Label(root, text="En attente...", font=("Arial", 16), bg="#2c3e50", fg="#bdc3c7")
        self.label_status.pack(pady=20)

        # Bouton Quitter
        self.btn_quit = ttk.Button(root, text="Quitter", command=root.destroy)
        self.btn_quit.pack(pady=5)

        # Lancer la lecture série dans un thread séparé pour ne pas figer l'interface
        self.running = True
        self.thread = threading.Thread(target=self.read_serial)
        self.thread.daemon = True
        self.thread.start()

    def update_ui(self, status):
        # Cette fonction met à jour les couleurs selon le message reçu
        if status == "ALERT":
            self.canvas.itemconfig(self.circle, fill="#e74c3c") # Rouge
            self.label_status.config(text="⚠️ ALERTE !", fg="#e74c3c")
            self.label_title.config(fg="#e74c3c")
            
        elif status == "DETECT":
            self.canvas.itemconfig(self.circle, fill="#f39c12") # Orange
            self.label_status.config(text="👀 Détection...", fg="#f39c12")
            self.label_title.config(fg="white")

        elif status == "NORMAL":
            self.canvas.itemconfig(self.circle, fill="#2ecc71") # Vert
            self.label_status.config(text="✅ Zone Sécurisée", fg="#2ecc71")
            self.label_title.config(fg="white")
            
        elif status == "SYSTEM_READY":
            self.canvas.itemconfig(self.circle, fill="#3498db") # Bleu
            self.label_status.config(text="Système Connecté", fg="#3498db")

    def read_serial(self):
        try:
            ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
            print(f"Connecté à {PORT}")
            
            while self.running:
                if ser.in_waiting > 0:
                    try:
                        # On lit la ligne et on enlève les espaces/retours à la ligne (\r\n)
                        line = ser.readline().decode('utf-8', errors='ignore').strip()
                        
                        if line:
                            print(f"Reçu STM32: {line}") # Affiche dans la console pour debug
                            # On demande à l'interface de se mettre à jour
                            self.root.after(0, self.update_ui, line)
                            
                    except Exception as e:
                        print(f"Erreur lecture: {e}")
                time.sleep(0.05)
                
        except serial.SerialException:
            print(f"❌ Impossible d'ouvrir le port {PORT}")
            self.root.after(0, lambda: self.label_status.config(text="Erreur Connexion", fg="red"))

# --- Lancement ---
if __name__ == "__main__":
    root = tk.Tk()
    app = RadarInterface(root)
    root.mainloop()