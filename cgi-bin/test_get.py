#!/usr/bin/env python3

import os
import sys

# Print headers
print("Content-Type: text/html")
print("Status: 200 OK")
print()  # Empty line to separate headers from body

# Print HTML content
print("<html><head><title>CGI GET Test</title></head><body>")
print("<h1>CGI GET Test Successful!</h1>")
print("<h2>Environment Variables:</h2>")
print("<ul>")

# Show some relevant environment variables
env_vars = ['REQUEST_METHOD', 'QUERY_STRING', 'HTTP_HOST', 'SERVER_NAME', 'SERVER_PORT', 'SCRIPT_NAME', 'PATH_INFO']
for var in env_vars:
    value = os.environ.get(var, 'Not set')
    print(f"<li><b>{var}:</b> {value}</li>")

print("</ul>")
print("</body></html>")
