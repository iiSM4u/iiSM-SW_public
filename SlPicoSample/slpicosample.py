# This Python file uses the following encoding: utf-8
import sys

# pip install pyserial
import serial.tools.list_ports

from sc_laser_type import ScLaserType
from sc_const_message import SC_MESSAGE
from sc_const_code import SC_CODE
from sc_const_setting import SC_SETTING
from sc_port_wrapper import ScProWrapper

from PySide6.QtWidgets import QApplication, QWidget, QMessageBox
from PySide6.QtGui import QIntValidator

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
            QSlider:disabled { color: darkgray; }
        """)

        self.isReady = False  # 초기화시에만 사용
        self.initUI()
        self.setEnableUI(False)

        self.isScPortOpen = False
        self.isScPowerOn = False
        self.isScLaserOn = False
        self.frequencies = []

        self.scPortWrapper = ScProWrapper()
        self.scPortWrapper.set_on_msg(self.on_update_msg)

        self.isReady = True

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
        self.ui.cbType.addItems([laserType.name for laserType in ScLaserType])

        # port와 frequency는 현재 선택된 항목만 알면 되기 때문에 별도 connect를 하지 않음
        for port in serial.tools.list_ports.comports():
            self.ui.cbPort.addItem(port.device)

        self.ui.sliderLaserPower.setMinimum(SC_SETTING.LASER_MIN)
        self.ui.sliderLaserPower.setMaximum(SC_SETTING.LASER_MAX)
        self.ui.sliderLaserPower.sliderMoved.connect(self.on_sliderLaserPower_sliderMoved)

        self.ui.editPower.setValidator(QIntValidator(SC_SETTING.LASER_MIN, SC_SETTING.LASER_MAX, self))
        self.ui.editPower.editingFinished.connect(self.on_editPower_editingFinished)

    def setEnableUI(self, enable: bool) -> None:
        self.ui.btnRefresh.setEnabled(enable)
        self.ui.btnPowerOff.setEnabled(enable)
        self.ui.btnPowerOn.setEnabled(enable)
        self.ui.btnLaserOff.setEnabled(enable)
        self.ui.btnLaserOn.setEnabled(enable)
        self.ui.btnFrequency.setEnabled(enable)
        self.ui.btnPowerSet.setEnabled(enable)

        self.ui.cbFrequency.setEnabled(enable)
        # self.ui.sliderLaserPower.setEnabled(enable)
        # self.ui.editPower.setEnabled(enable)

    def on_update_msg(self, msg):
        self.ui.lbMessage.setText(msg)

    def on_btnRefresh_click(self) -> None:
        self.ui.lbMessage.setText("on_btnRefresh_click")

    def on_btnPowerOff_click(self) -> None:
        # laser power = 0 -> laser off -> power off
        self.scPortWrapper.set_laser_power(value=0)
        self.isScLaserOn = not self.scPortWrapper.laser_off()
        self.isScPowerOn = not self.scPortWrapper.power_off()
        self.on_update_msg(SC_MESSAGE.POWER_OFF.format(not self.isScPowerOn))

    def on_btnPowerOn_click(self) -> None:
        self.isScPowerOn = self.scPortWrapper.power_on()
        self.on_update_msg(SC_MESSAGE.POWER_ON.format(self.isScPowerOn))

    def on_btnLaserOff_click(self) -> None:
        # laser power = 0 -> laser off
        self.scPortWrapper.set_laser_power(value=0)
        self.isScLaserOn = not self.scPortWrapper.laser_off()
        self.on_update_msg(SC_MESSAGE.LASER_OFF.format(not self.isScLaserOn))

    def on_btnLaserOn_click(self) -> None:
        self.isScLaserOn = self.scPortWrapper.laser_on()
        self.on_update_msg(SC_MESSAGE.LASER_ON.format(self.isScLaserOn))

    def on_btnFrequency_click(self) -> None:
        if self.ui.cbFrequency.currentIndex() > -1:
            value = self.frequencies[self.ui.cbFrequency.currentIndex()]
            result = self.scPortWrapper.setFreq(value=value)
            self.on_update_msg(SC_MESSAGE.UPDATE_LASER_FREQUENCY.format(str(value) if result else "Failed"))

    def on_btnPowerSet_click(self) -> None:
        self.set_laser_power(self.ui.sliderLaserPower.value())

    def on_editPower_editingFinished(self) -> None:
        value = int(self.ui.editPower.text())
        if value >= SC_SETTING.LASER_MIN and value <= SC_SETTING.LASER_MIN:
            self.ui.sliderLaserPower.setValue(value)
        else:
            # 잘못된 범위면 slider의 값으로 돌림
            QMessageBox.warning(self, "Error", SC_MESSAGE.INVALIED_POWER_VALUE.format(str(value)))
            self.ui.editPower.setText(self.ui.sliderLaserPower.value())

    def on_sliderLaserPower_sliderMoved(self, position) -> None:
        self.ui.editPower.setText(self.ui.sliderLaserPower.value())
        self.set_laser_power(self.ui.sliderLaserPower.value())

    def on_cbType_selectedIndexChanged(self, index) -> None:
        # 맨 처음 시작할 때 실행 안되게
        if self.isReady:
            if self.ui.cbPort.currentIndex() > -1:
                type = ScLaserType[self.ui.cbType.currentText()]
                portName = self.ui.cbPort.currentText()  # 테스트 필요

                # 일단 combobox를 비운다
                self.ui.cbFrequency.clear()

                # SLV에서만 frequency 설정 가능하게
                if type == ScLaserType.SLV:
                    self.frequencies = SC_SETTING.FREQUENCY_SLV
                    self.ui.cbFrequency.addItems([f"{freq} MHz" for freq in self.frequencies])
                    self.ui.cbFrequency.index = 0
                    self.ui.cbFrequency.setEnabled(True)
                else:
                    self.ui.cbFrequency.index = -1
                    self.ui.cbFrequency.setEnabled(False)

                self.isScPortOpen = self.scPortWrapper.open_port(laserType=type, port_name=portName)
            else:
                QMessageBox.warning(self, "Error", SC_MESSAGE.NO_PORT)

    def set_laser_power(value: int) -> bool:
        result = self.scPortWrapper.set_laser_power(value=value)
        self.on_update_msg(SC_MESSAGE.UPDATE_LASER_POWER.format(str(value) if result else "Failed"))

if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = SlPicoSample()
    widget.show()
    sys.exit(app.exec())
