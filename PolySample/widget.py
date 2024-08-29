# This Python file uses the following encoding: utf-8
import sys

# pip install pythonnet
import clr

# dll directory path
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

    def on_btnPath_click(self) -> None:
        file_name, _ = QFileDialog.getOpenFileName(self, "Open ISM File", "", "ISM files (*.ism)", options=QFileDialog.Options())
        if file_name:
            self.ui.lbPath.setText(file_name)

    def on_btnConnect_click(self) -> None:
        self.setEnableUI(False)
        num = self.connect(self.ui.lbStatus.text())
        self.ui.lbStatus.setText(self.poly.GetStringMsg(num))
        self.setEnableUI(True)

    def on_btnDisconnect_click(self) -> None:
        self.setEnableUI(False)
        num = self.disconnect()
        self.ui.lbStatus.setText(self.poly.GetStringMsg(num))
        self.setEnableUI(True)

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

    def connect(self, path: str) -> int:
        return self.poly.PolyConnect(path)

    def disconnect(self, path: str) -> int:
        return self.poly.Disconnect()

    def on_poly_update(self, sender, num: int) -> None:
        try:
            message = self.poly.GetStringMsg(num)
            self.ui.lbStatus.setText(message.name)
        except ValueError:
            print(f"Unknown message code: {num}")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = Widget()
    widget.show()
    sys.exit(app.exec())





