Import("env")
import os

def copy_firmware(source, target, env):
    os.system('python firmware/scripts/uf2conv.py .pio/build/grow_engn_axis_driver/firmware.bin --base 0x4000 --family 0x55114460 --convert --output .pio/build/grow_engn_axis_driver/firmware.uf2')

env.AddPostAction("buildprog", copy_firmware)
