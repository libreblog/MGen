include "..\include\SI VoR.pl"

# Controls
KswGroup = "C0: Ah p", "C#0: Ah f", "D0: Oo" # Syllable

# Initial setup
InitCommand = "Ah f"

# Instrument parameters
n_min = B2 # Lowest note
n_max = C5 # Highest note
t_min = 100 # Shortest note in ms
t_max = 12000 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms

