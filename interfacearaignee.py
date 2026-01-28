import tkinter as tk
from tkinter import messagebox

# --- Configuration des Couleurs et Polices (Thème Dark/Moderne) ---
COLOR_BG_MAIN = "#2c3e50"      # Bleu nuit foncé pour le fond
COLOR_BG_FRAME = "#34495e"     # Bleu un peu plus clair pour les zones
COLOR_TEXT_MAIN = "#ecf0f1"    # Blanc cassé pour le texte
COLOR_ACCENT = "#3498db"       # Bleu clair pour les actions standard
COLOR_SUCCESS = "#2ecc71"      # Vert pour Avancer/Connecté
COLOR_WARNING = "#e67e22"      # Orange pour Reculer
COLOR_DANGER = "#e74c3c"       # Rouge pour Stop/Déconnecté

FONT_HEADER = ("Helvetica", 16, "bold")
FONT_BTN_MAIN = ("Helvetica", 12, "bold")
FONT_STATUS = ("Helvetica", 14)

class RobotInterfacePro:
    def __init__(self, root):
        self.root = root
        self.root.title("Interface Robot Pro")
        # Agrandissement de la fenêtre
        self.root.geometry("500x650")
        self.root.configure(bg=COLOR_BG_MAIN)

        self.is_connected = False

        # --- HEADER : Statut Bluetooth ---
        # Utilisation d'une Frame avec une couleur de fond distincte
        self.header_frame = tk.Frame(root, bg=COLOR_BG_FRAME, pady=25, padx=20)
        self.header_frame.pack(fill=tk.X, pady=(0, 20)) # Marge en bas

        self.lbl_status_title = tk.Label(self.header_frame, text="Bluetooth :", font=FONT_HEADER, bg=COLOR_BG_FRAME, fg=COLOR_TEXT_MAIN)
        self.lbl_status_title.pack(side=tk.LEFT)

        self.lbl_status_indicator = tk.Label(self.header_frame, text="DÉCONNECTÉ ✕", font=FONT_HEADER, fg=COLOR_DANGER, bg=COLOR_BG_FRAME, width=15)
        self.lbl_status_indicator.pack(side=tk.LEFT, padx=10)

        # Bouton de connexion stylisé
        self.btn_connect = tk.Button(self.header_frame, text="CONNEXION", command=self.toggle_connection,
                                     bg=COLOR_ACCENT, fg="white", font=("Helvetica", 11, "bold"),
                                     relief=tk.FLAT, bd=0, padx=15, pady=8, activebackground="#2980b9", cursor="hand2")
        self.btn_connect.pack(side=tk.RIGHT)


        # --- CORPS : Commandes de pilotage ---
        self.control_frame = tk.Frame(root, bg=COLOR_BG_MAIN, pady=20)
        self.control_frame.pack(expand=True)

        # Paramètres communs pour les gros boutons de direction
        btn_params = {
            'width': 12,
            'height': 3,
            'font': FONT_BTN_MAIN,
            'fg': 'white',
            'relief': tk.FLAT,
            'bd': 0,
            'cursor': "hand2", # Change le curseur en main au survol
            # 'activeforeground': 'white' # Garde le texte blanc au clic
        }

        # Nous ajoutons des flèches Unicode (↑↓←→) au texte pour le visuel
        # AVANCER (Haut) - Vert
        self.btn_avancer = tk.Button(self.control_frame, text="AVANCER\n↑", bg=COLOR_SUCCESS, activebackground="#27ae60",
                                     command=lambda: self.send_command("F"), **btn_params)
        self.btn_avancer.grid(row=0, column=1, padx=10, pady=10)

        # GAUCHE - Bleu
        self.btn_gauche = tk.Button(self.control_frame, text="GAUCHE\n←", bg=COLOR_ACCENT, activebackground="#2980b9",
                                    command=lambda: self.send_command("L"), **btn_params)
        self.btn_gauche.grid(row=1, column=0, padx=10, pady=10)

        # STOP (Centre) - Rouge vif, un peu plus gros
        self.btn_stop = tk.Button(self.control_frame, text="STOP\n■", bg=COLOR_DANGER, activebackground="#c0392b",
                                  command=lambda: self.send_command("S"), **btn_params)
        self.btn_stop.grid(row=1, column=1, padx=10, pady=10)

        # DROITE - Bleu
        self.btn_droite = tk.Button(self.control_frame, text="DROITE\n→", bg=COLOR_ACCENT, activebackground="#2980b9",
                                    command=lambda: self.send_command("R"), **btn_params)
        self.btn_droite.grid(row=1, column=2, padx=10, pady=10)

        # RECULER (Bas) - Orange
        self.btn_reculer = tk.Button(self.control_frame, text="RECULER\n↓", bg=COLOR_WARNING, activebackground="#d35400",
                                     command=lambda: self.send_command("B"), **btn_params)
        self.btn_reculer.grid(row=2, column=1, padx=10, pady=10)


        # --- FOOTER : Logs ---
        # Barre de log en bas, style console
        self.log_label = tk.Label(root, text="> Prêt. En attente de connexion...",
                                  font=("Consolas", 10), bg=COLOR_BG_FRAME, fg="#bdc3c7",
                                  anchor=tk.W, padx=10, pady=8, relief=tk.FLAT)
        self.log_label.pack(side=tk.BOTTOM, fill=tk.X)

    def toggle_connection(self):
        """Simule la connexion Bluetooth avec retour visuel"""
        if not self.is_connected:
            # Simulation de connexion réussie
            self.is_connected = True
            self.lbl_status_indicator.config(text="CONNECTÉ ✓", fg=COLOR_SUCCESS)
            self.btn_connect.config(text="DÉCONNEXION", bg=COLOR_DANGER, activebackground="#c0392b")
            self.update_log(">> Système connecté au module Bluetooth.")
        else:
            # Simulation de déconnexion
            self.is_connected = False
            self.lbl_status_indicator.config(text="DÉCONNECTÉ ✕", fg=COLOR_DANGER)
            self.btn_connect.config(text="CONNEXION", bg=COLOR_ACCENT, activebackground="#2980b9")
            self.update_log(">> Système déconnecté.")

    def send_command(self, cmd):
        """Gère l'envoi de commande et la sécurité"""
        if not self.is_connected:
            # On fait clignoter le statut en rouge si on essaie sans connexion
            current_color = self.lbl_status_indicator.cget("fg")
            self.lbl_status_indicator.config(fg="white", bg=COLOR_DANGER)
            self.root.after(200, lambda: self.lbl_status_indicator.config(fg=current_color, bg=COLOR_BG_FRAME))
            self.update_log("! ERREUR : connexion requise pour envoyer des commandes.")
            return

        action = ""
        if cmd == "F": action = "Mouvement : AVANCER"
        elif cmd == "B": action = "Mouvement : RECULER"
        elif cmd == "L": action = "Rotation : GAUCHE"
        elif cmd == "R": action = "Rotation : DROITE"
        elif cmd == "S": action = "Action : ARRÊT D'URGENCE"

        # --> Insérer ici le code réel d'envoi Bluetooth <--

        self.update_log(f"> Commande envoyée : [{cmd}] - {action}")

    def update_log(self, message):
        self.log_label.config(text=message)

if __name__ == "__main__":
    root = tk.Tk()
    # Optionnel : Tenter de définir une icône de fenêtre (nécessite un fichier .ico ou .png)
    # try: root.iconbitmap('robot_icon.ico')
    # except: pass
    app = RobotInterfacePro(root)
    root.mainloop()