
from PyQt5.QtWidgets import QApplication, QWidget, QHBoxLayout, QVBoxLayout
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



class PlotGraphicsView(pg.GraphicsLayoutWidget):
    _update_data_sig = pyqtSignal(int)
    def __init__(self, max_len=2000, color="w"):
        super().__init__()

        self.plot_item = self.addPlot(0, 0)
        pen = pg.mkPen(color=color, width=2)
        self.plot = self.plot_item.plot(pen=pen)

        self.data = deque(maxlen=max_len)

        self._update_data_sig.connect(self._update_data)
    
    def _update_data(self, data):
        self.data.append(data)
        self.plot.setData(self.data)

    def update_data(self, data):
        self._update_data_sig.emit(data)


def receive_data(callback1, callback2):
    zmq_ctx = zmq.Context()
    zmq_socket = zmq_ctx.socket(zmq.SUB)
    zmq_socket.setsockopt(zmq.SUBSCRIBE, b"")
    zmq_socket.connect(default_publisher_addr)

    try:
        while True:
            line = zmq_socket.recv().decode('utf-8')
            arr = parse_com_log(line)

            if not arr:
                continue

            num = arr['p']
            x1 = arr['x1']
            x2 = arr['x2']
            
            if num == 1:
                callback1(x1 + x2)
            elif num == 2:
                callback2(x1 + x2)
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    app = QApplication([])
    window = QWidget()
    layout = QVBoxLayout()

    p1_intensity_view = PlotGraphicsView()
    p2_intensity_view = PlotGraphicsView()

    p1_intensity_view.plot_item.setYRange(0, 5000)
    p2_intensity_view.plot_item.setYRange(0, 5000)
    p2_intensity_view

    layout.addWidget(p1_intensity_view)
    layout.addWidget(p2_intensity_view)

    window.setLayout(layout)

    window.setWindowTitle("Beam intensity")
    window.show()

    recv_thread = threading.Thread(target=receive_data, daemon=True, args=(p1_intensity_view.update_data, p2_intensity_view.update_data))
    recv_thread.start()

    sys.exit(app.exec_())
