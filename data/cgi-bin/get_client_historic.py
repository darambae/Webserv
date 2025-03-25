import json
import os
import matplotlib.pyplot as plt
import base64
from io import BytesIO
import signal
import sys

def signal_handler(sig, frame):
    sys.exit(0)
def error_message(message):
    response_body = []
    response_body.append(f"<h1>")
    response_body.append(message)
    response_body.append(f"</h1><a href='/' class=\"button\">Go back</a></body></html>")
    response_body = "\n".join(response_body)
    response = f"Content-Length: {len(response_body)}\r\n\r\n{response_body}"
    print("Status: 200")
    print(response)
    return

def print_gameids(data, game_id):
    game_player = data["games"][str(game_id)]
    player_data = data["players"]

    player_names = []
    total_scores = []

    if game_player:
        for player in game_player:
            player_id = player["player_id"]
            player_name = player_data[player_id]["player_name"]
            player_names.append(player_name)
            total_scores.append(player["total_score"])

    x_pos = range(len(player_names))
    plt.bar(x_pos, total_scores, color='skyblue', label='Scores')
    plt.ylabel('Score')
    plt.title(f'Game {game_id} scores')
    plt.xticks(x_pos, player_names) # Set label locations.
    plt.legend()

    # Save the plot to a BytesIO object
    img_buffer = BytesIO()
    plt.savefig(img_buffer, format='png')
    img_buffer.seek(0)

    # Encode the image in base64
    img_base64 = base64.b64encode(img_buffer.read()).decode('utf-8')
    img_buffer.close()

    return img_base64

def handle_request():

    response_body = []
    response_body.append("<!DOCTYPE html>")
    response_body.append("<html><body>")
    response_body.append("<link rel='stylesheet' href='style.css'/>")
    session_id = os.environ.get("HTTP_COOKIE")
    if session_id:
        cookies_path = os.path.realpath("data/cgi-bin/games_per_sess_id.json")
        with open(cookies_path, "r") as file:
            cookies = json.load(file)
        game_ids = cookies.get(session_id)
        if game_ids == None:
            error_message(f"For session id {session_id}, no history for this client")
            return
        file_path = os.path.realpath("data/cgi-bin/record.json")
        with open(file_path, "r") as file:
            data = json.load(file)
        for game_id in game_ids:
            # if ((int)game_id > len(data["games"]) or game_id < 1):
            #     response_body.append(f"<h1>The biggest id you can give is {len(data['games'])}</h1><a href='/' class=\"button\">Go back</a></body></html>")
            #     response_body = "\n".join(response_body)
            #     response = f"Content-Type: text/html\r\nContent-Length: {len(response_body)}\r\n\r\n{response_body}"
            #     print("Status: 200")
            #     print(response)
            #     return

            # else:
                 # Generate the HTML response
            response_body.append(f"<h1>Result of Game {game_id}</h1>")
            response_body.append(f'<div><img src="data:image/png;base64,{print_gameids(data, game_id)}"></div>')

    else:
        error_message("no session ID found, you have to accept cookies")
        return
        # response_body.append(f"<h1>Input Error</h1><a href='/' class=\"button\">Go back</a></body></html>")
        # response_body = "\n".join(response_body)
        # response = f"Content-Type: text/html\r\nContent-Length: {len(response_body)}\r\n\r\n{response_body}"

    response_body.append("<a href='/' class=\"button\">Go back</a>")
    response_body.append("</body></html>")
    response_body = "\n".join(response_body)

    response = f"Content-Length: {len(response_body)}\r\n\r\n{response_body}"
    print("Status: 200")
    print(response)
    exit(0)
    return

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    handle_request()
