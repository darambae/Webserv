<?php

// Vérification que le formulaire a été soumis
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    // Récupérer les données envoyées par le formulaire pour chaque joueur
	$names = $_POST['name'] ?? [];
	$players = [
		$_POST['player1'] ?? [],
		$_POST['player2'] ?? [],
		$_POST['player3'] ?? [],
		$_POST['player4'] ?? [],
		$_POST['player5'] ?? []
	];
	$content = "<!DOCTYPE html>
	<html>
	<body>
		<h2>Scores</h2>";
    // Affichage des scores
    for ($i = 0; $i < count($players); $i++) {
		$content .= "$names[$i]";
        if (!empty($players[$i]) && !empty($names[$i])) {
            $content .= "<h2>" . htmlspecialchars($names[$i]) . "</h2><ul>";
            foreach ($players[$i] as $score) {
                $content .= "<li>" . htmlspecialchars($score) . "</li>";
            }
            $content .= "</ul>";
        }
    }

	$content .= "</body></html>";
	$length = strlen($content);
	echo "Content-Type: text/html\r\n";
	echo "Content-Length: $length\r\n\r\n";
	echo $content;
} else {
    // Si l'utilisateur tente d'accéder directement à la page sans soumettre le formulaire
    header("HTTP/1.1 405 Method Not Allowed");
    // echo "Méthode non autorisée. Veuillez soumettre un formulaire.";
    exit;
}
?>