# 아래 것들 다 가져와야 함
    # public (PolyMessage, string) GetDeviceStatus(int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         int msg = poly.GetDeviceStatus();
    #         return ((PolyMessage)msg, poly.GetStringMsg(msg));
    #     }
    #     return (PolyMessage.MSG_NO_ERROR, string.Empty);
    # }

    # public (PolyMessage, string) DeviceResetAll()
    # {
    #     if (_polyList.Count > 0)
    #     {
    #         foreach (ClassPoly poly in _polyList)
    #         {
    #             int msg = poly.DeviceReset();

    #             if (msg < 0) // 0 보다 작으면 에러. 에러면 에러난 위치에서 중지.
    #             {
    #                 return ((PolyMessage)msg, poly.GetStringMsg(msg));
    #             }
    #         }

    #         return (PolyMessage.MSG_NO_ERROR, string.Empty);
    #     }
    #     return (PolyMessage.ERR_FAILED, string.Empty);
    # }

    # public (PolyMessage, string) DeviceReset(int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         int msg = poly.DeviceReset();
    #         return ((PolyMessage)msg, poly.GetStringMsg(msg));
    #     }
    #     return (PolyMessage.ERR_FAILED, string.Empty);
    # }

    # public (string, string) GetDeviceList(int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         string deviceList = string.Empty;
    #         int msg = poly.GetDeviceList(deviceList: ref deviceList);
    #         string text = poly.GetStringMsg(msg);
    #         return (deviceList, text);
    #     }
    #     return (string.Empty, string.Empty);
    # }

    # public (string, string, string, string) GetInfoData(int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         string model = string.Empty, serial = string.Empty, range = string.Empty;
    #         int msg = poly.GetInforData(model: ref model, serial: ref serial, range: ref range);
    #         return (model, serial, range, poly.GetStringMsg(msg));
    #     }
    #     return (string.Empty, string.Empty, string.Empty, string.Empty);
    # }

    # public bool GetDeviceEnabled(int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         return poly.GetDeviceEnabled();
    #     }
    #     return false;
    # }

    # public string GetComPortNumber(int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         return poly.GetComPortNumber();
    #     }
    #     return string.Empty;
    # }

    # public (double, double) GetStartEndNM(int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         return (poly.StartPoint_nm, poly.EndPoint_nm);
    #     }
    #     return (0d, 0d);
    # }

    # public (PolyMessage, string) GoBlankPositionAll()
    # {
    #     if (_polyList.Count > 0)
    #     {
    #         foreach (ClassPoly poly in _polyList)
    #         {
    #             int msg = poly.GoBlankPosition();

    #             if (msg < 0) // 0 보다 작으면 에러. 에러면 에러난 위치에서 중지.
    #             {
    #                 return ((PolyMessage)msg, poly.GetStringMsg(msg));
    #             }
    #         }

    #         return (PolyMessage.MSG_NO_ERROR, string.Empty);
    #     }
    #     return (PolyMessage.ERR_FAILED, string.Empty);
    # }

    # public (PolyMessage, string) GoBlankPosition(int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         int msg = poly.GoBlankPosition();
    #         return ((PolyMessage)msg, poly.GetStringMsg(msg));
    #     }
    #     return (PolyMessage.ERR_FAILED, string.Empty);
    # }

    # public (double, double, double, double, string) GetCurrentWavelength(int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         string swStr = string.Empty, cwStr = string.Empty, lwStr = string.Empty, fwhmStr = string.Empty;
    #         int msg = poly.GetCurrentWavelength(sw: ref swStr, cw: ref cwStr, lw: ref lwStr, fwhm: ref fwhmStr);

    #         double.TryParse(swStr, out double sw);
    #         double.TryParse(cwStr, out double cw);
    #         double.TryParse(lwStr, out double lw);
    #         double.TryParse(fwhmStr, out double fwhm);

    #         return (sw, cw, lw, fwhm, poly.GetStringMsg(msg));
    #     }
    #     return (0d, 0d, 0d, 0d, string.Empty);
    # }

    # public (int, int, int, int, int, int, string) GetCurrentRaman(int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         int centerRaman = 0, bandwidthRaman = 0, startPoint = 0, endPoint = 0, bandwidthLeft = 0, bandwidtRight = 0;
    #         int msg = poly.GetCurrentRaman(CenterRaman: ref centerRaman, BandwidthRaman: ref bandwidthRaman, StartPoint: ref startPoint, EndPoint: ref endPoint, BandwidthLeft: ref bandwidthLeft, BandwidtRight: ref bandwidtRight);
    #         string text = poly.GetStringMsg(msg);
    #         return (centerRaman, bandwidthRaman, startPoint, endPoint, bandwidthLeft, bandwidtRight, text);
    #     }

    #     return (0, 0, 0, 0, 0, 0, string.Empty);
    # }

    # public (PolyMessage, string) SetWavelength(double cw, double fwhm, int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         int msg = poly.SetWavelength(CW: cw, FWHM: fwhm);
    #         return ((PolyMessage)msg, poly.GetStringMsg(msg));
    #     }
    #     return (PolyMessage.ERR_FAILED, string.Empty);
    # }

    # public (PolyMessage, string) SetRamanStokes(double iExcWavelength, int iCenterRaman, int iBandwidthRaman, int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         int msg = poly.SetRamanStokes(iExcWavelength: iExcWavelength, iCenterRaman: iCenterRaman, iBandwidthRaman: iBandwidthRaman);
    #         return ((PolyMessage)msg, poly.GetStringMsg(msg));
    #     }
    #     return (PolyMessage.ERR_FAILED, string.Empty);
    # }

    # public (PolyMessage, string) SetAntiRamanStokes(double iExcWavelength, int iCenterRaman, int iBandwidthRaman, int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         int msg = poly.SetAntiRamanStokes(iExcWavelength: iExcWavelength, iCenterRaman: iCenterRaman, iBandwidthRaman: iBandwidthRaman);
    #         return ((PolyMessage)msg, poly.GetStringMsg(msg));
    #     }
    #     return (PolyMessage.ERR_FAILED, string.Empty);
    # }

    # public (PolyMessage, string) ScanWavelength(double start, double end, double fwhm, double step, double delay, uint repeat = 0, int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         int msg = poly.ScanWavelength(start: start, end: end, fwhm: (int)fwhm, step: step, delay: delay, repeat: repeat);
    #         return ((PolyMessage)msg, poly.GetStringMsg(msg));
    #     }
    #     return (PolyMessage.ERR_FAILED, string.Empty);
    # }

    # public (PolyMessage, string) ScanWavelengthRaman(int wavelength, int start, int end, int bandwidth, int step, double delay, int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         int msg = poly.ScanWavelengthRaman(wavelength: wavelength, start: start, end: end, bandwidth: bandwidth, step: step, delay: delay);
    #         return ((PolyMessage)msg, poly.GetStringMsg(msg));
    #     }
    #     return (PolyMessage.ERR_FAILED, string.Empty);
    # }

    # public (PolyMessage, string) ScanWavelengthRaman(int wavelength, int start, int end, int bandwidth, int step, double delay, uint repeat, int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         int msg = poly.ScanWavelengthRaman(wavelength: wavelength, start: start, end: end, bandwidth: bandwidth, step: step, delay: delay, repeat: repeat);
    #         return ((PolyMessage)msg, poly.GetStringMsg(msg));
    #     }
    #     return (PolyMessage.ERR_FAILED, string.Empty);
    # }

    # public bool IsConnectionLost(PolyMessage result)
    # {
    #     return result == PolyMessage.ERR_DEVICE_NOT_CONNECTED ||
    #         result == PolyMessage.ERR_CONNECTION_ERROR ||
    #         result == PolyMessage.ERR_CONNECTION_LOST ||
    #         result == PolyMessage.ERR_COMM_TIMEOUT ||
    #         result == PolyMessage.ERR_COMM_ERROR;
    # }

    # public string GetDllVersion(int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         return poly.GetDllVer();
    #     }
    #     return string.Empty;
    # }

    # public (PolyMessage, string) DimmingControl(uint dimVal, int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         int msg = poly.DimmingControl(dimVal: dimVal);
    #         return ((PolyMessage)msg, poly.GetStringMsg(msg));
    #     }
    #     return (PolyMessage.ERR_FAILED, string.Empty);
    # }

    # public int GetDimVal(int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         return poly.GetDimmingVal();
    #     }
    #     return -1;
    # }

    # public (PolyMessage, string) GetDimVal(double movingWavelengthCont, int index = 0)
    # {
    #     if (TryGetPoly(index: index, poly: out ClassPoly poly))
    #     {
    #         int msg = poly.SetMovingWavelengthCont(MovingWavelengthCont: movingWavelengthCont);
    #         return ((PolyMessage)msg, poly.GetStringMsg(msg));
    #     }
    #     return (PolyMessage.ERR_FAILED, string.Empty);
    # }
