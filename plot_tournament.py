import csv
from matplotlib import pyplot as plt

with open("tournament_data/black.csv", "r") as csvfile:
    black_wins = list(csv.reader(csvfile))

with open("tournament_data/white.csv", "r") as csvfile:
    white_wins = list(csv.reader(csvfile))
    
black_wins = [list(map(int, row)) for row in black_wins]
white_wins = [list(map(int, row)) for row in white_wins]

with open("tournament_data/header.csv", "r") as csvfile:
    players = list(csv.reader(csvfile))[0]

n = len(players)

# Sanitize data for computing win %
for i in range(n):
    for j in range(n):
        if white_wins[i][j] == 0 and black_wins[i][j] == 0:
            white_wins[i][j] += 1
            black_wins[i][j] += 1

winrates = [[white_wins[j][i] / (white_wins[j][i] + black_wins[j][i]) for i in range(n)] for j in range(n)]

# Plotting a heatmap, modifying matplotlib example 
# https://matplotlib.org/3.1.1/gallery/images_contours_and_fields/image_annotated_heatmap.html

fig, ax = plt.subplots()

im = ax.imshow(winrates, cmap="gray")

# We want to show all ticks...
ax.set_xticks(range(n))
ax.set_yticks(range(n))
# ... and label them with the respective list entries
ax.set_xticklabels(players)
ax.set_yticklabels(players)

ax.set_ylabel("Black player")
ax.set_xlabel("White player")

ax.xaxis.tick_top()
ax.xaxis.set_label_position("top")

# Rotate the tick labels and set their alignment.
plt.setp(ax.get_xticklabels(), rotation=-45, ha="right", rotation_mode="anchor")

# Loop over data dimensions and create text annotations.
for i in range(n):
    for j in range(n):
        black_win_pc = 100 - int(winrates[i][j] * 100)
        c = "w" if black_win_pc > 50 else "k"
        text = ax.text(j, i, "%d%%" % black_win_pc,
                       ha="center", va="center", color=c)

ax.set_title("")
fig.tight_layout()

plt.show()
