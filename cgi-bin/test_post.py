#!/usr/bin/env python3

import os
import sys

# Get content length
content_length = int(os.environ.get('CONTENT_LENGTH', 0))

# Read POST data
post_data = ""
if content_length > 0:
    post_data = sys.stdin.read(content_length)

# Print headers
print("Content-Type: text/html")
print("Status: 200 OK")
print()  # Empty line to separate headers from body

# Print HTML content
print("<html><head><title>CGI POST Test</title></head><body>")
print("<h1>CGI POST Test Successful!</h1>")
print(f"<h2>Content Length: {content_length}</h2>")
print(f"<h2>POST Data:</h2>")
print(f"<pre>{post_data}</pre>")
print("<h2>Environment Variables:</h2>")
print("<ul>")

# Show some relevant environment variables
env_vars = ['REQUEST_METHOD', 'CONTENT_TYPE', 'CONTENT_LENGTH', 'HTTP_HOST', 'SERVER_NAME', 'SERVER_PORT']
for var in env_vars:
    value = os.environ.get(var, 'Not set')
    print(f"<li><b>{var}:</b> {value}</li>")

print("</ul>")
print("</body></html>")
