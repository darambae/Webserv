function getCookie(name) {
    let match = document.cookie.match(new RegExp('(^| )' + name + '=([^;]+)'));
    return match ? match[2] : null;
}

document.addEventListener("DOMContentLoaded", function () {
    // Vérifie si le cookie "cookie-consent" existe
    if (!getCookie("cookie-consent")) {
        document.getElementById("cookiePopup").style.display = "block";
    }
});

function acceptCookies() {
    document.cookie = "cookie-consent=accept; path=/; expires=Fri, 31 Dec 9999 23:59:59 GMT";
    fetch('/cgi-bin/cookies-consent.py', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: 'consent=accept'
    }).then(response => {
        if (response.ok) {
            document.getElementById("cookiePopup").style.display = "none";
        }
    }).catch(error => {
        console.error('Error while accepting cookies:', error);
    });
}

function declineCookies() {
    document.cookie = "cookie-consent=reject; path=/; expires=Fri, 31 Dec 9999 23:59:59 GMT";

    fetch('/cgi-bin/cookies-consent.py', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: 'consent=reject'
    }).then(response => {
        if (response.ok) {
            document.getElementById("cookiePopup").style.display = "none";
        }
    }).catch(error => {
        console.error('Error while refusing cookies:', error);
    });
}

/* function checkCookies(event) {
    if (getCookie("cookie-consent") !== "accept") {
        event.preventDefault(); // Empêche l'envoi du formulaire
        window.location.href = "/cookiesRequired.html"; // Redirige vers la page expliquant l'importance des cookies
        return false;
    }
    return true;
} */

function checkCookies(event) {
    event.preventDefault(); // Empêche l'envoi immédiat du formulaire

    fetch('/cgi-bin/check_cookies.py', { // Fichier CGI qui gère la redirection
        method: 'GET',
        credentials: 'same-origin'
    }).then(response => {
        if (response.redirected) {
            window.location.href = response.url; // Redirige si le serveur l'exige
        } else {
            event.target.submit(); // Si pas de redirection, envoie le formulaire
        }
    }).catch(error => {
        console.error("Erreur lors de la vérification des cookies :", error);
    });

    return false; // Empêche l’envoi du formulaire avant d’avoir une réponse du serveur
}

window.checkCookies = checkCookies;