#!/usr/bin/python3

import cgi, os

form = cgi.FieldStorage()

# Get filename here
fileitem = form['cgi']

# Test if the file was uploaded
if fileitem.filename:
   open(os.getcwd() + '/upload_files/' + os.path.basename(fileitem.filename), 'wb').write(fileitem.file.read())

image_url = f"/upload_files/{os.path.basename(fileitem.filename)}"

print(f"""
<!DOCTYPE html>
<html>
    <head>
        <style>
            @keyframes hole {{
                100% {{
                    transform: rotate(360deg);
                }}
            }}
            @keyframes gradientAnimation {{
                0% {{
                    background-position: 0% 50%;
                }}
                50% {{
                    background-position: 100% 50%;
                }}
                100% {{
                    background-position: 0% 50%;
                }}
            }}
            @keyframes rainbowAnimation {{
                0% {{
                    background-position: 0% 0%;
                }}
                100% {{
                    background-position: 100% 100%;
                }}
            }}
            a {{
                color: #7db8ec;
                font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;
                position: absolute;
                font-size: 2em;
                margin-bottom: 25em;
                transition: color 0.5s ease;
                text-decoration: none;
            }}
            a:hover {{
                color: aliceblue;
                cursor: pointer;
            }}
            #text1 {{
                margin-right : 30em;
            }}
            #text2 {{
                margin-right : 10em;
            }}
            #text3 {{
                margin-left : 13em;
            }}
            #text4 {{
                margin-left : 37em;
            }}
            #text5 {{
                color: #7db8ec;
                font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;
                position: absolute;
                font-size: 3em;
                margin-bottom: 10em;
                background: linear-gradient(45deg, red, orange, yellow, green, blue, indigo, violet, indigo, blue, green, yellow, red);
                background-size: 300% 300%;
                -webkit-background-clip: text;
                -webkit-text-fill-color: transparent;
                animation: rainbowAnimation 15s linear infinite;
            }}
            html, body {{
                margin: 0;
                padding: 0;
                height: 100%;
                width: 100%;
                overflow: hidden;
            }}
            textarea {{
                width: 300px;
                height: 50px;
                margin-bottom: 10px;
                padding: 10px;
                border: 1px solid #ccc;
                border-radius: 5px;
                font-size: 1em;
                resize: none;
            }}
            #button-64 {{
                align-items: center;
                background-image: linear-gradient(144deg,#AF40FF, #5B42F3 50%,#00DDEB);
                border: 0;
                border-radius: 8px;
                box-shadow: rgba(151, 65, 252, 0.2) 0 15px 30px -5px;
                box-sizing: border-box;
                color: #FFFFFF;
                display: flex;
                font-family: Phantomsans, sans-serif;
                font-size: 10px;
                justify-content: center;
                line-height: 1em;
                max-width: 50%;
                min-width: 70px;
                padding: 3px;
                position: absolute;
                margin-top: 10em;
                text-decoration: none;
                user-select: none;
                -webkit-user-select: none;
                touch-action: manipulation;
                white-space: nowrap;
                cursor: pointer;
            }}

            #button-64:active,
            .button-64:hover {{
                outline: 0;
            }}

            #button-64 span {{
                background-color: rgb(5, 6, 45);
                padding: 16px 24px;
                border-radius: 6px;
                width: 100%;
                height: 100%;
                transition: 300ms;
            }}

            #button-64:hover span {{
                background: none;
            }}

            @media (min-width: 768px) {{
                #button-64 {{
                    font-size: 24px;
                    min-width: 196px;
                }}
            }}
	    #myFile {{
                color: white;
            }}
        </style>
    </head>
    <body style="margin: 0; padding: 0; height: 100%; background: linear-gradient(90deg, black, rgb(50, 1, 71)); background-size: 200% 200%; animation: gradientAnimation 8s ease infinite; display: flex; justify-content: center; align-items: center;">
        <a id="text1" class="text" href="/">HOME</a>
        <a id="text2" class="text" href="/cgi">CGI</a>
        <a id="text3" class="text" href="/cookies">COOKIES</a>
        <a id="text4" class="text" href="/upload">UPLOAD</a>
        <text id="text5">FILE UPLOADED !</text>
        <form method="post" enctype="multipart/form-data" action="/cgi-bin/upload.py">
            <input type="file" id="myFile" name="cgi">
            <input type="submit">
        </form>
        <img id="uploadedImage" src="{image_url}" alt="Uploaded Image" style="max-width: 20%; height: auto; margin-top: 600px; position: absolute;">
        <script>
            function processInput() {{
                var inputText = document.getElementById("userInput").value;
            }}
        </script>
    </body>
</html>
""")