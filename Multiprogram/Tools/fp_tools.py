import numpy as np
from PIL import Image

# from martineau @ stackoverflow
def int_to_bytes(n, minlen=0):
    """ Convert integer to bytearray with optional minimum length. 
    """
    if n > 0:
        arr = []
        while n:
            n, rem = n >> 8, n & 0xff
            arr.append(rem)
        b = bytearray(reversed(arr))
    elif n == 0:
        b = bytearray(b'\x00')
    else:
        raise ValueError('Only non-negative values supported')

    if minlen > 0 and len(b) < minlen: # zero padding needed?
        b = (minlen-len(b)) * '\x00' + b
    return b

# convert image to bytearray file for easy texture loading on PS2
def img_to_PS2_raw(image_path, final_path):
    if('.raw' not in final_path):
        final_path += '.raw'
    img = Image.open(image_path)
    data = np.array(img)
    new_data = [int_to_bytes(c) for j in data for i in j for c in i[:3]]

    f_bytes = bytearray()
    for byte in new_data:
        f_bytes += byte

    with open(final_path, 'wb') as fp:
        fp.write(f_bytes)

img_path = '../images/bedrich.bmp'
new_img_name = '../images/bedrich.raw'
img_to_PS2_raw(img_path, new_img_name)