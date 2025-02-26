<?php
// Vérification que le formulaire a été soumis
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    // Récupérer les données envoyées par le formulaire pour chaque joueur
    $player1 = $_POST['player1'] ?? [];
    $player2 = $_POST['player2'] ?? [];
    $player3 = $_POST['player3'] ?? [];
    $player4 = $_POST['player4'] ?? [];
    $player5 = $_POST['player5'] ?? [];

    // Afficher les scores pour chaque joueur (vous pouvez remplacer cette partie par des calculs)
    echo "<h2>Scores</h2>";
    echo "<h3>Player 1</h3>";
    echo "<ul>";
    foreach ($player1 as $score) {
        echo "<li>" . htmlspecialchars($score) . "</li>";
    }
    echo "</ul>";

    echo "<h3>Player 2</h3>";
    echo "<ul>";
    foreach ($player2 as $score) {
        echo "<li>" . htmlspecialchars($score) . "</li>";
    }
    echo "</ul>";
    // Faites de même pour les autres joueurs si nécessaire
}
?>
