# This Python file uses the following encoding: utf-8
import sys
import os
import asyncio

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
        self.blank(self.ui.editCwl.text(), self.ui.editFwhm.text())

    def on_btnExit_click(self) -> None:
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
        self.ui.lbStatus.setText(self.poly.GetStringMsg(num))

    # FWS - event
    def on_poly_update(self, sender, num: int) -> None:
        self.update_result_status(num)

    # FWS
    def connect(self, path: str) -> None:
        self.ui.btnConnect.setEnabled(False)
        self.ui.lbStatus.setText("trying to connect..")
        num = self.poly.PolyConnect(path)
        self.update_result_status(num)

        if num == 0:
            model = clr.Reference[str]("")
            serial = clr.Reference[str]("")
            portNo = clr.Reference[str]("")
            num = self.poly.GetInforData(model, serial, portNo)
            self.update_result_status(num)

            if num == 0:
                self.ui.lbModel.setText(model)
                self.ui.lbRange.setText(serial)
                self.ui.lbRange.setText(portNo)
                asyncio.create_task(self.check_device_ready())  # waiting

        self.ui.btnConnect.setEnabled(num != 0)

    def disconnect(self, path: str) -> None:
        num = self.poly.Disconnect()
        self.update_result_status(num)
        if num == 0:
            self.setEnableUI(False)

    def go(self, cwl: str, fwhm: str) -> None:
        num = self.poly.SetWavelength(cwl, fwhm)
        self.update_result_status(num)

    def blank(self, cwl: str, fwhm: str) -> None:
        num = self.poly.GoBlankPosition(cwl, fwhm)
        self.update_result_status(num)

    async def check_device_ready(self) -> None:
        is_ready = False

        while not is_ready:
            num = await self.poly.GetDeviceStatus()
            self.update_result_status(num)

            if num == 6:  # ready
                self.setEnableUI(True)
                is_ready = True

            await asyncio.sleep(0.1)  # 100ms

if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = Widget()
    widget.show()
    sys.exit(app.exec())

