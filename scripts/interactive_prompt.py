import sys
import zmq
import time
import asyncio
import zmq.asyncio
import threading
import argparse
import logging
import numpy as np
from datetime import datetime
from collections import deque

import aioconsole

ctx = zmq.asyncio.Context()

async def receive_data(writer, addr, port):
    zmq_socket = ctx.socket(zmq.SUB)
    zmq_socket.setsockopt(zmq.SUBSCRIBE, b"")
    zmq_socket.connect(f"tcp://{addr}:{port}")

    try:
        while True:
            line = (await zmq_socket.recv()).decode('utf-8')
            if line[0] == "]":
                writer.write(line[1:].strip()+"\n")
    except KeyboardInterrupt:
        pass

async def read_stdin_send_cmd(reader, addr, port):
    s = ctx.socket(zmq.PUSH)
    s.connect(f"tcp://{addr}:{port}")

    try:
        while True:
            line = await reader.readline()
            if not line:
                break
            await s.send(line.strip()+b"\n")
    except KeyboardInterrupt:
        pass

async def main(args):
    reader, writer = await aioconsole.get_standard_streams()

    await asyncio.gather(read_stdin_send_cmd(reader, args.addr, args.push_port), receive_data(writer, args.addr, args.pub_port))

if __name__ == "__main__":
    logging.basicConfig(format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p', level=logging.INFO)

    parser = argparse.ArgumentParser(description="Sending and receiving commands to/from auto-alignment MCU.")

    parser.add_argument("--addr", dest='addr', metavar='ADDR', type=str, 
            help="Address to the receiver zmq server.", default="127.0.0.1")
    parser.add_argument("--pub-port", dest='pub_port', metavar='PUB_PORT', type=int, 
            help="Port to the message publisher of the receiver zmq server.", default=5333)
    parser.add_argument("--push-port", dest='push_port', metavar='PUSH_PORT', type=int, 
            help="Push port to the command transmitter of the receiver zmq server.", default=5334)
    args = parser.parse_args()

    try:
        asyncio.run(main(args))
    except KeyboardInterrupt:
        pass

