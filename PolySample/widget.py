# This Python file uses the following encoding: utf-8
import sys
import os
import time
import datetime

# pip install pythonnet
import clr

# dll directory path. (default current)
sys.path.append(".")

# assembly name
clr.AddReference("PolyDll")

from PySide6.QtWidgets import QApplication, QWidget, QFileDialog

# Important:
# You need to run the following command to generate the ui_form.py file
#     pyside6-uic form.ui -o ui_form.py, or
#     pyside2-uic form.ui -o ui_form.py
from ui_form import Ui_Widget
from ISM_Device import ClassPoly  # namespace and class name

class Widget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.initUI()
        self.poly = ClassPoly()
        self.poly.EventPolyUiUpdate += self.on_poly_update
        self.isConnected = False

    # buttons
    def on_btnPath_click(self) -> None:
        file_name, _ = QFileDialog.getOpenFileName(self, "Open ISM File", "", "ISM files (*.ism)", options=QFileDialog.Options())
        if file_name:
            file_name = os.path.normpath(file_name)
            self.ui.lbPath.setText(file_name)
            self.ui.btnConnect.setEnabled(True)

    def on_btnConnect_click(self) -> None:
        self.connect(self.ui.lbPath.text())

    def on_btnDisconnect_click(self) -> None:
        self.disconnect()

    def on_btnGo_click(self) -> None:
        self.go(self.ui.editCwl.text(), self.ui.editFwhm.text())

    def on_btnBlank_click(self) -> None:
        self.blank()

    def on_btnExit_click(self) -> None:
        if self.isConnected:
            self.disconnect()
        QApplication.quit()

    # ui
    def initUI(self):
        self.ui = Ui_Widget()
        self.ui.setupUi(self)

        self.setStyleSheet("""
            QPushButton:disabled { background-color: lightgray; color: darkgray; }
            QLineEdit:disabled { background-color: lightgray; color: darkgray; }
        """)

        self.ui.btnPath.clicked.connect(self.on_btnPath_click)
        self.ui.btnConnect.clicked.connect(self.on_btnConnect_click)
        self.ui.btnDisconnect.clicked.connect(self.on_btnDisconnect_click)
        self.ui.btnGo.clicked.connect(self.on_btnGo_click)
        self.ui.btnBlank.clicked.connect(self.on_btnBlank_click)
        self.ui.btnExit.clicked.connect(self.on_btnExit_click)

        self.setEnableUI(False)
        self.ui.btnConnect.setEnabled(False)

    def setEnableUI(self, isConnected: bool) -> None:
        self.ui.btnConnect.setEnabled(isConnected == False)
        self.ui.btnDisconnect.setEnabled(isConnected)
        self.ui.btnGo.setEnabled(isConnected)
        self.ui.btnBlank.setEnabled(isConnected)
        self.ui.editCwl.setEnabled(isConnected)
        self.ui.editFwhm.setEnabled(isConnected)

    def update_result_status(self, num: int) -> None:
        msg = self.poly.GetStringMsg(num)
        self.ui.lbStatus.setText(msg)

    # FWS - event
    def on_poly_update(self, sender, num: int) -> None:
        self.update_result_status(num)

    # FWS
    def connect(self, path: str) -> None:
        #self.log_to_file(f"Attempting to connect with path: {path}")
        self.ui.btnConnect.setEnabled(False)
        self.ui.lbStatus.setText("trying to connect..")
        num = self.poly.PolyConnect(path)
        self.update_result_status(num)

        if num == 0:
            model = ""
            serialNo = ""
            waveRange = ""
            num, model, serialNo, waveRange = self.poly.GetInforData(model, serialNo, waveRange)
            self.update_result_status(num)

            if num == 0:
                self.ui.lbModel.setText(model)
                self.ui.lbSerialNo.setText(serialNo)
                self.ui.lbWaveRange.setText(waveRange)
                self.check_device_ready()  # waiting

        self.ui.btnConnect.setEnabled(num != 0)

    def disconnect(self) -> None:
        num = self.poly.Disconnect()
        self.update_result_status(num)
        if num == 10:  # closed
            self.setEnableUI(False)

    def go(self, cwl: str, fwhm: str) -> None:
        num = self.poly.SetWavelength(str(cwl), str(fwhm))
        self.update_result_status(num)

    def blank(self) -> None:
        num = self.poly.GoBlankPosition()
        self.update_result_status(num)

    def check_device_ready(self) -> None:
        self.isConnected = False

        while not self.isConnected:
            num = self.poly.GetDeviceStatus()
            self.update_result_status(num)

            if num == 6:  # ready
                self.setEnableUI(True)
                self.isConnected = True

            time.sleep(0.1)

    def log_to_file(self, message: str) -> None:
        log_dir = "logs"
        os.makedirs(log_dir, exist_ok=True)
        log_file = os.path.join(log_dir, f"{datetime.date.today()}.log")

        with open(log_file, "a") as f:
            timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            f.write(f"[{timestamp}] {message}\n")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = Widget()
    widget.show()
    sys.exit(app.exec())

