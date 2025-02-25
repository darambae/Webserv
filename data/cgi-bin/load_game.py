import os
import json

def load_data():
    file_path = os.path.realpath("data/cgi-bin/record.json")
    print(file_path)
    with open(file_path, "r") as file:
        data = json.load(file)
    return data

def handle_request():
    data = load_data()
    res_body = []

    res_body.append("<!DOCTYPE html>")
    res_body.append("<html><body>")
    if data:
        num_game = len(data.get("games", []))
        res_body.append(f"<h4>Number of registered games: {num_game}</h4>")
        res_body.append("<h4>Registered player list</h4>")
        num_player = len(data.get("players", {}))
        for player_id, player_info in data.get("players", {}).items():
            player_name = player_info.get("player_name", "Unknown")
            res_body.append(f"<p>Player Name(id): {player_name} ({player_id})</p>")
    else:
        res_body.append("<h1>No game data found</h1>")
    res_body.append("</body></html>")
    res_body = "\n".join(res_body)
    response = f"Content-Type: text/html\r\nContent-Length: {len(res_body)}\r\n\r\n{res_body}"
    print(response)
    return

if __name__ == "__main__":
    handle_request()