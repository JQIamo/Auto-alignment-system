import re
import sys
import argparse
import threading
import zmq
from collections import deque

pub_addr = "tcp://127.0.0.1:5333"

def receive_data():

    zmq_ctx = zmq.Context()
    zmq_socket = zmq_ctx.socket(zmq.SUB)
    zmq_socket.setsockopt(zmq.SUBSCRIBE, b"")
    zmq_socket.connect(pub_addr)

    print(f"Connected to {pub_addr}")

    try:
        for i in range(10):
            arr = zmq_socket.recv_json()

            num = arr['p']
            x = arr['x']
            y = arr['y']

            err_x = arr['err_x']
            err_y = arr['err_y']

            if num == 1:
                print(f"p{num} x: {x:.2f} ({err_x:.2f}), y: {y:.2f} ({err_y:.2f})")
            else:
                print(f">> p{num} x: {x:.2f} ({err_x:.2f}), y: {y:.2f} ({err_y:.2f})")

    except KeyboardInterrupt:
        pass

receive_data()
