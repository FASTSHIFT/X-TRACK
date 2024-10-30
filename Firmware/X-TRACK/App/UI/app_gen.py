"""
MIT License

Copyright (c) 2024 _VIFEXTech, ChatGPT-3.5

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

import os
import shutil
import sys

def copy_template_to_new_app(template_dir, new_app_name):
    # New app directory name
    new_app_dir = new_app_name

    # Check if directory already exists
    if os.path.exists(new_app_dir):
        print(f"Directory '{new_app_dir}' already exists. Aborting.")
        sys.exit(1)

    # Copy template directory to new app directory
    shutil.copytree(template_dir, new_app_dir)

    # Generate new file names
    new_file_names = [file.replace('Template', new_app_name) for file in os.listdir(new_app_dir)]

    # Modify file names
    for old_file, new_file in zip(os.listdir(new_app_dir), new_file_names):
        os.rename(os.path.join(new_app_dir, old_file), os.path.join(new_app_dir, new_file))

    # Modify template name in file contents
    for file_name in new_file_names:
        file_path = os.path.join(new_app_dir, file_name)
        with open(file_path, 'r') as file:
            file_data = file.read()
        file_data = file_data.replace('Template', new_app_name)
        file_data = file_data.replace('__TEMPLATE', '__' + new_app_name.upper())
        with open(file_path, 'w') as file:
            file.write(file_data)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: python {sys.argv[0]} <NewAppName>")
        sys.exit(1)

    new_app_name = sys.argv[1]
    template_dir = "_Template"

    copy_template_to_new_app(template_dir, new_app_name)
    print(f"Successfully created new app '{new_app_name}' from template.")
