import sys

from PyQt6.QtCore import QSize, Qt
from PyQt6.QtWidgets import QApplication, QMainWindow, QPushButton, QHBoxLayout, QWidget


# Подкласс QMainWindow для настройки главного окна приложения
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("OksEngine Control")

        self.layout = QHBoxLayout()

        self.generateBuildFilesButton = QPushButton("Generate build files")
        self.generateBuildFilesButton.setCheckable(True)
        self.generateBuildFilesButton.clicked.connect(self.GenerateBuildFilesButtonClicked)

        self.runUnitTestsButtonC = QPushButton("Run unit tests")
        self.runUnitTestsButtonC.setCheckable(True)
        self.runUnitTestsButtonC.clicked.connect(self.RunUnitTestsButtonClicked)


        self.layout.addWidget(self.generateBuildFilesButton)
        self.layout.addWidget(self.runUnitTestsButtonC)

        self.container = QWidget()
        self.container.setLayout(self.layout)
        # Устанавливаем центральный виджет Window.
        self.setCentralWidget(self.container)
        
    def GenerateBuildFilesButtonClicked(self):
        print("GenerateBuildFiles")

    def RunUnitTestsButtonClicked(self):
        print("Run unit tests")