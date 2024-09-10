# This Python file uses the following encoding: utf-8
import sys
import random
import threading
import time

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
            QLineEdit:disabled { background-color: lightgray; color: darkgray; }
            QSlider:disabled { color: darkgray; }
        """)

        self.initUI()
        self.offUI()

        self.laserType = ScLaserType.none
        self.isScPortOpen = False
        self.isScPowerOn = False
        self.isScLaserOn = False
        self.frequencies = []

        self.scPortWrapper = ScProWrapper()
        self.scPortWrapper.set_on_msg(self.on_update_msg)

        thread = threading.Thread(target=self.refresh_status)
        thread.start()

    # ui
    def initUI(self) -> None:
        self.ui.btnRefresh.clicked.connect(self.on_btnRefresh_click)
        self.ui.btnPowerOff.clicked.connect(self.on_btnPowerOff_click)
        self.ui.btnPowerOn.clicked.connect(self.on_btnPowerOn_click)
        self.ui.btnLaserOff.clicked.connect(self.on_btnLaserOff_click)
        self.ui.btnLaserOn.clicked.connect(self.on_btnLaserOn_click)
        self.ui.btnFrequency.clicked.connect(self.on_btnFrequency_click)
        self.ui.btnLaserPowerSet.clicked.connect(self.on_btnLaserPowerSet_click)

        self.ui.cbType.currentIndexChanged.connect(self.on_cbType_selectedIndexChanged)
        self.ui.cbType.addItems([laserType.name for laserType in ScLaserType])

        # port와 frequency는 현재 선택된 항목만 알면 되기 때문에 별도 connect를 하지 않음
        for port in serial.tools.list_ports.comports():
            self.ui.cbPort.addItem(port.device)

        self.ui.sliderLaserPower.setMinimum(SC_SETTING.LASER_MIN)
        self.ui.sliderLaserPower.setMaximum(SC_SETTING.LASER_MAX + 1)
        self.ui.sliderLaserPower.valueChanged.connect(self.on_sliderLaserPower_valueChanged)

        self.ui.editPower.setValidator(QIntValidator(SC_SETTING.LASER_MIN, SC_SETTING.LASER_MAX, self))
        self.ui.editPower.setText("0")  # 0으로 초기화
        self.ui.editPower.editingFinished.connect(self.on_editPower_editingFinished)

    def offUI(self) -> None:
        self.ui.btnRefresh.setEnabled(False)
        self.ui.btnPowerOff.setEnabled(False)
        self.ui.btnPowerOn.setEnabled(False)
        self.ui.btnLaserOff.setEnabled(False)
        self.ui.btnLaserOn.setEnabled(False)
        self.ui.btnFrequency.setEnabled(False)
        self.ui.btnLaserPowerSet.setEnabled(False)
        self.ui.cbFrequency.setEnabled(False)
        self.ui.sliderLaserPower.setEnabled(False)
        self.ui.editPower.setEnabled(False)

        # 다른 거는 다 끄고 type, port만 켠다.
        self.ui.cbType.setEnabled(True)
        self.ui.cbPort.setEnabled(True)

        # status는 일단 off로 설정
        self.ui.lbStatus.setText("Off")
        self.ui.lbStatus.setStyleSheet("background-color: #ED8F98; border: 1px solid gray;")

    def on_update_msg(self, msg):
        self.ui.lbMessage.setText(msg)

    def on_btnRefresh_click(self) -> None:
        self.update_status_alarm()

    def on_btnPowerOn_click(self) -> None:
        # power가 켜지면 type, port는 변경 불가
        self.ui.cbType.setEnabled(False)
        self.ui.cbPort.setEnabled(False)

        self.isScPowerOn = True
        # self.isScPowerOn = self.scPortWrapper.power_on()
        # self.on_update_msg(SC_MESSAGE.POWER_ON.format(self.isScPowerOn))

        if self.isScPowerOn:
            self.ui.btnPowerOn.setEnabled(False)
            self.ui.btnPowerOff.setEnabled(True)
            self.ui.btnLaserOn.setEnabled(True)

    def on_btnPowerOff_click(self) -> None:
        # laser power = 0 -> laser off -> power off
        if self.isScLaserOn:
            self.scPortWrapper.set_laser_power(value=0)
            self.ui.sliderLaserPower.setValue(0)
            self.isScLaserOn = not self.scPortWrapper.laser_off()

        self.isScPowerOn = False
        # self.isScPowerOn = not self.scPortWrapper.power_off()
        # self.on_update_msg(SC_MESSAGE.POWER_OFF.format(not self.isScPowerOn))

        if not self.isScPowerOn:
            self.offUI()
            self.ui.btnPowerOn.setEnabled(True)

    def on_btnLaserOn_click(self) -> None:
        # laser가 켜지면 type, port는 변경 불가
        if self.laserType == ScLaserType.SLF:
            self.ui.cbType.setEnabled(False)
            self.ui.cbPort.setEnabled(False)

        self.isScLaserOn = True
        # self.isScLaserOn = self.scPortWrapper.laser_on()
        # self.on_update_msg(SC_MESSAGE.LASER_ON.format(self.isScLaserOn))

        if self.isScLaserOn:
            self.ui.lbStatus.setText("On")
            self.ui.lbStatus.setStyleSheet("background-color: lightgreen; border: 1px solid gray;")

            self.ui.btnLaserOn.setEnabled(False)
            self.ui.btnLaserOff.setEnabled(True)
            self.ui.btnLaserPowerSet.setEnabled(True)
            self.ui.sliderLaserPower.setEnabled(True)
            self.ui.editPower.setEnabled(True)

            if self.laserType == ScLaserType.SLV:
                self.ui.cbFrequency.setEnabled(True)
                self.ui.btnFrequency.setEnabled(True)

    def on_btnLaserOff_click(self) -> None:
        # laser power = 0 -> laser off
        self.scPortWrapper.set_laser_power(value=0)

        self.isScLaserOn = False
        # self.isScLaserOn = not self.scPortWrapper.laser_off()
        # self.on_update_msg(SC_MESSAGE.LASER_OFF.format(not self.isScLaserOn))

        if not self.isScLaserOn:
            self.ui.lbStatus.setText("Off")
            self.ui.lbStatus.setStyleSheet("background-color: #ED8F98; border: 1px solid gray;")

            self.ui.btnLaserOn.setEnabled(True)
            self.ui.btnLaserOff.setEnabled(False)
            self.ui.btnLaserPowerSet.setEnabled(False)
            self.ui.sliderLaserPower.setEnabled(False)
            self.ui.editPower.setEnabled(False)

            if self.laserType == ScLaserType.SLV:
                self.ui.cbFrequency.setEnabled(False)
                self.ui.btnFrequency.setEnabled(False)
            else:
                self.offUI()
                self.ui.btnLaserOn.setEnabled(True)

    def on_btnFrequency_click(self) -> None:
        if self.ui.cbFrequency.currentIndex() > -1:
            value = self.frequencies[self.ui.cbFrequency.currentIndex()]
            result = self.scPortWrapper.setFreq(value=value)
            self.on_update_msg(SC_MESSAGE.UPDATE_LASER_FREQUENCY.format(str(value) if result else "Failed"))

    def on_btnLaserPowerSet_click(self) -> None:
        self.set_laser_power(self.ui.sliderLaserPower.value())

    def on_editPower_editingFinished(self) -> None:
        value = int(self.ui.editPower.text())
        if value >= SC_SETTING.LASER_MIN and value <= SC_SETTING.LASER_MAX:
            # slider와 값이 다를 경우에만 udpate
            if self.ui.sliderLaserPower.value() != value:
                self.ui.sliderLaserPower.setValue(value)
        else:
            # 잘못된 범위면 slider의 값으로 돌림
            QMessageBox.warning(self, "Error", SC_MESSAGE.INVALIED_POWER_VALUE.format(str(value)))
            self.ui.editPower.setText(self.ui.sliderLaserPower.value())

    def on_sliderLaserPower_valueChanged(self, value) -> None:
        # lineedit의 값과 다를 경우에만 udpate
        if int(self.ui.editPower.text()) != value:
            self.ui.editPower.setText(str(value))
        self.set_laser_power(value)

    def on_cbType_selectedIndexChanged(self, index) -> None:
        self.isScPortOpen = False
        self.ui.cbFrequency.clear()
        self.ui.cbFrequency.index = -1
        self.offUI()

        if self.ui.cbType.currentIndex() > 0:
            # if self.ui.cbPort.currentIndex() > -1:
                self.laserType = ScLaserType[self.ui.cbType.currentText()]
                portName = self.ui.cbPort.currentText()
                self.isScPortOpen = True
                # self.isScPortOpen = self.scPortWrapper.open_port(laserType=type, port_name=portName)

                if self.isScPortOpen:
                    self.ui.btnRefresh.setEnabled(True)

                    # SLV에서만 frequency 설정 가능하게
                    if self.laserType == ScLaserType.SLV:
                        self.frequencies = SC_SETTING.FREQUENCY_SLV
                        self.ui.cbFrequency.addItems([f"{freq} MHz" for freq in self.frequencies])
                        self.ui.cbFrequency.index = 0
                        self.ui.btnPowerOn.setEnabled(True)
                    else:
                        self.ui.btnLaserOn.setEnabled(True)
                else:
                    QMessageBox.warning(self, "Error", SC_MESSAGE.ERROR_PORT)
            # else:
            #     QMessageBox.warning(self, "Error", SC_MESSAGE.NO_PORT)
            #     self.ui.cbType.setCurrentIndex(0)

    def set_laser_power(value: int) -> bool:
        result = self.scPortWrapper.set_laser_power(value=value)
        self.on_update_msg(SC_MESSAGE.UPDATE_LASER_POWER.format(str(value) if result else "Failed"))

    def update_status_alarm(self) -> None:
        # (result, status, temperature, powerPercent, frequency) = self.scPortWrapper.query_status()
        result = True
        temperature = random.randint(0, 100)
        if result:
            self.ui.lbTemperature.setText(f"{temperature}°C")

        # (result, err_no, err_code, err_title, err_desc) = self.scPortWrapper.query_alarm()
        val = random.randint(0, 10)
        err_no = val < 5
        err_code = f"CODE{val}"
        if result:
            self.ui.lbError.setText(err_code)
            if err_no:
                self.ui.lbError.setStyleSheet("background-color: lightgreen; border: 1px solid gray;")
            else:
                self.ui.lbError.setStyleSheet("background-color: #ED8F98; border: 1px solid gray;")

    def refresh_status(self):
        while True:
            if self.isScPortOpen:
                self.update_status_alarm()
            time.sleep(SC_SETTING.REFRESH_DELAY_SECOND)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = SlPicoSample()
    widget.show()
    sys.exit(app.exec())
