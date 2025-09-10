import os
from SCons.Script import *

# Source files and folders
source_files = [
    'leafgl.h',
    'util/model.h',
    'util/model.c'
]

# Destination directory
glewf_dir = 'GLEWF'

# Environment setup
env = Environment()

# First, create the GLEWF directory if it doesn't exist
if not os.path.exists(glewf_dir):
    os.makedirs(glewf_dir)

# Copy files preserving directory structure
copied_files = []
for src in source_files:
    # This will preserve subdirectories, e.g. util/model.c
    dst = os.path.join(glewf_dir, src)
    # Make sure destination directory exists
    dst_dir = os.path.dirname(dst)
    if not os.path.exists(dst_dir):
        os.makedirs(dst_dir)
    copied = env.Command(dst, src, Copy("$TARGET", "$SOURCE"))
    copied_files.append(copied)

# Custom action: Move the entire GLEWF folder to /usr/include preserving structure
def install_action(target, source, env):
    if os.path.exists('/usr/include/GLEWF'):
        print("Updating GLEWF library for newest leafgl")
        os.system('sudo rm -rf /usr/include/GLEWF')
    print("Installing GLEWF library to /usr/include")
    return os.system('sudo mv GLEWF /usr/include')

# Add the move as the final build step
env.AlwaysBuild(env.Command('install', copied_files, install_action))

def uninstall_action(target, source, env):
    print("Uninstalling GLEWF library... see you next time :3")
    return os.system('sudo rm -rf /usr/include/GLEWF')

env.Alias('uninstall', env.Command(None, None, uninstall_action))

def rebuild_action(target, source, env):
    print("Rebuilding GLEWF library... hang tight...")
    uninstall_action(target, source, env)
    install_action(target, source, env)

env.Alias('rebuild', env.Command(None, None, rebuild_action))
