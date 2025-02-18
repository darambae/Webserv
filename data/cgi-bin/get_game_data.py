
import json
import os
import matplotlib.pyplot as plt
import base64
from io import BytesIO

def handle_request():
    game_id = os.environ.get("QUERY_STRING").split("=")[1]
    print(game_id)
    file_path = os.path.realpath("data/record.json")
    print(file_path)
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
    plt.legend()

    # Save the plot to a BytesIO object
    img_buffer = BytesIO()
    plt.savefig(img_buffer, format='png')
    img_buffer.seek(0)

    # Encode the image in base64
    img_base64 = base64.b64encode(img_buffer.read()).decode('utf-8')
    img_buffer.close()

    # Generate the HTML response
    print("Content-Type: text/html")
    print()
    print("<!DOCTYPE html>")
    print("<html><body>")
    print(f"<h1>Game Data for {game_id}</h1>")
    print(f'<img src="data:image/png;base64,{img_base64}">')
    print("</body></html>")
    return


if __name__ == "__main__":
    handle_request()