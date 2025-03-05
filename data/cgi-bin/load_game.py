import os
import json
import signal
import sys

def signal_handler(sig, frame):
    sys.exit(0)

def load_data():
    file_path = os.path.realpath("data/cgi-bin/record_copy.json")
    with open(file_path, "r") as file:
        data = json.load(file)
    return data

def handle_request():
    res_body = []

    res_body.append("<!DOCTYPE html>")
    res_body.append("<html><body><section>")
    res_body.append("<link rel='stylesheet' href='style.css'/>")
    res_body.append("<h1>Game History</h1>")
    data = load_data()
    if data:
        num_game = len(data.get("games", []))
        res_body.append(f"<h4>Number of registered games: {num_game}</h4>")
        res_body.append("<h4>Registered player list</h4>")
        res_body.append("<table>")
        res_body.append("<tr><th>Game ID</th><th>Player Name</th></tr>")
        for game_id in data["games"]:
            print(f"Game ID: {game_id}")
            players = data["games"][game_id]
            for player in players:
                player_id = player["player_id"]
                player_name = data["players"][player_id]["player_name"]
                res_body.append(f"<tr><td>{player_id}</td><td>{player_name}</td></tr>")
        res_body.append("</table>")
    else:
        res_body.append("<h1>No game data found</h1>")
    res_body.append("<a href='/' class=\"button\">Go back</a>")
    res_body.append("</section></body></html>")
    res_body = "\n".join(res_body)
    response = f"Content-Type: text/html\r\nContent-Length: {len(res_body)}\r\n\r\n{res_body}"
    print(response)
    return

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    handle_request()