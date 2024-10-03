#!/usr/bin/python3

import cgi
import sys
import os

print("Content-Type: text/html\n)

content_length = int(os.environ.get('CONTENT_LENGTH', 0))

post_data = sys.stdin.read(content_length)

form = cgi.FieldStorage()

name = form.getvalue('name')
email = form.getvalue('email')

print(f"<html><body>")
print(f"<h1>Hello {name}, your email is {email}</h1>")
print(f"</body></html>")
