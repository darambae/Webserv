function acceptCookies() {
    fetch('/cookies-consent', {
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
    fetch('/cookies-consent', {
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