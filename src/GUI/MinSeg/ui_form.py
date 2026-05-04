# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'form.ui'
##
## Created by: Qt User Interface Compiler version 6.11.0
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
    QMainWindow, QMenuBar, QPushButton, QSizePolicy,
    QSlider, QSpacerItem, QStatusBar, QVBoxLayout,
    QWidget)

from pyqtgraph import PlotWidget

class Ui_MinSegGUI(object):
    def setupUi(self, MinSegGUI):
        if not MinSegGUI.objectName():
            MinSegGUI.setObjectName(u"MinSegGUI")
        MinSegGUI.resize(800, 600)
        self.centralwidget = QWidget(MinSegGUI)
        self.centralwidget.setObjectName(u"centralwidget")
        self.horizontalLayout = QHBoxLayout(self.centralwidget)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.btn_start = QPushButton(self.centralwidget)
        self.btn_start.setObjectName(u"btn_start")

        self.horizontalLayout_2.addWidget(self.btn_start)

        self.btn_stop = QPushButton(self.centralwidget)
        self.btn_stop.setObjectName(u"btn_stop")

        self.horizontalLayout_2.addWidget(self.btn_stop)

        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_2.addItem(self.horizontalSpacer_2)

        self.label_status = QLabel(self.centralwidget)
        self.label_status.setObjectName(u"label_status")

        self.horizontalLayout_2.addWidget(self.label_status)


        self.verticalLayout.addLayout(self.horizontalLayout_2)

        self.horizontalLayout_4 = QHBoxLayout()
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")
        self.verticalLayout_6 = QVBoxLayout()
        self.verticalLayout_6.setObjectName(u"verticalLayout_6")
        self.label_angle = QLabel(self.centralwidget)
        self.label_angle.setObjectName(u"label_angle")

        self.verticalLayout_6.addWidget(self.label_angle)

        self.label_pwm = QLabel(self.centralwidget)
        self.label_pwm.setObjectName(u"label_pwm")

        self.verticalLayout_6.addWidget(self.label_pwm)

        self.label_rate = QLabel(self.centralwidget)
        self.label_rate.setObjectName(u"label_rate")

        self.verticalLayout_6.addWidget(self.label_rate)


        self.horizontalLayout_4.addLayout(self.verticalLayout_6)


        self.verticalLayout.addLayout(self.horizontalLayout_4)

        self.graph_1 = PlotWidget(self.centralwidget)
        self.graph_1.setObjectName(u"graph_1")
        self.widget = QWidget(self.graph_1)
        self.widget.setObjectName(u"widget")
        self.widget.setGeometry(QRect(0, 120, 769, 113))
        self.gridLayout = QGridLayout(self.widget)
        self.gridLayout.setObjectName(u"gridLayout")

        self.verticalLayout.addWidget(self.graph_1)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.verticalLayout.addItem(self.horizontalSpacer)

        self.label_4 = QLabel(self.centralwidget)
        self.label_4.setObjectName(u"label_4")

        self.verticalLayout.addWidget(self.label_4)

        self.Slider_k = QSlider(self.centralwidget)
        self.Slider_k.setObjectName(u"Slider_k")
        self.Slider_k.setOrientation(Qt.Orientation.Horizontal)

        self.verticalLayout.addWidget(self.Slider_k)


        self.horizontalLayout.addLayout(self.verticalLayout)

        MinSegGUI.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MinSegGUI)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 800, 33))
        MinSegGUI.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MinSegGUI)
        self.statusbar.setObjectName(u"statusbar")
        MinSegGUI.setStatusBar(self.statusbar)

        self.retranslateUi(MinSegGUI)

        QMetaObject.connectSlotsByName(MinSegGUI)
    # setupUi

    def retranslateUi(self, MinSegGUI):
        MinSegGUI.setWindowTitle(QCoreApplication.translate("MinSegGUI", u"MinSegGUI", None))
        self.btn_start.setText(QCoreApplication.translate("MinSegGUI", u"Start", None))
        self.btn_stop.setText(QCoreApplication.translate("MinSegGUI", u"Stop", None))
        self.label_status.setText(QCoreApplication.translate("MinSegGUI", u"Status: ", None))
        self.label_angle.setText(QCoreApplication.translate("MinSegGUI", u"Angle: ", None))
        self.label_pwm.setText(QCoreApplication.translate("MinSegGUI", u"PWM:", None))
        self.label_rate.setText(QCoreApplication.translate("MinSegGUI", u"Rate:", None))
        self.label_4.setText(QCoreApplication.translate("MinSegGUI", u"Reference", None))
    # retranslateUi

