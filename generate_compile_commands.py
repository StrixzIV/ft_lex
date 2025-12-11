import json
import glob
import os

project_dir = os.getcwd()
src_dir = os.path.join(project_dir, 'src')
libl_dir = os.path.join(project_dir, 'libl')
include_dir = os.path.join(project_dir, 'include')

entries = []

# C++ files
cpp_files = glob.glob(os.path.join(src_dir, '*.cpp'))
for f in cpp_files:
    entries.append({
        "directory": project_dir,
        "command": f"clang++ -Wall -Wextra -Werror -std=c++17 -I{include_dir} -c {f}",
        "file": f
    })

# C files
c_files = glob.glob(os.path.join(libl_dir, '*.c'))
for f in c_files:
    entries.append({
        "directory": project_dir,
        "command": f"clang -Wall -Wextra -Werror -c {f}",
        "file": f
    })

with open('compile_commands.json', 'w') as f:
    json.dump(entries, f, indent=4)
