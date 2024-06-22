import sys

from PyQt6.QtCore import QSize, Qt
from PyQt6.QtGui import QIcon
from PyQt6.QtWidgets import QApplication, QMainWindow, QPushButton, QHBoxLayout, QWidget
import subprocess
import os
import sys

# Подкласс QMainWindow для настройки главного окна приложения
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("OksEngine Control")

        self.layout = QHBoxLayout()
        self.generateBuildFilesButton = QPushButton(
            icon=QIcon("./generateBuildFiles.svg"),
            parent=self
        )
        self.generateBuildFilesButton.setFixedSize(100, 60)
        self.generateBuildFilesButton.setIconSize(QSize(40, 40))
        self.generateBuildFilesButton.setCheckable(True)
        self.generateBuildFilesButton.clicked.connect(self.GenerateBuildFilesButtonClicked)

        self.runUnitTestsButtonC = QPushButton(
            icon=QIcon("./runUnitTests.svg"),
            parent=self
        )
        self.runUnitTestsButtonC.setFixedSize(100, 60)
        self.runUnitTestsButtonC.setIconSize(QSize(40, 40))
        self.runUnitTestsButtonC.setCheckable(True)
        self.runUnitTestsButtonC.clicked.connect(self.RunUnitTestsButtonClicked)
                 
        self.configureProjectButton = QPushButton(
            icon=QIcon("./configureProject.svg"),
            parent=self
        )
        self.configureProjectButton.setFixedSize(100, 60)
        self.configureProjectButton.setIconSize(QSize(40, 40))
        self.configureProjectButton.setCheckable(True)
        self.configureProjectButton.clicked.connect(self.ConfigureProjectButtonClicked)

        self.loadDependenciesButton = QPushButton(
            icon=QIcon("./loadDependencies.svg"),
            parent=self
        )
        self.loadDependenciesButton.setFixedSize(100, 60)
        self.loadDependenciesButton.setIconSize(QSize(40, 40))
        self.loadDependenciesButton.setCheckable(True)
        self.loadDependenciesButton.clicked.connect(self.LoadDependenciesButtonClicked)


        self.layout.addWidget(self.generateBuildFilesButton)
        self.layout.addWidget(self.runUnitTestsButtonC)
        self.layout.addWidget(self.configureProjectButton)
        self.layout.addWidget(self.loadDependenciesButton)

        self.container = QWidget()
        self.container.setLayout(self.layout)
        # Устанавливаем центральный виджет Window.
        self.setCentralWidget(self.container)
        
    def GenerateBuildFilesButtonClicked(self):
        currentDirectory = os.path.dirname(os.path.abspath(sys.argv[0]))
        print(subprocess.run([currentDirectory + "/../../../Scripts/GenerateBuildFiles.sh"], shell=True))

    def RunUnitTestsButtonClicked(self):
        currentDirectory = os.path.dirname(os.path.abspath(sys.argv[0]))
        print(subprocess.run([currentDirectory + "/../../../Scripts/Test.sh"], shell=True))

    def ConfigureProjectButtonClicked(self):
        currentDirectory = os.path.dirname(os.path.abspath(sys.argv[0]))
        print(subprocess.run([currentDirectory + "/../../../Scripts/ConfigureProject.sh"], shell=True))

    def LoadDependenciesButtonClicked(self):
        currentDirectory = os.path.dirname(os.path.abspath(sys.argv[0]))
        print(subprocess.run([currentDirectory + "/../../../Scripts/LoadDependencies.sh"], shell=True))