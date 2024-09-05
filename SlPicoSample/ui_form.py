# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'form.ui'
##
## Created by: Qt User Interface Compiler version 6.7.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QComboBox, QFrame, QGridLayout,
    QHBoxLayout, QLabel, QPlainTextEdit, QPushButton,
    QSizePolicy, QSlider, QSpacerItem, QVBoxLayout,
    QWidget)

class Ui_SlPicoSample(object):
    def setupUi(self, SlPicoSample):
        if not SlPicoSample.objectName():
            SlPicoSample.setObjectName(u"SlPicoSample")
        SlPicoSample.resize(714, 345)
        self.verticalLayout = QVBoxLayout(SlPicoSample)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.widget = QWidget(SlPicoSample)
        self.widget.setObjectName(u"widget")
        self.horizontalLayout = QHBoxLayout(self.widget)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.label = QLabel(self.widget)
        self.label.setObjectName(u"label")
        font = QFont()
        font.setPointSize(12)
        font.setBold(True)
        self.label.setFont(font)

        self.horizontalLayout.addWidget(self.label)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)

        self.btnRefresh = QPushButton(self.widget)
        self.btnRefresh.setObjectName(u"btnRefresh")

        self.horizontalLayout.addWidget(self.btnRefresh)


        self.verticalLayout.addWidget(self.widget)

        self.line = QFrame(SlPicoSample)
        self.line.setObjectName(u"line")
        self.line.setStyleSheet(u"")
        self.line.setLineWidth(1)
        self.line.setFrameShape(QFrame.Shape.HLine)
        self.line.setFrameShadow(QFrame.Shadow.Sunken)

        self.verticalLayout.addWidget(self.line)

        self.widget_2 = QWidget(SlPicoSample)
        self.widget_2.setObjectName(u"widget_2")
        self.gridLayout = QGridLayout(self.widget_2)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_4 = QLabel(self.widget_2)
        self.label_4.setObjectName(u"label_4")
        self.label_4.setMinimumSize(QSize(60, 0))
        font1 = QFont()
        font1.setBold(True)
        self.label_4.setFont(font1)
        self.label_4.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_4, 0, 5, 1, 1)

        self.label_3 = QLabel(self.widget_2)
        self.label_3.setObjectName(u"label_3")
        self.label_3.setMinimumSize(QSize(60, 0))
        self.label_3.setFont(font1)
        self.label_3.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_3, 0, 3, 1, 1)

        self.label_2 = QLabel(self.widget_2)
        self.label_2.setObjectName(u"label_2")
        self.label_2.setMinimumSize(QSize(60, 0))
        self.label_2.setFont(font1)
        self.label_2.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_2, 0, 0, 1, 1)

        self.lbStatus = QLabel(self.widget_2)
        self.lbStatus.setObjectName(u"lbStatus")
        self.lbStatus.setMinimumSize(QSize(120, 0))
        self.lbStatus.setMaximumSize(QSize(16777215, 16777215))
        self.lbStatus.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.lbStatus, 0, 6, 1, 1)

        self.label_5 = QLabel(self.widget_2)
        self.label_5.setObjectName(u"label_5")
        self.label_5.setMinimumSize(QSize(60, 0))
        self.label_5.setFont(font1)
        self.label_5.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_5, 1, 0, 1, 1)

        self.widget_3 = QWidget(self.widget_2)
        self.widget_3.setObjectName(u"widget_3")
        self.horizontalLayout_2 = QHBoxLayout(self.widget_3)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.btnPowerOff = QPushButton(self.widget_3)
        self.btnPowerOff.setObjectName(u"btnPowerOff")
        self.btnPowerOff.setMinimumSize(QSize(0, 0))
        self.btnPowerOff.setMaximumSize(QSize(16777215, 16777215))

        self.horizontalLayout_2.addWidget(self.btnPowerOff)

        self.btnPowerOn = QPushButton(self.widget_3)
        self.btnPowerOn.setObjectName(u"btnPowerOn")
        self.btnPowerOn.setMinimumSize(QSize(0, 0))
        self.btnPowerOn.setMaximumSize(QSize(16777215, 16777215))

        self.horizontalLayout_2.addWidget(self.btnPowerOn)


        self.gridLayout.addWidget(self.widget_3, 1, 2, 1, 1)

        self.label_6 = QLabel(self.widget_2)
        self.label_6.setObjectName(u"label_6")
        self.label_6.setMinimumSize(QSize(60, 0))
        self.label_6.setFont(font1)
        self.label_6.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_6, 1, 3, 1, 1)

        self.widget_4 = QWidget(self.widget_2)
        self.widget_4.setObjectName(u"widget_4")
        self.horizontalLayout_3 = QHBoxLayout(self.widget_4)
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.btnLaserOff = QPushButton(self.widget_4)
        self.btnLaserOff.setObjectName(u"btnLaserOff")
        self.btnLaserOff.setMinimumSize(QSize(0, 0))

        self.horizontalLayout_3.addWidget(self.btnLaserOff)

        self.btnLaserOn = QPushButton(self.widget_4)
        self.btnLaserOn.setObjectName(u"btnLaserOn")
        self.btnLaserOn.setMinimumSize(QSize(0, 0))

        self.horizontalLayout_3.addWidget(self.btnLaserOn)


        self.gridLayout.addWidget(self.widget_4, 1, 4, 1, 1)

        self.label_7 = QLabel(self.widget_2)
        self.label_7.setObjectName(u"label_7")
        self.label_7.setMinimumSize(QSize(60, 0))
        self.label_7.setFont(font1)
        self.label_7.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_7, 1, 5, 1, 1)

        self.lbTemperature = QLabel(self.widget_2)
        self.lbTemperature.setObjectName(u"lbTemperature")
        self.lbTemperature.setMinimumSize(QSize(120, 0))
        self.lbTemperature.setMaximumSize(QSize(16777215, 16777215))
        self.lbTemperature.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.lbTemperature, 1, 6, 1, 1)

        self.label_8 = QLabel(self.widget_2)
        self.label_8.setObjectName(u"label_8")
        self.label_8.setMinimumSize(QSize(60, 0))
        self.label_8.setFont(font1)
        self.label_8.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_8, 2, 0, 1, 1)

        self.widget_5 = QWidget(self.widget_2)
        self.widget_5.setObjectName(u"widget_5")
        self.horizontalLayout_4 = QHBoxLayout(self.widget_5)
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")
        self.cbFrequency = QComboBox(self.widget_5)
        self.cbFrequency.setObjectName(u"cbFrequency")
        self.cbFrequency.setMaximumSize(QSize(16777215, 16777215))

        self.horizontalLayout_4.addWidget(self.cbFrequency)

        self.btnFrequency = QPushButton(self.widget_5)
        self.btnFrequency.setObjectName(u"btnFrequency")
        self.btnFrequency.setMaximumSize(QSize(16777215, 16777215))

        self.horizontalLayout_4.addWidget(self.btnFrequency)


        self.gridLayout.addWidget(self.widget_5, 2, 2, 1, 1)

        self.label_9 = QLabel(self.widget_2)
        self.label_9.setObjectName(u"label_9")
        self.label_9.setMinimumSize(QSize(60, 0))
        self.label_9.setFont(font1)
        self.label_9.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_9, 2, 5, 1, 1)

        self.lbError = QLabel(self.widget_2)
        self.lbError.setObjectName(u"lbError")
        self.lbError.setMinimumSize(QSize(120, 0))
        self.lbError.setMaximumSize(QSize(16777215, 16777215))
        self.lbError.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.lbError, 2, 6, 1, 1)

        self.widget_7 = QWidget(self.widget_2)
        self.widget_7.setObjectName(u"widget_7")
        self.horizontalLayout_6 = QHBoxLayout(self.widget_7)
        self.horizontalLayout_6.setObjectName(u"horizontalLayout_6")
        self.cbType = QComboBox(self.widget_7)
        self.cbType.setObjectName(u"cbType")
        self.cbType.setMinimumSize(QSize(0, 0))
        self.cbType.setMaximumSize(QSize(16777215, 16777215))

        self.horizontalLayout_6.addWidget(self.cbType)


        self.gridLayout.addWidget(self.widget_7, 0, 2, 1, 1)

        self.widget_8 = QWidget(self.widget_2)
        self.widget_8.setObjectName(u"widget_8")
        self.horizontalLayout_7 = QHBoxLayout(self.widget_8)
        self.horizontalLayout_7.setObjectName(u"horizontalLayout_7")
        self.cbPort = QComboBox(self.widget_8)
        self.cbPort.setObjectName(u"cbPort")
        self.cbPort.setMaximumSize(QSize(16777215, 16777215))

        self.horizontalLayout_7.addWidget(self.cbPort)


        self.gridLayout.addWidget(self.widget_8, 0, 4, 1, 1)


        self.verticalLayout.addWidget(self.widget_2)

        self.widget_9 = QWidget(SlPicoSample)
        self.widget_9.setObjectName(u"widget_9")
        self.horizontalLayout_8 = QHBoxLayout(self.widget_9)
        self.horizontalLayout_8.setObjectName(u"horizontalLayout_8")
        self.label_10 = QLabel(self.widget_9)
        self.label_10.setObjectName(u"label_10")
        self.label_10.setFont(font1)

        self.horizontalLayout_8.addWidget(self.label_10)


        self.verticalLayout.addWidget(self.widget_9)

        self.widget_6 = QWidget(SlPicoSample)
        self.widget_6.setObjectName(u"widget_6")
        self.horizontalLayout_5 = QHBoxLayout(self.widget_6)
        self.horizontalLayout_5.setObjectName(u"horizontalLayout_5")
        self.editPower = QPlainTextEdit(self.widget_6)
        self.editPower.setObjectName(u"editPower")
        self.editPower.setMaximumSize(QSize(100, 30))

        self.horizontalLayout_5.addWidget(self.editPower)

        self.btnPowerSet = QPushButton(self.widget_6)
        self.btnPowerSet.setObjectName(u"btnPowerSet")

        self.horizontalLayout_5.addWidget(self.btnPowerSet)

        self.sliderLaserPower = QSlider(self.widget_6)
        self.sliderLaserPower.setObjectName(u"sliderLaserPower")
        self.sliderLaserPower.setOrientation(Qt.Horizontal)

        self.horizontalLayout_5.addWidget(self.sliderLaserPower)


        self.verticalLayout.addWidget(self.widget_6)

        self.lbMessage = QLabel(SlPicoSample)
        self.lbMessage.setObjectName(u"lbMessage")

        self.verticalLayout.addWidget(self.lbMessage)


        self.retranslateUi(SlPicoSample)

        QMetaObject.connectSlotsByName(SlPicoSample)
    # setupUi

    def retranslateUi(self, SlPicoSample):
        SlPicoSample.setWindowTitle(QCoreApplication.translate("SlPicoSample", u"SL-Pico Sample", None))
        self.label.setText(QCoreApplication.translate("SlPicoSample", u"LASER", None))
        self.btnRefresh.setText(QCoreApplication.translate("SlPicoSample", u"refresh", None))
        self.label_4.setText(QCoreApplication.translate("SlPicoSample", u"Status", None))
        self.label_3.setText(QCoreApplication.translate("SlPicoSample", u"Port", None))
        self.label_2.setText(QCoreApplication.translate("SlPicoSample", u"Type", None))
        self.lbStatus.setText(QCoreApplication.translate("SlPicoSample", u"Off", None))
        self.label_5.setText(QCoreApplication.translate("SlPicoSample", u"Power", None))
        self.btnPowerOff.setText(QCoreApplication.translate("SlPicoSample", u"OFF", None))
        self.btnPowerOn.setText(QCoreApplication.translate("SlPicoSample", u"ON", None))
        self.label_6.setText(QCoreApplication.translate("SlPicoSample", u"LASER", None))
        self.btnLaserOff.setText(QCoreApplication.translate("SlPicoSample", u"OFF", None))
        self.btnLaserOn.setText(QCoreApplication.translate("SlPicoSample", u"ON", None))
        self.label_7.setText(QCoreApplication.translate("SlPicoSample", u"Temp.", None))
        self.lbTemperature.setText(QCoreApplication.translate("SlPicoSample", u"0\u00b0C", None))
        self.label_8.setText(QCoreApplication.translate("SlPicoSample", u"Freq.", None))
        self.btnFrequency.setText(QCoreApplication.translate("SlPicoSample", u"SET", None))
        self.label_9.setText(QCoreApplication.translate("SlPicoSample", u"Error", None))
        self.lbError.setText("")
        self.label_10.setText(QCoreApplication.translate("SlPicoSample", u"LASER Power", None))
        self.btnPowerSet.setText(QCoreApplication.translate("SlPicoSample", u"SET", None))
        self.lbMessage.setText("")
    # retranslateUi

