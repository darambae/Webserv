import os
import json

def load_data():
    file_path = os.path.realpath("data/record.json")
    with open(file_path, "r") as file:
        data = json.load(file)
    return data

def handle_request:
    data = load_data()
    print("Content-Type: text/html")
    print()
    print("<!DOCTYPE html>")
    print("<html><body>")
    if data:
        num_game = data.get("games").size()
        print(f"<h4>Number of registered game : {num_game}</h4>")
        print("<h4>Registered player list</h4>")
        num_player = data.get("players").size()
        for player in data.get("players") :
            print(f"<p>Player Name(id) : {player.get("player_name")}({player.key()})</p>")
    else
        print("<h1>No game data found</h1>")
    print("</body></html>")
    return

if __name__ == "__main__":
    handle_request()
