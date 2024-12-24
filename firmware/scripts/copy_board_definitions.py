import shutil
import sys
from pathlib import Path

def copy_file(source_path, destination_path):
    """
    Copy a file from source_path to destination_path.

    Parameters:
    source_path (str): The path of the source file.
    destination_path (str): The path where the file should be copied to.

    Raises:
    FileNotFoundError: If the source file does not exist.
    PermissionError: If there are permission issues with the source or destination.
    """
    try:
        shutil.copy(source_path, destination_path)
        print(f"File copied from {source_path} to {destination_path}.")
    except FileNotFoundError:
        print(f"Error: The file {source_path} was not found.")
    except PermissionError:
        print(f"Error: Permission denied when accessing {source_path} or {destination_path}.")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

# Example usage

import shutil
import os

def copy_directory(source_dir, destination_dir):
    """
    Recursively copy a directory from source_dir to destination_dir.

    Parameters:
    source_dir (str): The path of the source directory.
    destination_dir (str): The path where the directory should be copied to.

    Raises:
    FileNotFoundError: If the source directory does not exist.
    PermissionError: If there are permission issues with the source or destination.
    """
    try:
        # Ensure the source directory exists
        if not os.path.isdir(source_dir):
            raise FileNotFoundError(f"The source directory {source_dir} does not exist.")
        
        # Copy the directory and overwrite existing files
        shutil.copytree(source_dir, destination_dir, dirs_exist_ok=True)
        print(f"Directory copied from {source_dir} to {destination_dir}, overwriting existing files.")
    except FileNotFoundError:
        print(f"Error: The source directory {source_dir} was not found.")
    except PermissionError:
        print(f"Error: Permission denied when accessing {source_dir} or {destination_dir}.")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")


home_dir = str(Path.home())
copy_file('firmware/board_definitions/grow_engn_axis_driver.json', home_dir+"/.platformio/platforms/atmelsam/boards/grow_engn_axis_driver.json")
copy_directory('firmware/board_definitions/axis_driver', home_dir+"/.platformio/packages/framework-arduino-samd-adafruit/variants/axis_driver")