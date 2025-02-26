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
    res_body.append("<html><head><style>")
    res_body.append("body, h1, h4, p, ul, li, a, table { margin: 0; padding: 0; box-sizing: border-box; }")
    res_body.append("body { font-family: Arial, sans-serif; line-height: 1.6; background-color: #f4f4f4; color: #333; text-align: center;}")
    res_body.append("table {width: 50%; border-collapse: collapse; margin: 25px 0; font-size: 18px; text-align: left;}")
    res_body.append("th, td {padding: 12px; border-bottom: 1px solid #ddd;}")
    res_body.append("th {background-color: #f2f2f2;}")
    res_body.append("a {padding: 10px 20px; background-color: #007bff; color: #fff; border: none; border-radius: 5px; cursor: pointer; decoration: none;}")
    res_body.append("</style></head><body>")
    res_body.append("<h1>Game History</h1>")
    if data:
        num_game = len(data.get("games", []))
        res_body.append(f"<h4>Number of registered games: {num_game}</h4>")
        res_body.append("<h4>Registered player list</h4>")
        res_body.append("<table>")
        res_body.append("<tr><th>Game ID</th><th>Player Name</th></tr>")
        for player_id, player_info in data.get("players", {}).items():
            player_name = player_info.get("player_name", "Unknown")
            res_body.append(f"<tr><td>{player_id}</td><td>{player_name}</td></tr>")
        res_body.append("</table>")
        res_body.append("<a href='/'>Go back</a>")
    else:
        res_body.append("<h1>No game data found</h1>")
        res_body.append("<a href='/'>Go back</a>")
    res_body.append("</body></html>")
    res_body = "\n".join(res_body)
    response = f"Content-Type: text/html\r\nContent-Length: {len(res_body)}\r\n\r\n{res_body}"
    print(response)
    return

if __name__ == "__main__":
    handle_request()