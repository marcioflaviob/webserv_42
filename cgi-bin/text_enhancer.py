#!/usr/bin/python3

import cgi
import cgi
import sys

print("Content-Type: text/html")
print()

# Function to generate simple ASCII art (for demonstration purposes)
def to_ascii_art(text):
    return "\n".join([line for line in text.split('\n')])

# Handle form data
form = cgi.FieldStorage()
user_input = form.getvalue("userInput", "")

# Convert user input to ASCII art
ascii_art = to_ascii_art(user_input)

print(f"""
<!DOCTYPE html>
<html>
    <head>
        <style>
            #asciiArt {{
                font-family: monospace;
                color: #7db8ec;
                white-space: pre; /* Preserve whitespace for ASCII art */
                margin-top: 20px;
            }}
        </style>
    </head>
    <body style="margin: 0; padding: 0; height: 100%; background: linear-gradient(90deg, black, rgb(50, 1, 71)); background-size: 200% 200%; animation: gradientAnimation 8s ease infinite; display: flex; justify-content: center; align-items: center;">
        <a id="text1" class="text" href="/">HOME</a>
        <a id="text2" class="text" href="/cgi">CGI</a>
        <a id="text3" class="text" href="/cookies">COOKIES</a>
        <a id="text4" class="text" href="/upload">UPLOAD</a>
        <text id="text5">TEXT ENHANCER</text>
        <form method="post" action="">
            <textarea name="userInput" id="userInput" placeholder="Type something here...">{user_input}</textarea><br>
            <button id="button-64" type="submit"><span class="text">SUBMIT</span></button>
        </form>
        <div id="output"></div>
        <div id="asciiArt">{ascii_art}</div> <!-- Display ASCII art here -->
    </body>
</html>
""")
