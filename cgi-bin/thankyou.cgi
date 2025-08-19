#!/usr/bin/env python3

import sys
import os

# Enable debugging

# Read POST data from stdin if any
length = os.environ.get('CONTENT_LENGTH')
if length:
    try:
        length = int(length)
        post_data = sys.stdin.read(length)
    except ValueError:
        post_data = ''
else:
    post_data = ''

# Parse name from POST data
name = "Friend"
if post_data:
    for pair in post_data.split('&'):
        key, _, value = pair.partition('=')
        if key == "name":
            name = value.replace('+', ' ')


# Output HTML page
print("""<!-- Content-Type: text/html -->""")
print(f"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Thank You</title>
<style>
  body {{
    font-family: 'Arial', sans-serif;
    background-color: #FFF9C4;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    height: 100vh;
    margin: 0;
  }}
  h1 {{
    font-size: 2rem;
    color: #555;
    margin-bottom: 20px;
  }}
  .button {{
    background-color: #FFECB3;
    border: none;
    border-radius: 12px;
    padding: 15px 30px;
    font-size: 18px;
    cursor: pointer;
    transition: background-color 0.3s, transform 0.2s;
  }}
  .button:hover {{
    background-color: #FFE082;
    transform: scale(1.05);
  }}
</style>
</head>
<body>
  <h1>Thank You, {name}! 💖</h1>
  <button class="button" onclick="location.href='../index.html'">Go Back</button>
</body>
</html>""")
