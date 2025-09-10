#!/usr/bin/env python3
import os
import sys
import urllib.parse

# Read request metadata
content_length = int(os.environ.get("CONTENT_LENGTH", 0))
content_type = os.environ.get("CONTENT_TYPE", "")
upload_dir = os.environ.get("UPLOAD_DIR", ".")

if content_length <= 0 or "multipart/form-data" not in content_type:
    print("Content-Type: text/html\n")
    print("<html><body><h1 style='color:red;'>Invalid upload request</h1></body></html>")
    sys.exit(0)

body = sys.stdin.buffer.read(content_length)
boundary = "--" + content_type.split("boundary=")[-1]
parts = body.split(boundary.encode())

saved_files = []
custom_name = None

for part in parts:
    if b'Content-Disposition:' not in part:
        continue
    header_end = part.find(b"\r\n\r\n")
    headers = part[:header_end].decode(errors="ignore")
    # Extract filename if present
    if "filename=" in headers:
        filename = headers.split("filename=")[-1].strip().strip('"')
        file_content = part[header_end+4:]
        file_content = file_content.rstrip(b"\r\n--")
        if custom_name:
            save_name = custom_name
            if '.' not in save_name and '.' in filename:
                save_name += os.path.splitext(filename)[1]
        else:
            save_name = filename
        save_path = os.path.join(upload_dir, os.path.basename(save_name))
        os.makedirs(os.path.dirname(save_path), exist_ok=True)
        with open(save_path, "wb") as f:
            f.write(file_content)
        saved_files.append(save_path)
    elif 'name="custom_name"' in headers:
        # Extract custom_name value
        value = part[header_end+4:]
        value = value.decode(errors="ignore").strip().rstrip("\r\n--")
        if value:
            custom_name = urllib.parse.unquote_plus(value)

# Output full HTML
print("<!DOCTYPE html>")
print("<html><head><title>Upload Result</title></head>")
print("<body style='font-family:sans-serif;background-color:#fff9d9;padding:20px;'>")

if saved_files:
    print("""<!-- Content-Type: text/html -->""")
    print("<div style='background-color:#fff8dc;padding:20px;border-radius:5px;'>")
    print("<h1>Upload Successful!</h1>")
    print("<ul>")
    for path in saved_files:
        print(f"<li>Saved to: <b>{path}</b></li>")
    print("</ul></div>")
else:
    print("<h1 style='color:red;'>No files were uploaded.</h1>")

# Go back button
print("""
<br>
<button style="background-color:#ffd166;border:none;padding:10px 15px;border-radius:5px;cursor:pointer;font-weight:bold;"
        onclick="location.href='../index.html'">
  Go Back
</button>
""")

print("</body></html>")