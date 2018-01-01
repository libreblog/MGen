# Main
library = Voxos 2.0 Legato Sections # For which library algorithm is optimized
Type = 3 # Instrument type

# Instrument parameters
n_min = D2 # Lowest note
n_max = D6 # Highest note
t_min = 100 # Shortest note in ms
t_max = 8000 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
poly = 100 # Maximum number of simultaneous voices
#leap_t_min = 100 # Shortest note after leap in ms

# Automation parameters
CC_dynamics = 1
CC_ma = 9 # Number of CC steps to moving average (please use only odd numbers)
CC_steps = 30 # Number of CC steps in one note step (please use only odd numbers)
# CC2 - morph between Ooh and Ah

# Legato adaptor
legato_ahead = 165 # Time in ms to stretch legato notes back to cope with legato delay
max_ahead_note = 12 # Maximum chromatic interval having ahead property

# Nonlegato adaptor
nonlegato_freq = 20 # Frequency (in percent) when legato can be replaced with non-legato by moving note end to the left
nonlegato_minlen = 400 # Minimum note length (in ms) allowed to convert to nonlegato

# Bell adaptor
bell_mindur = 700 # Minimum note duration (ms) that can have a bell
bell_mul = 0.2-0.2 # Multiply dynamics by this parameter at bell start-end
bell_len = 30-30 # Percent of notelength to use for slope at bell start-end

# Reverse bell adaptor
rbell_freq = 80 # Frequency to apply reverse bell when all conditions met
rbell_dur = 300-1000 # Minimum note duration (ms) that can have a reverse bell - that will have deepest reverse bell
rbell_mul = 0.8-0.2 # Multiply dynamics by this parameter at bell center with mindur - with longer dur
rbell_pos = 20-80 # Leftmost-rightmost minimum reverse bell position inside window (percent of window duration)

# Randomization
rnd_vel = 8 # Randomize note velocity not greater than this percent
rnd_dyn = 8 # Randomize step dynamics not greater than this percent
