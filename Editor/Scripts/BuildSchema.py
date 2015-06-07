import os
import sys
import xml.etree.cElementTree as ET

START_REGION_STRING = "#pragma region Serializable"
END_REGION_STRING = "#pragma endregion"

paramTypeStringToEnum = { "int" : "0", "float" : "1", "bool" : "2", "string" : "3", "Vector3" : "4", "ColourRGB" : "5" }

def GetScriptList(projectFilepath):
    root = ET.parse(projectFilepath).getroot()

    settingsXML = root.find("Settings")
    assetPathXML = settingsXML.find("Resource-Root-Path")
    assetPath = assetPathXML.get("path")

    resourcesXML = root.find("Resources")
    scriptsXML = resourcesXML.find("Scripts")
    scriptsDict = {}
    for script in scriptsXML.iter("Script"):
        scriptsDict[script.get("guid")] = script.get("path")
    return [assetPath, scriptsDict]


def GetSerializableMembers(filePath):
    with open(filePath) as f:
        
        # Search for start of serializable region
        foundRegion = False
        line = f.readline()
        while(line):
            if (line.startswith(START_REGION_STRING)):
                foundRegion = True
                break
            line = f.readline()

        if (not foundRegion):
            print("Error: could not find start of Serializable region in " + filename)
            return

        # Accumulate serializable lines until end of region is found
        serializableLines = []
        foundEndRegion = False
        line = f.readline()
        while(line):
            if (line.startswith(END_REGION_STRING)):
                foundEndRegion = True
                break
            serializableLines.append(line.lstrip().rstrip(";\n"))
            line = f.readline()

        if (not foundEndRegion):
            print("Error: could not find end of Serializable region in "  + filename)
            return

        print("Successfully processed " + filePath)
        return serializableLines

def SetDefaultValue(rawValue, paramType, XMLelement):
    if (paramType == "0"):
        XMLelement.attrib["value"] = rawValue[0]
    elif (paramType == "1"):
        XMLelement.attrib["value"] = rawValue[0].rstrip(".f")
    elif (paramType == "2"):
        XMLelement.attrib["value"] = rawValue[0]
    elif (paramType == "3"):
        XMLelement.attrib["value"] = rawValue[0].strip('"')
    elif (paramType == "4"):
        XMLelement.attrib["value"] = ""
        ParseVector(rawValue, XMLelement)
    elif (paramType == "5"):
        XMLelement.attrib["value"] = ""
        ParseColor(rawValue, XMLelement)

def ParseVector(valueTokens, XMLelement):
    cleanTokens = [t.strip(",()f") for t in valueTokens]
    cleanTokens[0] = (cleanTokens[0])[len("Vector3("):]
    print(cleanTokens)
    ET.SubElement(XMLelement, "value", x = cleanTokens[0] , y = cleanTokens[1] , z = cleanTokens[2])

def ParseColor(valueTokens, XMLelement):
    cleanTokens = [t.strip(",()f") for t in valueTokens]
    cleanTokens[0] = (cleanTokens[0])[len("ColourRGB("):]
    print(cleanTokens)
    ET.SubElement(XMLelement, "value", r = cleanTokens[0] , g = cleanTokens[1] , b = cleanTokens[2])

def BuildSchemas(projectFilePath):
    # Create root XML elemnt
    rootXML = ET.Element("Scripts")

    # Get script list and asset path from project file
    assetPath, scriptsDict = GetScriptList(projectFilePath)

    # Process each script
    for guid, relativepath in scriptsDict.items():
        scriptXML = ET.SubElement(rootXML, "Script", guid = str(guid), path = relativepath)
        serializableMembers = GetSerializableMembers(assetPath + relativepath)
        if (serializableMembers is not None):
            for memberLine in serializableMembers:
                # Member should be in the form: {TYPE} {NAME} = {DEFAULTVALUE}
                tokens = [x for x in memberLine.split(' ') if x is not '']
                if (len(tokens) < 2):
                    continue
                paramType = paramTypeStringToEnum[tokens[0]]
                paramName = tokens[1]
                paramXML = ET.SubElement(scriptXML, "Param", type = paramType, name = paramName)
                if (len(tokens) >= 4 and tokens[2] == "="):
                    paramDefaultValue = tokens[3:]
                    print(paramDefaultValue)
                    SetDefaultValue(paramDefaultValue, paramType, paramXML)

    # Write XML to file
    tree = ET.ElementTree(rootXML)
    tree.write(assetPath + "Schema.xml")

if __name__ == "__main__":
    BuildSchemas(sys.argv[1])