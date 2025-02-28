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
		$content .= ' are the winners with a score of ' . '<span>' . htmlspecialchars($total_max) . '</span> <img src="/cgi-bin/trophee.png" alt="Trophée" width="40" class="trophy"></div>';
	} else {
		$content .= '<div class="winner">🥇 ' . htmlspecialchars(($winner)) . ' is the winner with a score of ' . '<span>' . htmlspecialchars($total_max) . '</span> <img src="/cgi-bin/trophee.png" alt="Trophée" width="40" class="trophy"></div>';
	}
	$content .= '</section>
	</main>

	</body>
	</html>';
}
// Vérification que le formulaire a été soumis
if ($_SERVER['REQUEST_METHOD'] == 'POST' && $_SERVER['CONTENT_LENGTH'] > 0) {
    // Récupérer les données envoyées par le formulaire pour chaque joueur
	$content_length = (int) $_SERVER['CONTENT_LENGTH']; // Taille des données POST
    $post_data = "";

    // Lire STDIN en boucle jusqu'à recevoir toutes les données
    $stdin = fopen("php://stdin", "r");
    while (strlen($post_data) < $content_length) {
        $post_data .= fread($stdin, 1024); // Lire 1 Ko à la fois
    }
    fclose($stdin);

	// echo "data from parent : ";
	// print_r($post_data);
	if (strlen($post_data) > 0) {
		$data = extractDataFromBody($post_data);
		$content = "";
		fillReturnHead($content);
		fillReturnBody($content, $data[1], $data[0]);
		$length = strlen($content);
		echo "Content-Type: text/html\r\n";
		echo "Content-Length: $length\r\n\r\n";
		echo $content;
	}
} else {
    // Si l'utilisateur tente d'accéder directement à la page sans soumettre le formulaire
    	header("HTTP/1.1 405 Method Not Allowed");
    // echo "Méthode non autorisée. Veuillez soumettre un formulaire.";
    	exit;
}
?>
