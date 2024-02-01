import os

# Retrieve and print the value of SCRIPT_NAME
script_name = os.environ.get("REQUEST_METHOD")
print(script_name)