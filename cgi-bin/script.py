import json
import cgi
import matplotlib.pyplot as plt
import numpy as np
from io import BytesIO
import os


# PSEUDO CODE
# 1. get_player(id)


# def get_all_players():
# form = cgi.FieldStorage()
# player_id = form.getlist("player_id")

def get_player(player_id): 
    file_path = os.path.realpath("record.json")
    with open(file_path, "r") as file:
        data = json.load(file)
    player_data = data["players"].get(str(player_id))
    if player_data:
        player_scores = [game["total_score"] for game in player_data["game_history"]]
        game_ids = [game["game_id"] for game in player_data["game_history"]]
    else:
        print(f"No player with ID {player_id} found")
        return

    average_scores = []
    winner_scores = []
    for game_id in game_ids:
        #now_score = for player_id in player_ids  game_id["players"][player_id]
        game = data["games"][game_id - 1]
        #if game["total_score"] >= now_score:
        total_score = sum(player["total_score"] for player in game["players"])
        num_players = len(game["players"])
        average_scores.append(total_score / num_players)
    
    x_pos = range(len(game_ids))
    plt.scatter(x_pos, average_scores, label='Average Score', color='gray') 
    plt.bar(x_pos, player_scores, label=f'{player_data["player_name"]} Scores', color='skyblue', alpha=0.6)
    
    plt.xlabel('nth game')
    plt.ylabel('Score')
    plt.title(f'{player_data["player_name"]}\' scores vs average score of the game')
    
    plt.xticks(x_pos, game_ids) # Set label locations.
    plt.legend()
    plt.show()
    # img_stream = BytesIO()
    # plt.savefig(img_stream, format='png')
    # img_stream.seek(0)
    # plt.close()
    # print("Content-Type: image/png\n")
    # print(img_stream.read())
    # img_stream.close()
    # return img_stream

get_player(1)


