import time
import math
import re
import logging

p1_x1_offset = 0
p1_x2_offset = 55
p1_y1_offset = 3111
p1_y2_offset = 3095

p1_offsets = (p1_x1_offset, p1_x2_offset, p1_y1_offset, p1_y2_offset)

p2_x1_offset = 0
p2_x2_offset = 0
p2_y1_offset = 3131
p2_y2_offset = 3183

p2_offsets = (p2_x1_offset, p2_x2_offset, p2_y1_offset, p2_y2_offset)


def raw_to_point_x(p_offsets, x1, x2, var_x1=0, var_x2=0):
    (x1_offset, x2_offset, y1_offset, y2_offset) = p_offsets
    x1 = x1 - x1_offset
    x2 = x2 - x2_offset

    x = (x1 - x2) / (x1 + x2)

    err = 0
    if var_x1 and var_x2:
       sum_err_square = var_x1 + var_x2  # var is err^2
       _sum = x1 + x2
       _sub = x1 - x2
       err = abs(x * math.sqrt(sum_err_square * (1/_sum**2 + 1/(_sub**2))))

    return x, err

def raw_to_point_y(p_offsets, y1, y2, var_y1=0, var_y2=0):
    (x1_offset, x2_offset, y1_offset, y2_offset) = p_offsets
    y1 = y1 - y1_offset
    y2 = y2 - y2_offset

    y = (y1 - y2) / (y1 + y2)

    err = 0
    if var_y1 and var_y2:
       sum_err_square = var_y1 + var_y2  # var is err^2
       _sum = y1 + y2
       _sub = y1 - y2

       if sum_err_square < 5 and _sub == 0:
           err = abs(y * math.sqrt(sum_err_square * (1/_sum**2)))
       else:
           err = abs(y * math.sqrt(sum_err_square * (1/_sum**2 + 1/_sub**2)))

    return y, err

def parse_com_log(line):
    m = re.match("r(\d): (\d+), (\d+), (\d+), (\d+), var(\d): (\d+), (\d+), (\d+), (\d+)", line)

    if not m:
        return None

    t = time.time()

    num = int(m[1])
    x1 = int(m[2])
    x2 = int(m[3])
    y1 = int(m[4])
    y2 = int(m[5])

    var_x1 = int(m[7])
    var_x2 = int(m[8])
    var_y1 = int(m[9])
    var_y2 = int(m[10])

    if num == 1:
        p_offsets = p1_offsets
    elif num == 2:
        p_offsets = p2_offsets

    x, err_x = -1, -1
    y, err_y = -1, -1
    try:
        x, err_x = raw_to_point_x(p_offsets, x1, x2, var_x1, var_x2)
    except ZeroDivisionError:
        logging.warning("ERROR: Zero division")

    try:
        y, err_y = raw_to_point_y(p_offsets, y1, y2, var_y1, var_y2)
    except ZeroDivisionError:
        logging.warning("ERROR: Zero division")

    data_dict = {
        "t": t,
        "p": num,
        "x1": x1,
        "x2": x2,
        "y1": y1,
        "y2": y2,
        "var_x1": var_x1,
        "var_x2": var_x2,
        "var_y1": var_y1,
        "var_y2": var_y2,
        "x": x,
        "y": y,
        "err_x": err_x,
        "err_y": err_y
    }

    return data_dict
