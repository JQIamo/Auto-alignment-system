import zmq
import zmq.asyncio
import sys
import argparse
import logging
from serial_asyncio import open_serial_connection
import asyncio

com_port = ""
pub_bind_addr = "tcp://*:5333"
pull_bind_addr = "tcp://*:5334"

com_reader = None
com_writer = None

zmq_ctx = None
pub_socket = None
pull_socket = None


async def grab_com_data():
    assert com_reader

    try:
        while True:
            line = (await com_reader.readline())
            logging.info(f">>> {line.decode('utf-8').strip()}")

            await pub_socket.send(line)
            
            await asyncio.sleep(0)
    except KeyboardInterrupt:
        pass

async def listen_write_com():
    assert com_writer

    try:
        while True:
            msg = await pull_socket.recv()
            logging.info(f"<<< {msg.decode('utf-8').strip()}")
            com_writer.write(msg)
            await com_writer.drain()

            await asyncio.sleep(0)
    except KeyboardInterrupt:
        pass


async def main():
    global com_reader, com_writer

    com_reader, com_writer = await open_serial_connection(url=com_port)
    grab_task = asyncio.create_task(grab_com_data())
    listen_task = asyncio.create_task(listen_write_com()) 

    await asyncio.wait([grab_task, listen_task])


if __name__ == "__main__":
    logging.basicConfig(format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p', level=logging.INFO)

    parser = argparse.ArgumentParser(description="Auto-alignment MCU interface.")

    parser.add_argument("com", metavar='COM', type=str, 
            help="COM Port connecting to the PSD MCU.")
    args = parser.parse_args()

    com_port = args.com

    zmq_ctx = zmq.asyncio.Context()
    pub_socket = zmq_ctx.socket(zmq.PUB)
    pub_socket.bind(pub_bind_addr)
    logging.info(f"Publisher binds at {pub_bind_addr}")

    pull_socket = zmq_ctx.socket(zmq.PULL)
    pull_socket.bind(pull_bind_addr)
    logging.info(f"Listener binds at {pull_bind_addr}")

    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass
