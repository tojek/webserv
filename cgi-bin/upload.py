#!/usr/bin/env python3
import os
import sys

# print("Content-Type: text/html\n")

# Read request metadata
content_length = int(os.environ.get("CONTENT_LENGTH", 0))
content_type = os.environ.get("CONTENT_TYPE", "")
upload_dir = os.environ.get("UPLOAD_DIR", ".")

if content_length <= 0 or "multipart/form-data" not in content_type:
    print("<html><body><h1 style='color:red;'>Invalid upload request</h1></body></html>")
    sys.exit(0)

body = sys.stdin.buffer.read(content_length)
boundary = "--" + content_type.split("boundary=")[-1]
parts = body.split(boundary.encode())

saved_files = []
for part in parts:
    if b"filename=" in part:
        header_end = part.find(b"\r\n\r\n")
        headers = part[:header_end].decode(errors="ignore")
        filename = headers.split("filename=")[-1].strip().strip('"')
        if not filename:
            continue
        file_content = part[header_end+4:]
        file_content = file_content.rstrip(b"\r\n--")
        save_path = os.path.join(upload_dir, os.path.basename(filename))
        with open(save_path, "wb") as f:
            f.write(file_content)
        saved_files.append(save_path)

# Output full HTML
print("<!DOCTYPE html>")
print("<html><head><title>Upload Result</title></head><body style='font-family:sans-serif;'>")
if saved_files:
    print("<div style='background-color:#fff8dc;padding:20px;border-radius:5px;'>")
    print("<h1>Upload Successful!</h1>")
    print("<ul>")
    for path in saved_files:
        print(f"<li>Saved to: <b>{path}</b></li>")
    print("</ul></div>")
else:
    print("<h1 style='color:red;'>No files were uploaded.</h1>")
print("</body></html>")
