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
        player_id = int(query_string.split("=")[1])
    else:
        response_body.append(f"<h1>Player id is not given in the query</h1><a href='/'>Go back</a></body></html>")
        response_body = "\n".join(response_body)
        response = f"Content-Type: text/html\r\nContent-Length: {len(response_body)}\r\n\r\n{response_body}"
        print(response)
        return
    
    file_path = os.path.realpath("data/cgi-bin/record.json")
    
    with open(file_path, "r") as file:
        data = json.load(file)
    player_data = data["players"].get(str(player_id))
    if player_data:
        player_scores = [game["total_score"] for game in player_data["game_history"]]
        game_ids = [game["game_id"] for game in player_data["game_history"]]
    else:
        response_body.append(f"<h1>No player with ID {player_id} found</h1><a href='/'>Go back</a></body></html>")
        response_body = "\n".join(response_body)
        response = f"Content-Type: text/html\r\nContent-Length: {len(response_body)}\r\n\r\n{response_body}"
        print(response)
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

    # Save the plot to a BytesIO object
    img_buffer = BytesIO()
    plt.savefig(img_buffer, format='png')
    img_buffer.seek(0)

    # Encode the image in base64
    img_base64 = base64.b64encode(img_buffer.read()).decode('utf-8')
    img_buffer.close()

    # Generate the HTML response
    response_body.append(f"<h1>{player_data['player_name']}'s game history</h1>")
    response_body.append(f'<div><img src="data:image/png;base64,{img_base64}" alt="Player Scores Graph"/></div>')
    response_body.append("<a href='/'>Go back</a>")
    response_body.append("</body></html>")
    response_body = "\n".join(response_body)

    response = f"Content-Type: text/html\r\nContent-Length: {len(response_body)}\r\n\r\n{response_body}"
    print(response)

if __name__ == "__main__":
    handle_request()