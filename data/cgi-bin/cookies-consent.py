#!/usr/bin/env python3
import sys
import os
import urllib.parse
from datetime import datetime, timedelta

# --- Vérification de la méthode HTTP ---
method = os.environ.get("REQUEST_METHOD", "")
if method != "POST":
    print("Status: 405 Method Not Allowed\r\n")
    print("Content-Length: 0\r\n\r\n")
    sys.exit(0)

# --- Lecture sécurisée du body ---
content_length = int(os.environ.get("CONTENT_LENGTH", 0))
body = sys.stdin.read(content_length)

# --- Parsing des paramètres ---
params = urllib.parse.parse_qs(body)
consent = params.get("consent", [""])[0].strip()

# --- DEBUG LOGS ---
print(f"#DEBUG Body: '{body}'", file=sys.stderr)
print(f"#DEBUG Consent: '{consent}'", file=sys.stderr)

# --- check consent ---
if consent in ["accept", "reject"]:
    # Date d'expiration du cookie dans 1 an
    expire_date = (datetime.utcnow() + timedelta(days=365)).strftime("%a, %d %b %Y %H:%M:%S GMT")
    
    response_body = "OK"
    print("Status: 200 OK")
    print(f"Content-Length: {len(response_body)}")
    print(f"Set-Cookie: cookie-consent={consent}; Path=/; Expires={expire_date}\r\n\r\n")
    print(response_body)
else:
    print("Status: 400 Bad Request\r\n\r\n")
