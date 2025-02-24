import json
import os
import sys

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
    # form = os.environ.get("QUERY_STRING")
    content_len = int(os.environ.get("CONTENT_LENGTH", 0))
    form = sys.stdin.read(content_len)
    response_body = []

    response_body.append("<!DOCTYPE html>")
    response_body.append("<html><body>")
    
    if not form:
        response_body.append("<h1>Invalid request</h1>")
        response_body.append("<a href='/'>Go back</a>")
        response_body.append("</body></html>")
        response_body = "\n".join(response_body)
        response = f"Content-Type: text/html\r\nContent-Length: {len(response_body)}\r\n\r\n{response_body}"
        print(response)
        return

    form_data = dict(qc.split("=") for qc in form.split("&"))
    game_id = int(form_data.get("game_id"))
    player_id = int(form_data.get("player_id"))
    sq_1 = int(form_data.get("sq_1"))
    sq_2 = int(form_data.get("sq_2"))
    sq_3 = int(form_data.get("sq_3"))
    sq_4 = int(form_data.get("sq_4"))
    sq_5 = int(form_data.get("sq_5"))
    sq_6 = int(form_data.get("sq_6"))
    sq_7 = int(form_data.get("sq_7"))
    sq_8 = int(form_data.get("sq_8"))
    sq_9 = int(form_data.get("sq_9"))
    keys = int(form_data.get("keys"))

    total_score = sq_1 + sq_2 + sq_3 + sq_4 + sq_5 + sq_6 + sq_7 + sq_8 + sq_9 + keys

    data = load_data()

    # Find the game or create a new one
    game = next((g for g in data["games"] if g["game_id"] == game_id), None)
    if not game:
        game = {"game_id": game_id, "players": []}
        data["games"].append(game)

    # Find the player or create a new one
    if len(game["players"]) >= 6:
        response_body.append("<h4>This game has already enough players</h4>")
        response_body.append("<a href='/'>Go back</a>")
        response_body.append("</body></html>")
        response_body = "\n".join(response_body)
        response = f"Content-Type: text/html\r\nContent-Length: {len(response_body)}\r\n\r\n{response_body}"
        print(response)
        return

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

    response_body.append("<h1>Game result added successfully!</h1>")
    response_body.append("<a href='/'>Go back</a>")
    response_body.append("</body></html>")
    response_body = "\n".join(response_body)

    response = f"Content-Type: text/html\r\nContent-Length: {len(response_body)}\r\n\r\n{response_body}"
    print(response)

if __name__ == "__main__":
    handle_request()