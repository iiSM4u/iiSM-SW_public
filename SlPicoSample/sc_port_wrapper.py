import time
from typing import Tuple
from typing import List

from sc_port import ScPort
from sc_laser_type import ScLaserType
from sc_status_type import ScStatusType
from sc_error_type import ScErrorType
from sc_const_code import SC_CODE
from sc_const_setting import SC_SETTING
from sc_const_message import SC_MESSAGE

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
                time.sleep(0.1)  # 작업 대기

            self.scPort.close_port()
            self.scPort = None

    def power_on(self) -> bool:
        # power on/off는 SLV 타입에서만 유효하다
        if self.scPort is not None:
            if self.currentType == ScLaserType.SLV:
                cmd = self.make_send_query(function=SC_CODE.POWER_ON_CODE, data1=SC_CODE.POWER_ON_DATA_1, data2=SC_CODE.POWER_ON_DATA_2)
                return self.scPort.send_cmd(cmd)
        return False

    def power_off(self) -> bool:
        # power on/off는 SLV 타입에서만 유효하다
        if self.scPort is not None:
            if self.currentType == ScLaserType.SLV:
                cmd = self.make_send_query(function=SC_CODE.POWER_OFF_CODE, data1=SC_CODE.POWER_OFF_DATA_1, data2=SC_CODE.POWER_OFF_DATA_2)
                return self.scPort.send_cmd(cmd)
        return False

    def laser_on(self) -> bool:
        if self.scPort is not None:
            cmd = self.make_send_query(function=SC_CODE.LIGHT_ON_CODE, data1=SC_CODE.LIGHT_ON_DATA_1, data2=SC_CODE.LIGHT_ON_DATA_2)

            if self.scPort.send_cmd(cmd):
                response = self.scPort.read()
                if len(response) == 8:
                    # 입력한 값과 reponse 값이 일치하면 성공. response에서 [0, 255]로 값이 들어오기 때문에 16진수를 int로 바꿔서 비교해야 함
                    return response[5] == SC_CODE.LIGHT_ON_DATA_1 and response[6] == SC_CODE.LIGHT_ON_DATA_2;
        return False

    def laser_off(self) -> bool:
        if self.scPort is not None:
            cmd = self.make_send_query(function=SC_CODE.LIGHT_OFF_CODE, data1=SC_CODE.LIGHT_OFF_DATA_1, data2=SC_CODE.LIGHT_OFF_DATA_2)

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
                cmd = self.make_send_query(function=SC_CODE.SET_POWER_CODE, data1=(value & 0xff), data2=((~(0x0a + value) + 1) & 0xff));

                # 매뉴얼에서는 7번째 자리를 그냥 00으로 채움
                # cmd = self.make_send_query(func: SC_CODE.SET_POWER_CODE, data1: (byte)value, data2: SC_CODE.SET_POWER_DATA_2);
                # cmd = new byte[] { 0x55, 0xaa, 0x01, 0x08, 0x01, (byte)value, 0x00, 0x0d };

                if self.scPort.send_cmd(cmd):
                    response = self.scPort.read()
                    if len(response) == 8:
                        # 입력한 값과 reponse 값이 일치하면 성공
                        return response[5] == value
        return False

    def query_status(self) -> Tuple[bool, ScStatusType, int, int, float]:
        if self.scPort is not None:
            cmd = self.make_send_query(function=SC_CODE.QUERY_STATUS_CODE, data1=SC_CODE.QUERY_STATUS_DATA_1, data2=SC_CODE.QUERY_STATUS_DATA_2)

            if self.scPort.send_cmd(cmd):
                response = self.scPort.read()
                if len(response) == 8:
                    status = self.convert_sc_status(value=response[2]);
                    temperature = response[4];
                    powerPercent = response[5];
                    frequencyIndex = response[6];
                    frequency = SC_SETTING.FREQUENCY[frequencyIndex] if frequencyIndex < len(SC_SETTING.FREQUENCY) else 0
                    return True, status, temperature, powerPercent, frequency
        return False, ScStatusType.none, 0, 0, 0.0

    def query_alarm(self) -> Tuple[bool, str, str, str, ScErrorType]:
        if self.scPort is not None:
            cmd = self.make_send_query(function=SC_CODE.QUERY_ALARM_CODE, data1=SC_CODE.QUERY_ALARM_DATA_1, data2=SC_CODE.QUERY_ALARM_DATA_2)

            if self.scPort.send_cmd(cmd):
                response = self.scPort.read()
                if len(response) == 8:
                    err_code, err_title, err_desc, err_type = self.convert_sc_error(code=response[5], type=response[6])
                    return True, err_code, err_title, err_desc, err_type
        return False, "", "", "", ScErrorType.none

    # 이게 매뉴얼 상의 구현된 frequency - 1~12까지의 index를 16진수 형식으로 넣는다.
    def set_freq(self, index: int) -> bool:
        if self.scPort is not None:
            if index > 0 and index < 13:
                cmd = self.make_send_query(function=SC_CODE.SET_FREQUENCY_CODE, data1=SC_CODE.SET_FREQUENCY_DATA_1, data2=index)

                if self.scPort.send_cmd(cmd):
                    response = self.scPort.read()
                    if len(response) == 8:
                        # 입력한 값과 reponse 값이 일치하면 성공
                        return response[6] == index
        return False

    # 매뉴얼 상의 frequency는 7번째 자리에 [01, 02, 03, 04, 05, 06, 07, 08, 09, 0A, 0B, 0C] 중의 하나를 설정하게 되어 있는데, 기존 코드는 전혀 다르게 구현되어 있음. 일단 그대로 둔다
    def set_freq_SLF(self, freq: float) -> bool:
        if self.scPort is not None:
            data1 = freq * 10.0
            data2 = 251 - data1
            cmd = self.make_send_query(function=SC_CODE.SET_FREQUENCY_CODE, data1=data1, data2=data2)

            if self.scPort.send_cmd(cmd):
                response = self.scPort.read()
                return len(response) == 8
        return False

    def set_freq_SLV(self, freq: float) -> bool:
        if self.scPort is not None:
            cmd = self.make_send_query(function=SC_CODE.SET_FREQUENCY_CODE, data1=0x00, data2=0x00)

            if abs(freq - 0.01) < 0.0000001:
                cmd[3] = 0xc1
                cmd[4] = 0x01
                cmd[5] = 0x02
                cmd[6] = 0x3b
            elif abs(freq - 0.1) < 0.0000001:
                cmd[5] = 0
                cmd[6] = 0x0a
            elif abs(freq - 0.2) < 0.0000001:
                cmd[5] = 0
                cmd[6] = 0x14
            elif abs(freq - 0.5) < 0.0000001:
                cmd[5] = 0
                cmd[6] = 0x32
            elif abs(freq - 1) < 0.0000001:
                cmd[5] = 0
                cmd[6] = 0x64
            elif abs(freq - 4) < 0.0000001:
                cmd[5] = 1
                cmd[6] = 0x90
            elif abs(freq - 5) < 0.0000001:
                cmd[5] = 1
                cmd[6] = 0xf4
            elif abs(freq - 10) < 0.0000001:
                cmd[5] = 3
                cmd[6] = 0xe8
            elif abs(freq - 20) < 0.0000001:
                cmd[5] = 7
                cmd[6] = 0xd0
            elif abs(freq - 40) < 0.0000001:
                cmd[5] = 0x0f
                cmd[6] = 0xa0
            elif abs(freq - 80) < 0.0000001:
                cmd[5] = 0x1f
                cmd[6] = 0x40
            elif abs(freq - 200) < 0.0000001:
                cmd[5] = 0x4e
                cmd[6] = 0x20
            else:
                return False

            if self.scPort.send_cmd(cmd):
                response = self.scPort.read()
                return len(response) == 8
        return False

    def make_send_query(self, function: int, data1: int, data2: int) -> List[int]:
        return [SC_CODE.HEADER_1, SC_CODE.HEADER_2, SC_CODE.SPACING, function, SC_CODE.SPACING, data1, data2, SC_CODE.FRAME_END]

    def convert_sc_status(self, value: int) -> ScStatusType:
        if value == 0:
            return ScStatusType.Off
        elif value >= 1 and value <= 5:
            return ScStatusType.Ready
        elif value == 9:
            return ScStatusType.Alarm
        else:
            return ScStatusType.On

    def convert_sc_error(self, code: int, type: int) -> Tuple[str, str, str, ScErrorType]:
        if code == SC_CODE.ERROR_00_CODE and type == SC_CODE.ERROR_00_TYPE:
            return [SC_MESSAGE.ERROR_00_CODE, SC_MESSAGE.ERROR_00_TITLE, SC_MESSAGE.ERROR_00_DESCRIPTION, ScErrorType.NoError]
        elif code == SC_CODE.ERROR_01_CODE and type == SC_CODE.ERROR_01_TYPE:
            return [SC_MESSAGE.ERROR_01_CODE, SC_MESSAGE.ERROR_01_TITLE, SC_MESSAGE.ERROR_01_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_03_CODE and type == SC_CODE.ERROR_03_TYPE:
            return [SC_MESSAGE.ERROR_03_CODE, SC_MESSAGE.ERROR_03_TITLE, SC_MESSAGE.ERROR_03_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_04_CODE and type == SC_CODE.ERROR_04_TYPE:
            return [SC_MESSAGE.ERROR_04_CODE, SC_MESSAGE.ERROR_04_TITLE, SC_MESSAGE.ERROR_04_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_06_CODE and type == SC_CODE.ERROR_06_TYPE:
            return [SC_MESSAGE.ERROR_06_CODE, SC_MESSAGE.ERROR_06_TITLE, SC_MESSAGE.ERROR_06_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_07_CODE and type == SC_CODE.ERROR_07_TYPE:
            return [SC_MESSAGE.ERROR_07_CODE, SC_MESSAGE.ERROR_07_TITLE, SC_MESSAGE.ERROR_07_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_08_CODE and type == SC_CODE.ERROR_08_TYPE:
            return [SC_MESSAGE.ERROR_08_CODE, SC_MESSAGE.ERROR_08_TITLE, SC_MESSAGE.ERROR_08_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_09_CODE and type == SC_CODE.ERROR_09_TYPE:
            return [SC_MESSAGE.ERROR_09_CODE, SC_MESSAGE.ERROR_09_TITLE, SC_MESSAGE.ERROR_09_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_0A_CODE and type == SC_CODE.ERROR_0A_TYPE:
            return [SC_MESSAGE.ERROR_10_CODE, SC_MESSAGE.ERROR_10_TITLE, SC_MESSAGE.ERROR_10_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_11_CODE and type == SC_CODE.ERROR_11_TYPE:
            return [SC_MESSAGE.ERROR_11_CODE, SC_MESSAGE.ERROR_11_TITLE, SC_MESSAGE.ERROR_11_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_19_CODE and type == SC_CODE.ERROR_19_TYPE:
            return [SC_MESSAGE.ERROR_19_CODE, SC_MESSAGE.ERROR_19_TITLE, SC_MESSAGE.ERROR_19_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_29_CODE and type == SC_CODE.ERROR_29_TYPE:
            return [SC_MESSAGE.ERROR_29_CODE, SC_MESSAGE.ERROR_29_TITLE, SC_MESSAGE.ERROR_29_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_31_CODE and type == SC_CODE.ERROR_31_TYPE:
            return [SC_MESSAGE.ERROR_31_CODE, SC_MESSAGE.ERROR_31_TITLE, SC_MESSAGE.ERROR_31_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_39_CODE and type == SC_CODE.ERROR_39_TYPE:
            return [SC_MESSAGE.ERROR_39_CODE, SC_MESSAGE.ERROR_39_TITLE, SC_MESSAGE.ERROR_39_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_41_CODE and type == SC_CODE.ERROR_41_TYPE:
            return [SC_MESSAGE.ERROR_41_CODE, SC_MESSAGE.ERROR_41_TITLE, SC_MESSAGE.ERROR_41_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_49_CODE and type == SC_CODE.ERROR_49_TYPE:
            return [SC_MESSAGE.ERROR_49_CODE, SC_MESSAGE.ERROR_49_TITLE, SC_MESSAGE.ERROR_49_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_51_CODE and type == SC_CODE.ERROR_51_TYPE:
            return [SC_MESSAGE.ERROR_51_CODE, SC_MESSAGE.ERROR_51_TITLE, SC_MESSAGE.ERROR_51_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_59_CODE and type == SC_CODE.ERROR_59_TYPE:
            return [SC_MESSAGE.ERROR_59_CODE, SC_MESSAGE.ERROR_59_TITLE, SC_MESSAGE.ERROR_59_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_61_CODE and type == SC_CODE.ERROR_61_TYPE:
            return [SC_MESSAGE.ERROR_61_CODE, SC_MESSAGE.ERROR_61_TITLE, SC_MESSAGE.ERROR_61_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_69_CODE and type == SC_CODE.ERROR_69_TYPE:
            return [SC_MESSAGE.ERROR_69_CODE, SC_MESSAGE.ERROR_69_TITLE, SC_MESSAGE.ERROR_69_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_79_CODE and type == SC_CODE.ERROR_79_TYPE:
            return [SC_MESSAGE.ERROR_79_CODE, SC_MESSAGE.ERROR_79_TITLE, SC_MESSAGE.ERROR_79_DESCRIPTION, ScErrorType.Error]
        elif code == SC_CODE.ERROR_81_CODE and type == SC_CODE.ERROR_81_TYPE:
            return [SC_MESSAGE.ERROR_81_CODE, SC_MESSAGE.ERROR_81_TITLE, SC_MESSAGE.ERROR_81_DESCRIPTION, ScErrorType.Error]
        return ["", "", "", ScErrorType.none]

