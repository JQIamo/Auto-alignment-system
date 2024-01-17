import zmq
import time
import threading
import numpy as np
from datetime import datetime
from collections import deque
from functools import partial

from utils import parse_com_log

p1_queue = deque(maxlen=60)
p2_queue = deque(maxlen=60)

ctx = zmq.Context()
s = ctx.socket(zmq.PUSH)
s.connect("tcp://127.0.0.1:5334")

m_pos = {1: 0, 2: 0, 3: 0, 4: 0}

moving_lock = threading.Lock()

data_callback = None

def move_m(m, pos):
    d = pos - m_pos[m]
    s.send(f"m {m} {d:d}\n".encode("utf-8"))
    m_pos[m] = pos

def write_data(path, m, arr):
    with open(path, "a") as f:
        num = arr['p']

        x1 = arr['x1']
        y1 = arr['y1']
        x2 = arr['x2']
        y2 = arr['y2']
        var_x1 = arr['var_x1']
        var_x2 = arr['var_x2']
        var_y1 = arr['var_y1']
        var_y2 = arr['var_y2']

        print(f"pos {m} r{num} {int(time.time()):d} {x1} {x2} {y1} {y2} {var_x1} {var_x2} {var_y1} {var_y2}")
        f.write(f"pos {m} r{num} {int(time.time()):d} {x1} {x2} {y1} {y2} {var_x1} {var_x2} {var_y1} {var_y2}\n")

def receive_data():
    global data_callback
    zmq_socket = ctx.socket(zmq.SUB)
    zmq_socket.setsockopt(zmq.SUBSCRIBE, b"")
    zmq_socket.connect("tcp://127.0.0.1:5333")

    try:
        while True:
            if moving_lock.locked():
                time.sleep(0.1)
                continue
            line = zmq_socket.recv().decode('utf-8')
            if data_callback:
                arr = parse_com_log(line)
                if arr: 
                    data_callback(arr)
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    threading.Thread(target=receive_data, daemon=True).start()

    _range = { 
            1: [list(range(0, 1500, 40)), 
                list(range(1500, -1500, -40)),
                list(range(-1500, 1500, 40)),
                list(range(1500, -1500, -40)),
                list(range(-1500, 1500, 40)),
                list(range(1500, 0, -40))],
            2: [list(range(0, 1000, 40)), 
                list(range(1000, -1000, -40)),
                list(range(-1000, 1000, 40)),
                list(range(1000, -1000, -40)),
                list(range(-1000, 1000, 40)),
                list(range(1000, 0, -40))],
            3: [list(range(0, 1500, 40)), 
                list(range(1500, -1500, -40)),
                list(range(-1500, 1500, 40)),
                list(range(1500, -1500, -40)),
                list(range(-1500, 1500, 40)),
                list(range(1500, 0, -40))],
            4: [list(range(0, 1200, 40)), 
                list(range(1200, -1200, -40)),
                list(range(-1200, 1200, 40)),
                list(range(1200, -1200, -40)),
                list(range(-1200, 1200, 40)),
                list(range(1200, 0, -40))],
            }

    t = datetime.now().isoformat(timespec="hours")

    for m, segs in _range.items():
        for i, seg in enumerate(segs):
            filename = f"{t}_m{m}_seg{i}.txt"
            for pos in seg:
                print(f"M{m} seg{i} to {pos}")
                with moving_lock:
                    move_m(m, pos)
                    time.sleep(1)

                data_callback = partial(write_data, filename, pos)
                time.sleep(3)
