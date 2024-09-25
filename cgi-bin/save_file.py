#!/usr/bin/python3

import cgi
import os
import cgitb; cgitb.enable()

form = cgi.FieldStorage()

#fileitem = form['filename']

if 'filename' in form:
	fileitem = form['filemane']

	if fileitem.filename:
		fn = os.path.basename(fileitem.filename)
		with open('/tmp/' + fn, 'wb') as f:
			f.write(fileitem.file.read())
	
		message = f'The file "{fn}" was uploaded successfully'
	else:
		'No file was uploaded'
else:
	message = 'No file field found in the form'


print("Content-Type: text/html\n")
print(f"""
<html>
	<body>
		<p>{message}</p>
	</body>
</html>
""")
