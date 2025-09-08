#!/usr/bin/env python3
import os
import sys

# --- Configuration ---
upload_dir = "./uploads"
os.makedirs(upload_dir, exist_ok=True)

# --- Read request metadata ---
content_length = int(os.environ.get("CONTENT_LENGTH", 0))
content_type = os.environ.get("CONTENT_TYPE", "")

if content_length <= 0 or "multipart/form-data" not in content_type:
    print("Content-Type: text/html\n")
    print("<h1 style='color:red;'>Invalid upload request</h1>")
    sys.exit(0)

# --- Read request body ---
body = sys.stdin.buffer.read(content_length)

# Extract multipart boundary
boundary = "--" + content_type.split("boundary=")[-1]
boundary_bytes = boundary.encode()

# Split body into parts (skip the first empty part, and the last after the final boundary)
parts = body.split(boundary_bytes)[1:-1]

saved_files = []

for part in parts:
    if b"filename=" not in part:
        continue

    header_end = part.find(b"\r\n\r\n")
    if header_end == -1:
        continue

    headers = part[:header_end].decode(errors="ignore")
    file_content = part[header_end+4:]

    # Extract filename
    filename = ""
    for header_line in headers.split("\r\n"):
        if "filename=" in header_line:
            filename = header_line.split("filename=")[-1].strip().strip('"')
            filename = os.path.basename(filename)
            break

    if filename and len(file_content) > 0:
        save_path = os.path.join(upload_dir, filename)
        with open(save_path, "wb") as f:
            f.write(file_content)
        saved_files.append((filename, save_path))

# --- Output result page ---
print("""<!-- Content-Type: text/html -->""")
print("<!DOCTYPE html>")
print("<html><head><title>Upload Result</title></head><body style='font-family:sans-serif;'>")

if saved_files:
    print("<h1>Upload Successful!</h1>")
    print("<ul>")
    for filename, path in saved_files:
        print(f"<li>Saved: <b>{path}</b></li>")
        # Display image if it's a common image type
        if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.gif', '.bmp', '.webp')):
            print(f"<div><img src='/uploads/{filename}' alt='{filename}' style='max-width:400px;max-height:300px;'/></div>")
    print("</ul>")
else:
    print("<h1 style='color:red;'>No image uploaded.</h1>")

print("<br><button onclick=\"location.href='/upload.html'\">Go Back</button>")
print("</body></html>")