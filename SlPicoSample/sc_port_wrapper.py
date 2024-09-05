import time

from sc_port import ScPort
from sc_laser_type import ScLaserType
from sc_status_type import ScStatusType
from sc_const_code import SC_CODE

class ScProWrapper:
    def __init__(self):
        self.OnMSG = None
        self.scPort = None
        self.currentType = ScLaserType.none

    def set_on_msg(self, callback):
        self.OnMSG = callback

    def open_port(self, laserType: ScLaserType, port_name: str, baud_rate=9600):
        self.currentType = laserType

        # 이전에 있었으면 close
        if self.scPort is not None:
            self.close_port()

        self.scPort = ScPort()
        if self.OnMSG:
            self.scPort.set_on_msg(self.OnMSG)

        if self.scPort.open_port(port_name=port_name, baud_rate=baud_rate):
            return True

        # open에 실패했으면 null로 돌린다.
        self.close_port()
        if self.OnMSG:
            self.OnMSG("Initialize Failed")
        return False

    def close_port(self):
        if self.scPort is not None:
            self.currentType = ScLaserType.none
            self.laser_off()
            self.power_off()

            while self.scPort.IsBusy:
                time.sleep(0.1)  # 비동기 작업 대기

            self.scPort.close_port()
            self.scPort = None

    def power_on(self) -> bool:
        # power on/off는 SLV 타입에서만 유효하다
        if self.scPort is not None:
            if self.currentType == ScLaserType.SLV:
                cmd = self.make_send_query(func=SC_CODE.POWER_ON_CODE, data1=SC_CODE.POWER_ON_DATA_1, data2=SC_CODE.POWER_ON_DATA_2)
                return self.scPort.send_cmd(cmd)
        return False

    def power_off(self) -> bool:
        # power on/off는 SLV 타입에서만 유효하다
        if self.scPort is not None:
            if self.currentType == ScLaserType.SLV:
                cmd = self.make_send_query(func=SC_CODE.POWER_OFF_CODE, data1=SC_CODE.POWER_OFF_DATA_1, data2=SC_CODE.POWER_OFF_DATA_2)
                return self.scPort.send_cmd(cmd)
        return False

    def laser_on(self) -> bool:
        if self.scPort is not None:
            cmd = self.make_send_query(func=SC_CODE.LIGHT_ON_CODE, data1=SC_CODE.LIGHT_ON_DATA_1, data2=SC_CODE.LIGHT_ON_DATA_2)

            if self.scPort.send_cmd(cmd):
                response = self.scPort.read()
                if len(response) == 8:
                    # 입력한 값과 reponse 값이 일치하면 성공. response에서 [0, 255]로 값이 들어오기 때문에 16진수를 int로 바꿔서 비교해야 함
                    return response[5] == SC_CODE.LIGHT_ON_DATA_1 and response[6] == SC_CODE.LIGHT_ON_DATA_2;
        return False

    def laser_off(self) -> bool:
        if self.scPort is not None:
            cmd = self.make_send_query(func=SC_CODE.LIGHT_OFF_CODE, data1=SC_CODE.LIGHT_OFF_DATA_1, data2=SC_CODE.LIGHT_OFF_DATA_2)

            if self.scPort.send_cmd(cmd):
                response = self.scPort.read()
                if len(response) == 8:
                    # 입력한 값과 reponse 값이 일치하면 성공. response에서 [0, 255]로 값이 들어오기 때문에 16진수를 int로 바꿔서 비교해야 함
                    return response[5] == SC_CODE.LIGHT_OFF_DATA_1 and response[6] == SC_CODE.LIGHT_OFF_DATA_2;
        return False

    def set_laser_power(self, value: int) -> bool:
        if self.scPort is not None:
            if 0 < value < 100:
                # 이게 예전 코드에 있던 방식. 일단 이대로 쓴다.
                cmd = self.make_send_query(func=SC_CODE.SET_POWER_CODE, data1=(value & 0xff), data2=((~(0x0a + value) + 1) & 0xff));

                # 매뉴얼에서는 7번째 자리를 그냥 00으로 채움
                # cmd = self.make_send_query(func: SC_CODE.SET_POWER_CODE, data1: (byte)value, data2: SC_CODE.SET_POWER_DATA_2);
                # cmd = new byte[] { 0x55, 0xaa, 0x01, 0x08, 0x01, (byte)value, 0x00, 0x0d };

                if self.scPort.send_cmd(cmd):
                    response = self.scPort.read()
                    if len(response) == 8:
                        # 입력한 값과 reponse 값이 일치하면 성공
                        return response[5] == value
        return False



    def query_status(self):
        if self.scPort is not None:
            cmd = self.make_send_query(func=SC_CODE.QUERY_STATUS_CODE, data1=SC_CODE.QUERY_STATUS_DATA_1, data2=SC_CODE.QUERY_STATUS_DATA_2)

            if self.scPort.send_cmd(cmd):
                response = self.scPort.read()
                if len(response) == 8:
                    status = self.convert_sc_status(value=response[2]);
                    temperature = response[4];
                    powerPercent = response[5];
                    frequencyIndex = response[6];
                    frequency = SC_SETTING.FREQUENCY[frequencyIndex] if frequencyIndex < len(SC_SETTING.FREQUENCY) else 0
                    return (True, status, temperature, powerPercent, frequency)
        return (False, ScStatusType.none, 0, 0, 0.0)

    def query_alarm(self):
        if self.scPort is not None:
            cmd = self.make_send_query(func=SC_CODE.QUERY_ALARM_CODE, data1=SC_CODE.QUERY_ALARM_DATA_1, data2=SC_CODE.QUERY_ALARM_DATA_2)

            if self.scPort.send_cmd(cmd):
                response = self.scPort.read()
                if len(response) == 8:
                    err_code, err_title, err_desc = self.convert_sc_error(code=response[5], type=typeresponse[6])
                    return (True, err_code, err_title, err_desc)
        return (False, "", "", "")

    def make_send_query(self, func: int, data1: int, data2: int):
        return [0x55, 0xaa, 0x01, func, 0x01, data1, data2, 0x0d]

    def convert_sc_status(self, value: int) -> ScStatusType:
        if value == 0:
            return ScStatusType.Off
        elif value >= 1 and value <= 5:
            return ScStatusType.Ready
        elif value == 9:
            return ScStatusType.Alarm
        else:
            return ScStatusType.On

