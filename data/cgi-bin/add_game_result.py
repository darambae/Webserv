#!/usr/bin/python3

import json
import cgi
import os

def load_data():
    file_path = os.path.realpath("record.json")
    with open(file_path, "r") as file:
        data = json.load(file)
    return data

def save_data(data):
    file_path = os.path.realpath("record.json")
    with open(file_path, "w") as file:
        json.dump(data, file, indent=4)

def handle_request():
    form = cgi.FieldStorage()
    game_id = int(form.getvalue("game_id"))
    player_id = int(form.getvalue("player_id"))
    sq_1 = int(form.getvalue("sq_1"))
    sq_2 = int(form.getvalue("sq_2"))
    sq_3 = int(form.getvalue("sq_3"))
    sq_4 = int(form.getvalue("sq_4"))
    sq_5 = int(form.getvalue("sq_5"))
    sq_6 = int(form.getvalue("sq_6"))
    sq_7 = int(form.getvalue("sq_7"))
    sq_8 = int(form.getvalue("sq_8"))
    sq_9 = int(form.getvalue("sq_9"))
    keys = int(form.getvalue("keys"))

    total_score = sq_1 + sq_2 + sq_3 + sq_4 + sq_5 + sq_6 + sq_7 + sq_8 + sq_9 + keys

    data = load_data()

    # Find the game or create a new one
    game = next((g for g in data["games"] if g["game_id"] == game_id), None)
    if not game:
        game = {"game_id": game_id, "players": []}
        data["games"].append(game)

    # Find the player or create a new one
    player = next((p for p in game["players"] if p["player_id"] == player_id), None)
    if not player:
        player = {"player_id": player_id}
        game["players"].append(player)

    # Update player data
    player.update({
        "sq_1": sq_1,
        "sq_2": sq_2,
        "sq_3": sq_3,
        "sq_4": sq_4,
        "sq_5": sq_5,
        "sq_6": sq_6,
        "sq_7": sq_7,
        "sq_8": sq_8,
        "sq_9": sq_9,
        "keys": keys,
        "total_score": total_score
    })

    # Update player's game history
    player_data = data["players"].get(str(player_id))
    if not player_data:
        player_data = {"player_name": f"Player {player_id}", "profile_pic": "", "game_history": []}
        data["players"][str(player_id)] = player_data

    game_history = next((gh for gh in player_data["game_history"] if gh["game_id"] == game_id), None)
    if not game_history:
        game_history = {"game_id": game_id}
        player_data["game_history"].append(game_history)

    game_history["total_score"] = total_score

    save_data(data)

    print("Content-Type: text/html")
    print()
    print("<html><body>")
    print("<h1>Game result added successfully!</h1>")
    print("<a href='/'>Go back</a>")
    print("</body></html>")

if __name__ == "__main__":
    handle_request()