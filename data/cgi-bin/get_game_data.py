#!/usr/bin/python3

import json
import cgi
import os
import matplotlib.pyplot as plt

def handle_request():
    form = cgi.FieldStorage()
    game_id = int(form.getvalue("game_id"))
    file_path = os.path.realpath("record.json")
    with open(file_path, "r") as file:
        data = json.load(file)
    game_data = data["games"][game_id - 1].get("players")
    player_data = data["players"]
    
    player_names = []
    total_scores = []
    if game_data:
        for player in player_data:
            if int(player) <= len(game_data) and game_data[int(player) - 1]:
                player_names.append((player_data.get(player)).get("player_name"))
                total_scores.append(game_data[int(player) - 1].get("total_score"))
    
    x_pos = range(len(player_names))
    plt.bar(x_pos, total_scores, color='skyblue')
    plt.ylabel('Score')
    plt.title(f'Game {game_id} scores')
    plt.xticks(x_pos, player_names) # Set label locations.
    

if __name__ == "__main__":
    handle_request()