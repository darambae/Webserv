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