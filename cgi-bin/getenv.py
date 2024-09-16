#!/usr/bin/python3

import os

print("Content-type: text/html\r\n\r\n");
print("<font size+=1Environment</font><\br>");
for param in os.environ.keys():
    print("<b>%20s<\br>" % (param, os.environ[param]))
