#!/usr/bin/python3.7

import csv
import subprocess

contestants = [
    (1,"Random"),
    (2,"Greedy"),
    (3,"Generous"),
    (4,"Sampling greedy (10)"),
    (5,"Sampling greedy (100)"),
 #   (6,"Sampling greedy (1000)"),
    (7,"UCT (10)"),
    (8,"UCT (100)"),
 #   (9,"UCT (1000)"),
    (10,"UCB1 (10)"),
    (11,"UCB1 (100)"),
 #   (12,"UCB1 (1000)"),
 #   (13,"MiniMax sampling (10)"),
    (14,"MiniMax (3)"),
    (14,"MiniMax (4)"),
    (14,"MiniMax (5)")
]

match_rounds = 100

black_wins = [[0] * len(contestants) for i in range(len(contestants))]
white_wins = [[0] * len(contestants) for i in range(len(contestants))]

match_count = 0
for i, (p1_id, p1_name) in enumerate(contestants):
    for j, (p2_id, p2_name) in enumerate(contestants):
        match_count += 1
        print("Now playing: \"%s\" vs \"%s\" (%d of %d)" % 
            (p1_name, p2_name, match_count, len(contestants)**2))
        process_result = subprocess.run(["./reversi", str(p1_id), str(p2_id), str(match_rounds), '0'], 
            capture_output=True,
            encoding="ASCII")
        output = process_result.stdout.strip().split("\n")
        p1_wins_line = output[-2]
        p2_wins_line = output[-1]

        black_result = int(p1_wins_line.strip().split()[-1])
        white_result = int(p2_wins_line.strip().split()[-1])
                
        black_wins[i][j] = black_result 
        white_wins[i][j] = white_result

subprocess.run(["mkdir", "-p", "tournament_data"])

with open("tournament_data/black.csv", "w") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerows(black_wins)

with open("tournament_data/white.csv", "w") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerows(white_wins)

with open("tournament_data/header.csv", "w") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow([s for (_,s) in contestants])