import json
import os
import matplotlib.pyplot as plt
import base64
from io import BytesIO

def handle_request():

    response_body = []
    response_body.append("<!DOCTYPE html>")
    response_body.append("<html><body><style>")
    response_body.append("body, h1, h2, p, ul, li, a { margin: 0; padding: 0; box-sizing: border-box; }")
    response_body.append("body { font-family: Arial, sans-serif; line-height: 1.6; background-color: #f4f4f4; color: #333; text-align: center;}")
    response_body.append("img {display: block; margin: 0 auto;}")
    response_body.append("a {padding: 10px 20px; background-color: #007bff; color: #fff; border: none; border-radius: 5px; cursor: pointer; decoration: none;}")
    response_body.append("</style>")
    query_string = os.environ.get("QUERY_STRING")
    if query_string:
        game_id = int(query_string.split("=")[1])
    else:
        response_body.append(f"<h1>Game id is not given in the query string</h1><a href='/'>Go back</a></body></html>")
        response_body = "\n".join(response_body)
        response = f"Content-Type: text/html\r\nContent-Length: {len(response_body)}\r\n\r\n{response_body}"
        print(response)
        return
    
    file_path = os.path.realpath("data/cgi-bin/record.json")
    with open(file_path, "r") as file:
        data = json.load(file)
    if (game_id > len(data["games"]) or game_id < 1):
        response_body.append(f"<h1>The biggest id you can give is {len(data['games'])}</h1><a href='/'>Go back</a></body></html>")
        response_body = "\n".join(response_body)
        response = f"Content-Type: text/html\r\nContent-Length: {len(response_body)}\r\n\r\n{response_body}"
        print(response)
        return
    game_data = data["games"][game_id - 1]
    player_data = data["players"]
    
    player_names = []
    total_scores = []

    if game_data:
        for player in game_data.get("players", []):
            player_id = player["player_id"]
            player_name = player_data[str(player_id)]["player_name"]
            player_names.append(player_name)
            total_scores.append(player["total_score"])
    
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
    response_body.append(f"<h1>Result of Game {game_id}</h1>")
    response_body.append(f'<div><img src="data:image/png;base64,{img_base64}"></div>')
    response_body.append("<a href='/'>Go back</a>")
    response_body.append("</body></html>")
    response_body = "\n".join(response_body)

    response = f"Content-Type: text/html\r\nContent-Length: {len(response_body)}\r\n\r\n{response_body}"
    print(response)

if __name__ == "__main__":
    handle_request()