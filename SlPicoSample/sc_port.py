import time
import serial
from serial import SerialException

class ScPort:
    def __init__(self):
        self.OnMSG = None  # 콜백 함수
        self.IsBusy = False
        self.port = serial.Serial()

    def set_on_msg(self, callback):
        self.OnMSG = callback

    def open_port(self, port_name: str, baud_rate: int):
        try:
            self.port.port = port_name
            self.port.baudrate = baud_rate
            self.port.open()

            return self.port.is_open
        except SerialException as ex:
            if self.OnMSG:
                self.OnMSG(str(ex))
            return False

    def close_port(self):
        if self.port.is_open:
            self.port.close()

    def send_cmd(self, cmd):
        if self.port.is_open:
            self.IsBusy = True
            self.port.write(cmd)
            self.IsBusy = False
            return True
        else:
            if self.OnMSG:
                self.OnMSG("port is not opened")
            return False

    def read(self):
        if self.port.is_open:
            self.IsBusy = True
            result = []
            counter = 0

            while True:
                time.sleep(0.1)  # 일정 시간 대기

                if self.port.in_waiting > 0:  # 읽을 수 있는 데이터가 있는지 확인
                    result.append(self.port.read(1)[0])  # 한 바이트 읽기
                    counter = 0
                else:
                    if counter > 1:
                        break

                counter += 1

            self.IsBusy = False
            return bytes(result)
        else:
            if self.OnMSG:
                self.OnMSG("port is not opened")
            return bytes()
