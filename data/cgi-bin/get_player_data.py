#!/usr/bin/env python3

import json
import cgi
import os
import matplotlib.pyplot as plt
from io import BytesIO

def handle_request():
    form = cgi.FieldStorage()
    player_id = int(form.getvalue("player_id"))
    file_path = os.path.realpath("record.json")
    with open(file_path, "r") as file:
        data = json.load(file)
    player_data = data["players"].get(str(player_id))
    if player_data:
        player_scores = [game["total_score"] for game in player_data["game_history"]]
        game_ids = [game["game_id"] for game in player_data["game_history"]]
    else:
        print("Content-Type: text/html")
        print()
        print(f"<html><body><h1>No player with ID {player_id} found</h1><a href='/'>Go back</a></body></html>")
        return

    average_scores = []
    max_scores = []
    for game_id in game_ids:
        game = data["games"][game_id - 1]
        max_score = max(player["total_score"] for player in game["players"]) 
        max_scores.append(max_score)        
        total_score = sum(player["total_score"] for player in game["players"])
        num_players = len(game["players"])
        average_scores.append(total_score / num_players)
    
    x_pos = range(len(game_ids))
    plt.scatter(x_pos, max_scores, label='Max Score', color='red')
    plt.scatter(x_pos, average_scores, label='Average Score', color='gray')
    plt.bar(x_pos, player_scores, label=f'{player_data["player_name"]} Scores', color='skyblue', alpha=0.6)
    
    plt.xlabel('nth game')
    plt.ylabel('Score')
    plt.title(f'{player_data["player_name"]}\' scores vs average score of the game')
    
    plt.xticks(x_pos, game_ids) # Set label locations.
    plt.legend()
    plt.show()

if __name__ == "__main__":
    handle_request()