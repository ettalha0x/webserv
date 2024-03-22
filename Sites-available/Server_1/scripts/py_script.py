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

#  !/usr/bin/env python







#  #!/usr/bin/env python

# import sys

# # Set custom headers
# body_content = "This is the response body content."
# # content_length = len(body_content)
# content_length = 100

# custom_headers = {
#     "Content-Type": "text/plain",
#     "Content-Length": content_length
# }

# # Response body content

# # Calculate content length
# # content_length = 70

# # Print the headers
# for header_name, header_value in custom_headers.items():
#     print(f"{header_name}: {header_value}")


# # print("Content-Length: " + str(content_length))
# print("\r\n\r\n", end='') # Blank line to indicate the end of headers

# # Print the response body
# print(body_content)