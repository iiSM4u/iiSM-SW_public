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
from PySide6.QtWidgets import (QApplication, QGridLayout, QHBoxLayout, QLabel,
    QLineEdit, QPushButton, QSizePolicy, QSpacerItem,
    QVBoxLayout, QWidget)

class Ui_Widget(object):
    def setupUi(self, Widget):
        if not Widget.objectName():
            Widget.setObjectName(u"Widget")
        Widget.resize(345, 384)
        self.verticalLayout = QVBoxLayout(Widget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.widget = QWidget(Widget)
        self.widget.setObjectName(u"widget")
        self.horizontalLayout = QHBoxLayout(self.widget)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.lbPath = QLabel(self.widget)
        self.lbPath.setObjectName(u"lbPath")

        self.horizontalLayout.addWidget(self.lbPath)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)

        self.btnPath = QPushButton(self.widget)
        self.btnPath.setObjectName(u"btnPath")

        self.horizontalLayout.addWidget(self.btnPath)


        self.verticalLayout.addWidget(self.widget)

        self.widget_2 = QWidget(Widget)
        self.widget_2.setObjectName(u"widget_2")
        self.horizontalLayout_2 = QHBoxLayout(self.widget_2)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.btnConnect = QPushButton(self.widget_2)
        self.btnConnect.setObjectName(u"btnConnect")

        self.horizontalLayout_2.addWidget(self.btnConnect)

        self.btnDisconnect = QPushButton(self.widget_2)
        self.btnDisconnect.setObjectName(u"btnDisconnect")

        self.horizontalLayout_2.addWidget(self.btnDisconnect)


        self.verticalLayout.addWidget(self.widget_2)

        self.widget_3 = QWidget(Widget)
        self.widget_3.setObjectName(u"widget_3")
        self.gridLayout = QGridLayout(self.widget_3)
        self.gridLayout.setObjectName(u"gridLayout")
        self.lbSerialNoTitle = QLabel(self.widget_3)
        self.lbSerialNoTitle.setObjectName(u"lbSerialNoTitle")
        self.lbSerialNoTitle.setMaximumSize(QSize(80, 16777215))
        font = QFont()
        font.setBold(True)
        self.lbSerialNoTitle.setFont(font)
        self.lbSerialNoTitle.setLayoutDirection(Qt.LeftToRight)
        self.lbSerialNoTitle.setAlignment(Qt.AlignRight|Qt.AlignTrailing|Qt.AlignVCenter)

        self.gridLayout.addWidget(self.lbSerialNoTitle, 1, 0, 1, 1)

        self.lbModel = QLabel(self.widget_3)
        self.lbModel.setObjectName(u"lbModel")

        self.gridLayout.addWidget(self.lbModel, 0, 1, 1, 1)

        self.lbSerialNo = QLabel(self.widget_3)
        self.lbSerialNo.setObjectName(u"lbSerialNo")

        self.gridLayout.addWidget(self.lbSerialNo, 1, 1, 1, 1)

        self.lbWaveRangeTitle = QLabel(self.widget_3)
        self.lbWaveRangeTitle.setObjectName(u"lbWaveRangeTitle")
        self.lbWaveRangeTitle.setMaximumSize(QSize(80, 16777215))
        self.lbWaveRangeTitle.setFont(font)
        self.lbWaveRangeTitle.setAlignment(Qt.AlignRight|Qt.AlignTrailing|Qt.AlignVCenter)

        self.gridLayout.addWidget(self.lbWaveRangeTitle, 2, 0, 1, 1)

        self.lbModelTitle = QLabel(self.widget_3)
        self.lbModelTitle.setObjectName(u"lbModelTitle")
        self.lbModelTitle.setMaximumSize(QSize(80, 16777215))
        self.lbModelTitle.setFont(font)
        self.lbModelTitle.setAlignment(Qt.AlignRight|Qt.AlignTrailing|Qt.AlignVCenter)

        self.gridLayout.addWidget(self.lbModelTitle, 0, 0, 1, 1)

        self.lbWaveRange = QLabel(self.widget_3)
        self.lbWaveRange.setObjectName(u"lbWaveRange")

        self.gridLayout.addWidget(self.lbWaveRange, 2, 1, 1, 1)


        self.verticalLayout.addWidget(self.widget_3)

        self.widget_5 = QWidget(Widget)
        self.widget_5.setObjectName(u"widget_5")
        self.gridLayout_2 = QGridLayout(self.widget_5)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.lbCwlTitle = QLabel(self.widget_5)
        self.lbCwlTitle.setObjectName(u"lbCwlTitle")
        self.lbCwlTitle.setMaximumSize(QSize(60, 16777215))
        self.lbCwlTitle.setFont(font)
        self.lbCwlTitle.setAlignment(Qt.AlignRight|Qt.AlignTrailing|Qt.AlignVCenter)

        self.gridLayout_2.addWidget(self.lbCwlTitle, 0, 0, 1, 1)

        self.editCwl = QLineEdit(self.widget_5)
        self.editCwl.setObjectName(u"editCwl")

        self.gridLayout_2.addWidget(self.editCwl, 0, 1, 1, 1)

        self.lbFwhmTitle = QLabel(self.widget_5)
        self.lbFwhmTitle.setObjectName(u"lbFwhmTitle")
        self.lbFwhmTitle.setMaximumSize(QSize(60, 16777215))
        self.lbFwhmTitle.setFont(font)
        self.lbFwhmTitle.setAlignment(Qt.AlignRight|Qt.AlignTrailing|Qt.AlignVCenter)

        self.gridLayout_2.addWidget(self.lbFwhmTitle, 1, 0, 1, 1)

        self.editFwhm = QLineEdit(self.widget_5)
        self.editFwhm.setObjectName(u"editFwhm")

        self.gridLayout_2.addWidget(self.editFwhm, 1, 1, 1, 1)


        self.verticalLayout.addWidget(self.widget_5)

        self.widget_4 = QWidget(Widget)
        self.widget_4.setObjectName(u"widget_4")
        self.horizontalLayout_3 = QHBoxLayout(self.widget_4)
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.btnGo = QPushButton(self.widget_4)
        self.btnGo.setObjectName(u"btnGo")

        self.horizontalLayout_3.addWidget(self.btnGo)

        self.btnBlank = QPushButton(self.widget_4)
        self.btnBlank.setObjectName(u"btnBlank")

        self.horizontalLayout_3.addWidget(self.btnBlank)


        self.verticalLayout.addWidget(self.widget_4)

        self.widget_6 = QWidget(Widget)
        self.widget_6.setObjectName(u"widget_6")
        self.verticalLayout_2 = QVBoxLayout(self.widget_6)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.btnExit = QPushButton(self.widget_6)
        self.btnExit.setObjectName(u"btnExit")

        self.verticalLayout_2.addWidget(self.btnExit)


        self.verticalLayout.addWidget(self.widget_6)

        self.lbStatus = QLabel(Widget)
        self.lbStatus.setObjectName(u"lbStatus")

        self.verticalLayout.addWidget(self.lbStatus)


        self.retranslateUi(Widget)

        QMetaObject.connectSlotsByName(Widget)
    # setupUi

    def retranslateUi(self, Widget):
        Widget.setWindowTitle(QCoreApplication.translate("Widget", u"Poly Sample", None))
        self.lbPath.setText(QCoreApplication.translate("Widget", u"File Path", None))
        self.btnPath.setText(QCoreApplication.translate("Widget", u"Open", None))
        self.btnConnect.setText(QCoreApplication.translate("Widget", u"Connect", None))
        self.btnDisconnect.setText(QCoreApplication.translate("Widget", u"Disconnect", None))
        self.lbSerialNoTitle.setText(QCoreApplication.translate("Widget", u"Serial No", None))
        self.lbModel.setText(QCoreApplication.translate("Widget", u"Model", None))
        self.lbSerialNo.setText(QCoreApplication.translate("Widget", u"Serial No", None))
        self.lbWaveRangeTitle.setText(QCoreApplication.translate("Widget", u"Wave Range", None))
        self.lbModelTitle.setText(QCoreApplication.translate("Widget", u"Model", None))
        self.lbWaveRange.setText(QCoreApplication.translate("Widget", u"Wave Range", None))
        self.lbCwlTitle.setText(QCoreApplication.translate("Widget", u"CWL", None))
        self.lbFwhmTitle.setText(QCoreApplication.translate("Widget", u"FWHM", None))
        self.btnGo.setText(QCoreApplication.translate("Widget", u"Go", None))
        self.btnBlank.setText(QCoreApplication.translate("Widget", u"Blank Pos", None))
        self.btnExit.setText(QCoreApplication.translate("Widget", u"Exit", None))
        self.lbStatus.setText(QCoreApplication.translate("Widget", u"status...", None))
    # retranslateUi

