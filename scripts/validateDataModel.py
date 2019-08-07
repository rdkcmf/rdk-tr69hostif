# Python script to validate xml file with its corresponsding xsd schema

import sys
from lxml import etree

# Python-lxml utility class for validating xml file with xsd schema
class Validator:
    def __init__(self,xsd_path):
        xmlschema_doc = etree.parse(xsd_path)
        self.xmlschema = etree.XMLSchema(xmlschema_doc)

    def validate(self, xml_path) :
        xml_doc = etree.parse(xml_path)
        result = self.xmlschema.validate(xml_doc)
        if not result:
            for error in self.xmlschema.error_log:
                print error.message, error.line, error.column
        return result


# Validate xml based on cwmp xsd file
if len (sys.argv) != 3 :
    print ('Usage: python validateDataModel.py <XML_FILE_PATH> <XSD_FILE_PATH>')
    raise Exception ('Invalid arguments to validateDataModel.py')
else:
    xml_file_path=sys.argv[1]
    xsd_file_path=sys.argv[2]
    validator = Validator(xsd_file_path)

if validator.validate(xml_file_path):
    print('Data-model is valid')
else:
    print('data-model.xml is not valid');
    raise Exception('data-model.xml is not matching with the cwmp schema')
