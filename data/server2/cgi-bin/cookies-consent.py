#!/usr/bin/env python3
import sys
import os
import urllib.parse
import uuid
import pytz
from datetime import datetime, timedelta

# --- Vérification de la méthode HTTP ---
method = os.environ.get("REQUEST_METHOD", "")
if method != "POST":
    print("Status: 405 Method Not Allowed\r\n")
    print("Content-Length: 0\r\n\r\n")
    sys.exit(0)

# --- Lecture sécurisée du body ---
content_length = int(os.environ.get("CONTENT_LENGTH", 0))
body = os.environ.get("QUERY_STRING", "")

# --- Parsing des paramètres ---
params = urllib.parse.parse_qs(body)
consent = params.get("consent", [""])[0].strip()

# --- check consent ---
if consent in ["accept", "reject"]:
    local_tz = pytz.timezone("Europe/Paris")
    utc_now = datetime.utcnow()
    local_now = utc_now.replace(tzinfo=pytz.utc).astimezone(local_tz)
    #expiration date in 1 year
    #expire_date = (datetime.utcnow() + timedelta(days=365)).strftime("%a, %d %b %Y %H:%M:%S GMT")
    #expiration date in 3mn
    expire_date = (local_now + timedelta(minutes=3)).strftime("%a, %d %b %Y %H:%M:%S GMT")

    if consent == "accept":
        session_id = str(uuid.uuid4())
    else: 
        session_id = ""

    response_body = "OK"
    print("Status: 200")
    print(f"Content-Length: {len(response_body)}")
    if session_id:
        print(f"Set-Cookie: cookie-consent={consent}; Path=/; Expires={expire_date}")
        print(f"Set-Cookie: session_id={session_id}; Path=/; HttpOnly\r\n\r")
    else:
        print(f"Set-Cookie: cookie-consent={consent}; Path=/; Expires={expire_date}\r\n\r")
    print(response_body)
else:
    print("Status: 400 Bad Request\r\n\r\n")
