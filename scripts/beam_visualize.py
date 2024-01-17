from PyQt5.QtWidgets import QApplication, QWidget, QHBoxLayout, QVBoxLayout, QPushButton
from PyQt5.QtCore import pyqtSignal, QObject, Qt, QRectF
import pyqtgraph as pg

import re
import sys
import argparse
import threading
import zmq
import logging
from collections import deque

from utils import parse_com_log

default_publisher_addr = "tcp://127.0.0.1:5333"


class BeamLocationPlotter(QObject):
    _need_update = pyqtSignal()
    _need_update_marker = pyqtSignal()

    def __init__(self, plot):
        super().__init__()
        self.plot = plot
        self.history_length = 20
        self.beam_dot_items = []
        self.beam_dot_locations = deque(maxlen = self.history_length)
        self.marker_location = None
        self.marker_item = None

        self.init_dots()
        self._need_update.connect(self.update)
        self._need_update_marker.connect(self.update_marker)

    def init_dots(self):
        for i in range(self.history_length):
            item = pg.ScatterPlotItem(
                    size=0.1,
                    brush=pg.mkBrush(30, 255, 35, 255 / self.history_length * (i + 1)),
                    pxMode=False
                    )
            self.beam_dot_items.append(item)
            self.plot.addItem(item)

    def update_marker(self):
        if not self.marker_item:
            self.marker_item = pg.ScatterPlotItem(
                        size=0.05,
                        brush=pg.mkBrush(255, 0, 0, 200),
                        pxMode=False
                        )
            self.marker_item.setSymbol("x", update=False)
            self.plot.addItem(self.marker_item)

        x, y = self.marker_location
        self.marker_item.setData([y], [x])

    def update(self):
        for i, (x, y, err_x, err_y) in enumerate(self.beam_dot_locations):
            self.beam_dot_items[i].setSize(max(err_x, err_y))
            self.beam_dot_items[i].setData([y], [x])  # IMPORTANT: x and y are flipped!

    def add_point(self, x, y, err_x, err_y):
        self.beam_dot_locations.append((x, y, err_x, err_y))
        self._need_update.emit()

    def set_marker(self, x, y):
        self.marker_location = (x, y)
        self._need_update_marker.emit()


beam1 = None
beam2 = None
pub_addr = None

def receive_data():
    global beam1, beam2

    zmq_ctx = zmq.Context()
    zmq_socket = zmq_ctx.socket(zmq.SUB)
    zmq_socket.setsockopt(zmq.SUBSCRIBE, b"")
    zmq_socket.connect(pub_addr)

    logging.info(f"Connected to {pub_addr}")

    try:
        while True:
            line = zmq_socket.recv().decode('utf-8')
            logging.info(f">>> {line}")

            arr = parse_com_log(line)

            if not arr:
                continue

            num = arr['p']
            x = arr['x']
            y = arr['y']

            err_x = arr['err_x']
            err_y = arr['err_y']

            if num == 1:
                beam = beam1
            elif num == 2:
                beam = beam2
            
            beam.add_point(x, y, err_x, err_y)

            # if num == 1:
            #     print(f"p{num} x: {x:.2f} ({err_x:.2f}), y: {y:.2f} ({err_y:.2f})")
            # else:
            #     print(f">> p{num} x: {x:.2f} ({err_x:.2f}), y: {y:.2f} ({err_y:.2f})")

    except KeyboardInterrupt:
        pass


def mark_current_beam():
    global beam1, beam2

    b1x, b1y, _, _ = beam1.beam_dot_locations[-1]
    b2x, b2y, _, _ = beam2.beam_dot_locations[-1]
    beam1.set_marker(b1x, b1y)
    beam2.set_marker(b2x, b2y)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Beam location visualizer.")

    parser.add_argument("--addr", metavar='ADDR', type=str, 
            help="Address to the beam location publisher.", default=default_publisher_addr)
    args = parser.parse_args()

    pub_addr = args.addr

    app = QApplication([])

    window = QWidget()
    layout = QVBoxLayout()

    window.setLayout(layout)

    window.setWindowTitle("Beam visualize")
    window.show()

    glayout = pg.GraphicsLayoutWidget(show=True)

    glayout.resize(1000, 800)
    glayout.ci.setBorder((50, 50, 100))

    sub1 = glayout.addLayout()
    sub1.addLabel("<b>P1</b>")
    sub1.nextRow()
    p1 = sub1.addPlot()
    p1.setRange(QRectF(-1, -1, 2, 2))
    p1.setAspectLocked(True, 1)
    beam1 = BeamLocationPlotter(p1)

    sub2 = glayout.addLayout()
    sub2.addLabel("<b>P2</b>")
    sub2.nextRow()
    p2 = sub2.addPlot()
    p2.setRange(QRectF(-1, -1, 2, 2))
    p2.setAspectLocked(True, 1)
    beam2 = BeamLocationPlotter(p2)

    layout.addWidget(glayout)

    marker_btn = QPushButton("Set Marker")
    marker_btn.clicked.connect(mark_current_beam)
    layout.addWidget(marker_btn)

    window.resize(1000, 600)

    th = threading.Thread(target=receive_data, daemon=True)
    th.start()

    sys.exit(app.exec_())
