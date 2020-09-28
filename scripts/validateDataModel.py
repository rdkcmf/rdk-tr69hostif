##########################################################################
# If not stated otherwise in this file or this component's LICENSE
# file the following copyright and licenses apply:
#
# Copyright 2016 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################

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
                print (error.message, error.line, error.column)
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
