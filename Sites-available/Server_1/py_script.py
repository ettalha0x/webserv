 #!/usr/bin/env python

import sys

# Set custom headers
body_content = "This is the response body content."
# content_length = len(body_content)
content_length = 10

custom_headers = {
    "Content-Type": "text/plain",
    "Content-Length": content_length
}

# Response body content

# Calculate content length
# content_length = 70

# Print the headers
for header_name, header_value in custom_headers.items():
    print(f"{header_name}: {header_value}")

# print("Content-Length: " + str(content_length))
print("\r\n\r\n", end='') # Blank line to indicate the end of headers

# Print the response body
print(body_content)