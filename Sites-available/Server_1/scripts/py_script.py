# !/usr/bin/env python3

import cgi

# Print Content-Type header
print("Content-Type: text/html\n")
print("\r\n\r\n", end='')
# Parse the form data from stdin
form = cgi.FieldStorage()

# Check if 'variable_name' exists in the form data
if 'name' in form:
    variable_value = form['name'].value
    print(f"""<!DOCTYPE html><html><head>    <title>CGI Server Response</title></head><body>    <h1>Variable Value: {variable_value}</h1></body></html>""")
else:
    print("""<!DOCTYPE html><html><head>    <title>CGI Server Response</title></head><body>    <h1>Variable not found in request</h1></body></html>""")
