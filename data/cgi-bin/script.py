import json
import cgi
import matplotlib.pyplot as plt
import numpy as np
from io import BytesIO
import os


# PSEUDO CODE
# 1. get_player(id)
# 2. get_game(id)
# 3. get_num_players()
# 4. get_num_games()

# def get_all_players():
form = cgi.FieldStorage()
player_id = form.getlist("player_id")
print(player_id)
def get_num_players():
    file_path = os.path.realpath("record.json")
    with open(file_path, "r") as file:
        data = json.load(file)
    return len(data["players"])

def get_num_games():
    file_path = os.path.realpath("record.json")
    with open(file_path, "r") as file:
        data = json.load(file)
    return len(data["games"])

def get_game(game_id):
    file_path = os.path.realpath("record.json")
    with open(file_path, "r") as file:
        data = json.load(file) 
    game_data = data["games"][game_id - 1].get("players") # Get the game data
    player_data = data["players"] # Get the players data
    #print(game_data)
    #print(player_data)
    player_names = []
    total_scores = []
    if game_data:
        for player in player_data:
            if int(player) <= len(game_data) and game_data[int(player) - 1]:
                #print((player_data.get(player)).get("player_name"))
                player_names.append((player_data.get(player)).get("player_name"))
                total_scores.append(game_data[int(player) - 1].get("total_score"))
    print("Player Names: ", player_names)
    x_pos = range(len(player_names))
    plt.bar(x_pos, total_scores, color='skyblue')
    plt.ylabel('Score')
    plt.title(f'Game {game_id} scores')
    plt.xticks(x_pos, player_names) # Set label locations.
    plt.show()

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
    # img_stream = BytesIO()
    # plt.savefig(img_stream, format='png')
    # img_stream.seek(0)
    # plt.close()
    # print("Content-Type: image/png\n")
    # print(img_stream.read())
    # img_stream.close()
    # return img_stream
get_game(3)
#get_player(3)


