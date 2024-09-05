# This Python file uses the following encoding: utf-8
import sys

from PySide6.QtWidgets import QApplication, QWidget

# Important:
# You need to run the following command to generate the ui_form.py file
#     pyside6-uic form.ui -o ui_form.py, or
#     pyside2-uic form.ui -o ui_form.py
from ui_form import Ui_SlPicoSample

class SlPicoSample(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_SlPicoSample()
        self.ui.setupUi(self)

        self.setStyleSheet("""
            QPushButton:disabled { background-color: lightgray; color: darkgray; }
            QComboBox:disabled { background-color: lightgray; color: darkgray; }
            QPlainTextEdit:disabled { background-color: lightgray; color: darkgray; }
            QSlider:disabled { background-color: lightgray; color: darkgray; }
        """)

        self.initUI()

    # ui
    def initUI(self) -> None:
        self.ui.btnRefresh.clicked.connect(self.on_btnRefresh_click)
        self.ui.btnPowerOff.clicked.connect(self.on_btnPowerOff_click)
        self.ui.btnPowerOn.clicked.connect(self.on_btnPowerOn_click)
        self.ui.btnLaserOff.clicked.connect(self.on_btnLaserOff_click)
        self.ui.btnLaserOn.clicked.connect(self.on_btnLaserOn_click)
        self.ui.btnFrequency.clicked.connect(self.on_btnFrequency_click)
        self.ui.btnPowerSet.clicked.connect(self.on_btnPowerSet_click)

        self.ui.cbType.currentIndexChanged.connect(self.on_cbType_selectedIndexChanged)
        self.ui.cbType.addItems(["Type 1", "Type 2", "Type 3"])

        self.ui.cbPort.currentIndexChanged.connect(self.on_cbPort_selectedIndexChanged)
        self.ui.cbPort.addItems(["Port 1", "Port 2", "Port 3"])

        self.ui.cbFrequency.currentIndexChanged.connect(self.on_cbFrequency_selectedIndexChanged)
        self.ui.cbFrequency.addItems(["Freq 1", "Freq 2", "Freq 3"])

        self.ui.sliderLaserPower.sliderMoved.connect(self.on_sliderLaserPower_sliderMoved)
        self.ui.sliderLaserPower.setMinimum(0)
        self.ui.sliderLaserPower.setMaximum(100)

    def setEnableUI(self, enable: bool) -> None:
        self.ui.btnRefresh.setEnabled(enable)
        self.ui.btnPowerOff.setEnabled(enable)
        self.ui.btnPowerOn.setEnabled(enable)
        self.ui.btnLaserOff.setEnabled(enable)
        self.ui.btnLaserOn.setEnabled(enable)
        self.ui.btnFrequency.setEnabled(enable)
        self.ui.btnPowerSet.setEnabled(enable)

        self.ui.cbFrequency.setEnabled(enable)
        self.ui.sliderLaserPower.setEnabled(enable)

    def on_btnRefresh_click(self) -> None:
        self.ui.lbMessage.setText("on_btnRefresh_click")

    def on_btnPowerOff_click(self) -> None:
        self.ui.lbMessage.setText("on_btnPowerOff_click")

    def on_btnPowerOn_click(self) -> None:
        self.ui.lbMessage.setText("on_btnPowerOn_click")

    def on_btnLaserOff_click(self) -> None:
        self.ui.lbMessage.setText("on_btnLaserOff_click")

    def on_btnLaserOn_click(self) -> None:
        self.ui.lbMessage.setText("on_btnLaserOn_click")

    def on_btnFrequency_click(self) -> None:
        self.ui.lbMessage.setText("on_btnFrequency_click")

    def on_btnPowerSet_click(self) -> None:
        self.ui.lbMessage.setText("on_btnPowerSet_click")

    def on_cbType_selectedIndexChanged(self, index) -> None:
        self.ui.lbMessage.setText(f"on_cbType_selectedIndexChanged index: {index}, value: {self.ui.cbType.currentText()}")

    def on_cbPort_selectedIndexChanged(self, index) -> None:
        self.ui.lbMessage.setText(f"on_cbPort_selectedIndexChanged index: {index}, value: {self.ui.cbPort.currentText()}")

    def on_cbFrequency_selectedIndexChanged(self, index) -> None:
        self.ui.lbMessage.setText(f"on_cbFrequency_selectedIndexChanged index: {index}, value: {self.ui.cbFrequency.currentText()}")

    def on_sliderLaserPower_sliderMoved(self, position) -> None:
        self.ui.lbMessage.setText(f"on_sliderLaserPower_sliderMoved to position: {position}")



if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = SlPicoSample()
    widget.show()
    sys.exit(app.exec())
