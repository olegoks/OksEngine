import os
import yaml

def generate_cpp_code_by_ecs(ecsContent):
    print(ecsContent)
    yamlData = yaml.safe_load(ecsContent)
    if yamlData['System']:
        

def generate_cpp_code_from_ecs():
    currentDirectory = os.path.dirname(os.path.abspath(__file__))
    for element in os.listdir(currentDirectory):
        fullFilePath = currentDirectory + '\\' + element
        if os.path.isfile(fullFilePath):
            if element.endswith('.ecs'):
                ecsFile = open(fullFilePath, 'r')
                ecsFileContent = ecsFile.read()
                cppCode = generate_cpp_code_by_ecs(ecsFileContent)

generate_cpp_code_from_ecs()
