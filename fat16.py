# This file is used to test FAT16 related functionality as Mac doesn't support
# directly mounting a FAT16 drive.

from pyfatfs.PyFatFS import PyFatFS

fs = PyFatFS(filename='./bin/os.bin')

with open('./hello.txt', 'rb') as src:
    with fs.open('/hello.txt', 'wb') as dst:
        dst.write(src.read())

fs.close()