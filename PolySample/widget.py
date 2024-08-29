# This Python file uses the following encoding: utf-8
import sys

from PySide6.QtWidgets import QApplication, QWidget, QFileDialog

# Important:
# You need to run the following command to generate the ui_form.py file
#     pyside6-uic form.ui -o ui_form.py, or
#     pyside2-uic form.ui -o ui_form.py
from ui_form import Ui_Widget

class Widget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
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

    def on_btnPath_click(self) -> None:
        file_name, _ = QFileDialog.getOpenFileName(self, "Open ISM File", "", "ISM files (*.ism)", options=QFileDialog.Options())
        if file_name:
            self.ui.lbPath.setText(file_name)

    def on_btnConnect_click(self) -> None:
        self.ui.lbStatus.setText("click connect")
        #self.setEnableUI(False)

    def on_btnDisconnect_click(self) -> None:
        self.ui.lbStatus.setText("click disconnect")

    def on_btnGo_click(self) -> None:
        self.ui.lbStatus.setText("click go, cwl: " + self.ui.editCwl.text() + ", fwhm: " + self.ui.editFwhm.text())

    def on_btnBlank_click(self) -> None:
        self.ui.lbStatus.setText("click blank")

    def on_btnExit_click(self) -> None:
        self.ui.lbStatus.setText("click exit")

    def setEnableUI(self, enable: bool) -> None:
        self.ui.btnPath.setEnabled(enable)
        self.ui.btnConnect.setEnabled(enable)
        self.ui.btnDisconnect.setEnabled(enable)
        self.ui.btnGo.setEnabled(enable)
        self.ui.btnBlank.setEnabled(enable)
        self.ui.btnExit.setEnabled(enable)
        self.ui.editCwl.setEnabled(enable)
        self.ui.editFwhm.setEnabled(enable)



if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = Widget()
    widget.show()
    sys.exit(app.exec())
