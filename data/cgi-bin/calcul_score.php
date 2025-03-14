<?php
/*
tout ce qui est en dehors des balise (<?php et ?>)est affiché dans la sortie standard
$name, représente une variable
$name = 1; ou $name="marc"; ou $name = 'marc';
echo $name; pour l'afficher dans la stdout
$note = 10; $note2 = 12;
echo ($note + $note2)/2; écris 11 dans stdout
$prénom = 'marc'; $nom = 'dupont';
echo $prénom . $nom; pour des string, '.' au lieu de +
'' : affiche tout sans interpréter
"" : interprete les variables et les caractères spéciaux (\n)

*/

function errorCgi($error_number) {
	echo "Status : $error_number";
	exit;
}

function addDataToJson(array $names, array $players) {
	$file = realpath("data/cgi-bin/record.json");
	$json = file_get_contents($file);//récupère les data de JSON
	$data = json_decode($json, true);//true pour json objet = associative array, transforme les data.json en objets php utilisables
	$game_id = (string) ((int) array_key_last($data["games"]) + 1);
	$i = 0;
	foreach ($players as &$player) {
		if (count($player) > 0 && $names[$i]) {
			$total = 0;
			foreach ($player as $sq) {
				$total += (int) $sq;
			}
			$player["player_name"] = $names[$i++];
			$player["total"] = $total;
			foreach ($data["players"] as $player_id => &$player_data) {
				if (strcasecmp($player_data["player_name"], $player["player_name"])  === 0) {
					$player_data["game_history"][] = [
						"game_id" => $game_id,
						"total_score" => $total
					];
					$player["player_id"] = (string) $player_id;
				}
			}
			if (!array_key_exists("player_id", $player)) {
				$player["player_id"] = (string)((int) array_key_last($data["players"]) + 1);
				$data["players"][$player["player_id"]] = [
					"player_name" => $player["player_name"],
					"profile_pic" => "",
					"game_history" => [[
						"game_id" => $game_id,
						"total_score" => $total
					]]
					];
				}
				$data["games"][$game_id][] = [
					"player_id" => $player["player_id"],
					"sq_1" => (int) $player[0],
					"sq_2" => (int) $player[1],
					"sq_3" => (int) $player[2],
					"sq_4" => (int) $player[3],
					"sq_5" => (int) $player[4],
					"sq_6" => (int) $player[5],
					"sq_7" => (int) $player[6],
					"sq_8" => (int) $player[7],
					"sq_9" => (int) $player[8],
					"keys" => (int) $player[9],
					"total_score" => $player["total"]
				];
		}
	}
//	print_r($data);
	$new_json = json_encode($data, JSON_PRETTY_PRINT);
	if (file_put_contents($file,$new_json) === false) {
		$errorMessage = "an error occurs updating datas";
		echo "<p style='color: red; font-weight: bold;'>$errorMessage</p>";
	}
	if (!empty($_SERVER['HTTP_COOKIE'])) {
		$id_client = $_SERVER['HTTP_COOKIE'];
		$cookie_file = realpath("data/cgi-bin/games_per_sess_id.json");
		if ($cookie_file === false || !file_exists($cookie_file)) {
			mkdir($cookie_file, 0777, true); // Création récursive avec permissions
			$cookie_data = [];
		} else {
			$cookie_json = file_get_contents($cookie_file);
			$cookie_data = json_decode($cookie_json, true);
		}
		$cookie_data[$id_client][] = $game_id;
		$new_cookie_json = json_encode($cookie_data, JSON_PRETTY_PRINT);
		if (file_put_contents($cookie_file,$new_cookie_json) === false) {
			$errorMessage = "an error occurs updating datas";
			echo "<p style='color: red; font-weight: bold;'>$errorMessage</p>";
		}
	}
}
function extractDataFromBody($body) {
	parse_str($body, $parsed_postData);
		$names = $parsed_postData['name'] ?? [];
		$players = [
			$parsed_postData['player1'] ?? [],
			$parsed_postData['player2'] ?? [],
			$parsed_postData['player3'] ?? [],
			$parsed_postData['player4'] ?? [],
			$parsed_postData['player5'] ?? []
		];
		$data = [$names, $players];
		return $data;
}

function fillReturnHead(&$content) {
	$content = '<!DOCTYPE html>
		<html lang="fr">
		<head>
			<meta charset="UTF-8">
			<meta name="viewport" content="width=device-width, initial-scale=1.0">
			<title>Scores</title>
			<link rel="stylesheet" href="/cgi-bin/style.css">
		</head>
		<body>

			<header>
				<h1>Scores</h1>
			</header>

			<main>
			<section>';
}

function fillReturnBody(&$content, array $players, array $names) {
	$total_max = 0;
	$winnerS[0] = "";
	$winner = "";
	for ($i = 0; $i < count($players); $i++) {
		if (!empty($players[$i]) && !empty($names[$i])) {
			$content .= '<div class="player">' . htmlspecialchars($names[$i]);
			$total = 0;
			foreach ($players[$i] as $score) {
				$total = $total + $score;
			}
			$content .= '<span>' . htmlspecialchars($total) . '</span></div>';
			if ($total >= $total_max) {
				if ($total === $total_max && strlen($winnerS[0]) > 0) {
					$winnerS[] = $names[$i];
				}
				else {
					$total_max = $total;
					$winner = $names[$i];
					$winnerS[0] = $winner;
				}
			}
		}
	}
	if (count($winnerS) > 1) {
		$content .= '<div class="winner">🥇 ';
		foreach ($winnerS as $name) {
			$content .= ' / ' . htmlspecialchars(($name));
		}
		$content .= ' are the winners with a score of ' . '<span>' . htmlspecialchars($total_max) . '</span> <img src="/cgi-bin/trophee.png" alt="Trophée" width="40" class="trophy"></div>' . "<a href='/' class=\"button\">Go back</a>";
	} else {
		$content .= '<div class="winner">🥇 ' . htmlspecialchars(($winner)) . ' is the winner with a score of ' . '<span>' . htmlspecialchars($total_max) . '🥇' . '</span> <img src="/cgi-bin/trophee.png" alt="Trophée" width="80" class="trophy"></div>' . "<a href='/' class=\"button\">Go back</a>";
	}
	$content .= '</section>
	</main>

	</body>
	</html>';
}
// Vérification que le formulaire a été soumis
if ($_SERVER['REQUEST_METHOD'] == 'POST' && !empty($_SERVER['CONTENT_LENGTH'])) {
    // Récupérer les données envoyées par le formulaire pour chaque joueur
	$content_length = (int) $_SERVER['CONTENT_LENGTH']; // Taille des données POST
    $post_data = "";

    // Lire STDIN en boucle jusqu'à recevoir toutes les données
    $stdin = fopen("php://stdin", "r");
    while (strlen($post_data) < $content_length) {
        $readstr = fread($stdin, 1024); // Lire 1 Ko à la fois
		if ($readstr === false)
			errorCgi("422 Unprocessable Entity");
		$post_data .= $readstr;
    }
    fclose($stdin);

	// echo "data from parent : ";
	// print_r($post_data);
	if (strlen($post_data) > 0) {
		$data = extractDataFromBody($post_data);
		$names = $data[0];
		$scores = $data[1];
		$content = "";
		fillReturnHead($content);
		fillReturnBody($content, $scores, $names);
		$length = strlen($content);
		addDataToJson($names, players: $scores);
		echo "Status: 200\r\n";
		echo "Content-Length: $length\r\n\r\n";
		echo $content;
		exit;
	}
} else {
	errorCgi("400 Bad Request");
}
?>
